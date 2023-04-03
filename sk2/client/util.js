
function eint (x) {
  return floor(x * 1000)
}

function dint (x) {
  return Number(x) / 1000
}

function num (x) {
  return Number(x)
}

function distsq(a, b) {
  const dx = a.x - b.x
  const dy = a.y - b.y
  return dx * dx + dy * dy
}

function clip(x, lower, upper) {
  return Math.max(Math.min(x, upper), lower)
}

function polygon(x, y, radius, sides) {
  const angle = TAU / sides
  beginShape()
  for (let a = 0; a < TAU; a += angle) {
    const sx = x + Math.cos(a) * radius
    const sy = y + Math.sin(a) * radius
    vertex(sx, sy)
  }
  endShape(CLOSE)
}

function randomPolygon(size) {
  const v = []
  const steps = randomGaussian(10, 2)
  for (let a = 0; a < TAU; a += randomGaussian(TAU / steps, TAU / 36)) {
    d = randomGaussian(size / 2, 7)
    v.push([d * cos(a), d * sin(a)])
  }
  return v
}

function drawText(value, x, y, font, size, align) {
  textFont(font)
  textSize(size)
  textAlign(align)
  text(value, x, y)
}

function timeStr (millis) {
  const min = floor((millis / 1000) / 60).toString().padStart(2, '0')
  const sec = floor((millis / 1000) % 60).toString().padStart(2, '0')
  return min+':'+sec
}

function timedLambda(period, lambda) {
  let lastCall = 0
  function fun() {
    if (millis() - lastCall < period) return
    lastCall = millis()
    lambda()
  }
  return fun;
}

