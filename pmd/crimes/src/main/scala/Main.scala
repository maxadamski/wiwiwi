import org.apache.spark.sql.{SparkSession, Dataset}
import org.apache.spark.sql.streaming._
import org.apache.spark.sql.functions._
import scala.io.Source
import collection.immutable.Map

private case class Event(
  date: String,
  category: String,
  district: String,
  arrest: Int,
  domestic: Int,
  fbi: Int,
)

object Main {
  def toInt(x: Boolean) = if (x) 1 else 0

  def main(args: Array[String]) {
    if (args.length != 7) {
      println("usage: java -jar big-data.jar ANOM_WINDOW ANOM_THRESH STAT_TRIGGER ANOM_TRIGGER TOPIC SERVER IUCR_PATH")
      return
    }
    val Array(anomWindow, anomThreshStr, statTrigger, anomTrigger, topic, server, iucr) = args

    val iucrData = Source.fromFile(iucr).getLines.map(_.split(",").toSeq).toSeq
    val iucrToCategory = iucrData.map(x => x(0) -> x(1)).toMap
    val iucrToFbi = iucrData.map(x => x(0) -> toInt(x(3) == "I")).toMap

    val anomThresh = anomThreshStr.toDouble / 100

    val spark = SparkSession.builder.appName("Big Data").getOrCreate()
    spark.sparkContext.setLogLevel("WARN")
    import spark.implicits._
    val ds = spark.readStream.format("kafka")
      .option("kafka.bootstrap.servers", server)
      .option("subscribe", topic)
      .load()

    val rows = ds
      .selectExpr("CAST(key as STRING)", "CAST(value as STRING)")
      .as[(String, String)]

    val crimes = rows
      .map(_._2.split(","))
      .map(x => Event(
        date=x(1),
        category=iucrToCategory.getOrElse(x(2), "UNKNOWN"),
        district=x(5),
        arrest=toInt(x(3) == "True"),
        domestic=toInt(x(4) == "True"),
        fbi=iucrToFbi.getOrElse(x(2), 0)
      ))
      .withColumn("ts", to_timestamp($"date", "yyyy-MM-dd'T'HH:mm:ss.SSSX"))

    val stat = crimes
      .withColumn("month", to_timestamp(date_format($"ts", "yyyy-MM-'01'"), "yyyy-MM-dd"))
      .withWatermark("month", "1 month")
      .groupBy($"month", $"district", $"category")
      .agg(
          count("district").as("total"),
          sum("arrest").as("total_arrest"),
          sum("domestic").as("total_domestic"),
          sum("fbi").as("total_fbi")
      )
      .withColumn("month", date_format($"month", "yyyy-MM"))
      .select("month", "district", "category", "total", "total_arrest", "total_domestic", "total_fbi")

    val anom = crimes
      .withWatermark("ts", "1 day")
      .groupBy(window($"ts", s"${anomWindow} days", "1 day"), $"district")
      .agg(
          count("district").as("total"),
          sum("fbi").as("total_fbi"),
      )
      .withColumn("fbi_pr", when($"total_fbi" > 0, $"total_fbi" / $"total").otherwise(0))
      .withColumn("anomaly", $"fbi_pr" > anomThresh)
      .withColumn("start", $"window"("start"))
      .withColumn("end", $"window"("end"))
      .select("start", "end", "district", "total", "total_fbi", "fbi_pr", "anomaly")

    val statStream = stat.writeStream.format("csv")
      .trigger(Trigger.ProcessingTime(statTrigger))
      .option("checkpointLocation", "checkpoint_stat/")
      .outputMode("append")
      .start("stat/")

    val anomStream = anom.writeStream.format("csv")
      .trigger(Trigger.ProcessingTime(anomTrigger))
      .option("checkpointLocation", "checkpoint_anom/")
      .outputMode("append")
      .start("anom/")

    statStream.awaitTermination()
    anomStream.awaitTermination()
  }
}
