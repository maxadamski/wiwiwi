import com.datastax.driver.core.{Cluster, BoundStatement, ResultSet}
import cask.{Request, Response}
import java.text.{SimpleDateFormat, ParseException}
import java.util.Date
import java.time.temporal.ChronoUnit
import collection.JavaConverters._

object App extends cask.MainRoutes {
  override def port = 8080
  val dbHost = "localhost"
  val dbPort = 9042
  val dbName = "embedb"
  val maxRowsInResponse = 100_000
  val maxDateWindowSoft = 40

  val cluster = Cluster.builder().addContactPoint(dbHost).withPort(dbPort).build()
  val session = cluster.connect(dbName)
  val insertMes = session.prepare("INSERT INTO measurements (sensor_id, stamp, value) VALUES (?, TOTIMESTAMP(NOW()), ?);")
  val selectMes = session.prepare("SELECT stamp as time, value FROM measurements WHERE sensor_id = ? AND stamp >= ? AND stamp < ? ORDER BY stamp")
  val selectAgg = session.prepare("SELECT min(value) AS min, avg(value) AS avg, max(value) AS max FROM measurements WHERE sensor_id = ? AND stamp >= ? AND stamp < ?")
  val countMes = session.prepare("SELECT count(value) AS count FROM measurements WHERE sensor_id = ? AND stamp >= ? AND stamp < ?")

  def parseDateRange(from: String, to: String, fmt: String): Either[(Date, Date), Response[String]] = {
    val sdf = new SimpleDateFormat(fmt)
    try {
      return Left(sdf.parse(from) -> sdf.parse(to))
    } catch {
      case _: ParseException => return Right(Response(f"Provided date does not match format '${fmt}'\n", 400))
    }
  }

  def countMeasurements(sensor: Int, from: Date, to: Date) = {
      val stat = new BoundStatement(countMes).bind(sensor, from, to)
      session.execute(stat).one().getLong("count")
  }

  @cask.get("/sensor/:id/agg")
  def getAggregation(id: Int, from: String, to: String, dateformat: Option[String] = None): Response[String] = {
    val (fromDate, toDate) = parseDateRange(from, to, dateformat.getOrElse("yyyy-MM-dd")) match {
      case Left(dates) => dates; case Right(errorRes) => return errorRes
    }
    try {
      val stat = new BoundStatement(selectAgg).bind(id, fromDate, toDate)
      val res = session.execute(stat).one()
      val (min, avg, max) = (res.getDouble("min"), res.getDouble("avg"), res.getDouble("max"))
      Response(f"""{"min": ${min}, "avg": ${avg}, "max": ${max}}""", 200)
    } catch {
      case e: Throwable =>
        System.err.println(e)
        Response("Database error", 500)
    }
  }

  @cask.get("/sensor/:id")
  def getMeasurements(id: Int, from: String, to: String, dateformat: Option[String] = None): Response[String] = {
    val (fromDate, toDate) = parseDateRange(from, to, dateformat.getOrElse("yyyy-MM-dd")) match {
      case Left(dates) => dates; case Right(errorRes) => return errorRes
    }
    val days = ChronoUnit.DAYS.between(fromDate.toInstant(), toDate.toInstant()).toInt
    if (days > maxDateWindowSoft && countMeasurements(id, fromDate, toDate) > maxRowsInResponse) {
      return Response("Time window too wide. ")
    }
    try {
      val stat = new BoundStatement(selectMes).bind(id, fromDate, toDate)
      val rows = session.execute(stat).iterator().asScala
      val res = rows.map(x => x.getTimestamp("time").getTime() + "," + x.getDouble("value")).mkString("\n")
      Response(res, 200)
    } catch {
      case e: Throwable =>
        System.err.println(e)
        Response("Database error", 500)
    }
  }

  @cask.post("/sensor/:id")
  def postMeasurement(req: Request, id: Int): Response[String] = {
    val value = req.text().toDoubleOption match {
      case Some(x) => x; case None => return Response("Bad value format\n", 400)
    }
    session.execute(new BoundStatement(insertMes).bind(id, value))
    return Response("", 200)
  }

  initialize()
}

