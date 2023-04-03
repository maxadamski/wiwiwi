/* eslint no-undef: 0 */

let dt = 0

const win = nw.Window.get()
const dgram = require('dgram')
const Net = require('net')

let socket = {}
let host = '10.0.0.10'
let port = 6666
let nick = ''

let nickInput, addrInput, connectButton
let myFont

const angleSpeed = 0.003
const moveSpeed = 0.15
const bulletSpeed = 0.3
const bulletDecay = 1000 * 1
const maxLogs = 20
const mapPad = 100
const mapZoomInit = 2
const mapZoom = mapZoomInit
const mapFog = 800
const pelletAngleSpeed = 0.002

let stars = []
let logs = []

let pelletAngle = 0

// center of viewport
let cx = 0, cy = 0
// maximum viewport translation
let maxTx = 0, maxTy = 0

// local game state

// global game state
let connected = false
let joined = false
let gameOver = false
let winningId = -1
let lastPing = -1
let myId = -1
let myShip = {}
let mapSize = 5000
let starCount = mapSize / 5
let untilReset = 0
let untilStop = 0

let pellets = {}
let bullets = {}
let rocks = {}
let ships = {}

// ----------------------------------------------------------------------------
// -- Networking
// ----------------------------------------------------------------------------

function resetGame() {
  pellets = {}
  bullets = {}
  rocks = {}
  ships = {}
  myShip = {}
  myId = -1
  gameOver = false
  joined = false
  winningId = -1
}

let messageBuffer = ''

function sendMessage(message) {
  socket.write(message+'\n')
}

function recvMessage() {
  let received = messageBuffer.split('\n');
  if (received.length > 1) {
    for (let i = 0; i < received.length - 1; i++) {
      received[i].split(';').forEach(onMessage)
    }
    messageBuffer = received[received.length - 1]
  }
}

function onMessage(text) {
  if (text == '') return

  const msg = text.split(',')
  const oldMapSize = mapSize

  lastPing = millis()
  if (msg[0] == 'pong') {

  } else if (msg[0] == 'got-hit') {
    screenShake.toggle()

  } else if (msg[0] == 'join') {
    const id = num(msg[1])
    myShip = new MyShip(id)
    myId = id

    myShip.x = dint(msg[2])
    myShip.y = dint(msg[3])
    myShip.angle = dint(msg[4])
    myShip.spice = num(msg[5])
    myShip.energy = num(msg[6])
    myShip.shield = num(msg[7]) === 1
    if (num(msg[8]) === 1) gameOver = true

    mapSize = num(msg[9])
    untilReset = num(msg[10])
    untilStop = num(msg[11])

    joined = true

  } else if (msg[0] == 'stat-game') {
    mapSize = num(msg[1])
    untilReset = Math.max(num(msg[2]), 0)
    untilStop = Math.max(num(msg[3]), 0)
    gameOver = num(msg[4]) === 1

  } else if (msg[0] == 'stat-ship') {
    const id = num(msg[1])
    if (id == myId) {
      const ship = myShip
      ship.spice = num(msg[5])
      ship.energy = num(msg[6])
      ship.shield = num(msg[7]) === 1
      if (num(msg[8]) === 1) gameOver = true

    } else {
      if (!(id in ships)) ships[id] = new Ship(id)
      const ship = ships[id]

      ship.x = dint(msg[2])
      ship.y = dint(msg[3])
      ship.angle = dint(msg[4])
      ship.spice = num(msg[5])
      ship.energy = num(msg[6])
      ship.shield = num(msg[7]) === 1
    }

  } else if (msg[0] == 'stat-rock') {
    const id = num(msg[1])
    if (!(id in rocks)) rocks[id] = new Rock(id)
    const rock = rocks[id]

    rock.x = dint(msg[2])
    rock.y = dint(msg[3])
    rock.angle = dint(msg[4])
    rock.speed = dint(msg[5])
    rock.size = num(msg[6])
    rock.health = num(msg[7])

    if (!('angleSpeed' in rock)) rock.angleSpeed = randomGaussian(0, 0.0001)
    if (!('vertices' in rock)) rock.vertices = randomPolygon(rock.size)

  } else if (msg[0] == 'stat-bullet') {
    const id = num(msg[1])
    const pid = num(msg[2])
    if (!(id in bullets)) bullets[id] = new Bullet(id, pid)
    const bullet = bullets[id]

    bullet.x = dint(msg[3])
    bullet.y = dint(msg[4])
    bullet.angle = dint(msg[5])
    bullet.time = num(msg[6])

  } else if (msg[0] == 'stat-pellet') {
    console.log(text)
    const id = num(msg[1])
    if (!(id in pellets)) pellets[id] = new Pellet(id)
    const pellet = pellets[id]

    pellet.x = dint(msg[2])
    pellet.y = dint(msg[3])
    pellet.value = num(msg[4])
    pellet.type = num(msg[5])

  } else if (msg[0] == 'log-join') {
    addLog(`${msg[1]} joined the arena`)

  } else if (msg[0] == 'log-left') {
    addLog(`${msg[1]} left the arena`)

  } else if (msg[0] == 'log-dead') {
    addLog(`${msg[1]} was destroyed`)

  } else if (msg[0] == 'log-win') {
    addLog(`${msg[1]} won the game`)

  } else if (msg[0] == 'del-bullet') {
    const id = num(msg[1])
    if (id in bullets) delete bullets[id]

  } else if (msg[0] == 'del-pellet') {
    console.log(text)
    const id = num(msg[1])
    if (id in pellets) delete pellets[id]

  } else if (msg[0] == 'del-rock') {
    const id = num(msg[1])
    if (id in rocks) delete rocks[id]

  } else if (msg[0] == 'del-ship') {
    const id = num(msg[1])
    if (id == myId) gameOver = true
    if (id in ships) ships[id].visible = false

  } else if (msg[0] == 'game-over') {
    const id = num(msg[1])
    if (id == myId) {
      gameOver = true
      winningId = id
    }

  } else {
    console.log(`unknown message ${text}`)
  }

  if (mapSize != oldMapSize) windowResized()
}

function shootBullet () {
  sendMessage(`usr-fired,${myId},${eint(myShip.x)},${eint(myShip.y)},${eint(myShip.angle)}`)
}

let syncPlayer = timedLambda(20, () => {
  if (!joined) return
  sendMessage(`usr-coord,${myId},${eint(myShip.x)},${eint(myShip.y)},${eint(myShip.angle)}`)
})

let syncConnection = timedLambda(800, () => {
  sendMessage('ping')
  if (millis() - lastPing > 5*1000) {
    connected = false
    joined = false
  }
})

// ----------------------------------------------------------------------------
// -- Game objects
// ----------------------------------------------------------------------------

const screenShake = {
  decay: 0.5 * 1000,
  enabled: false,
  time: 0,
  speed: 0.2,

  toggle() {
    this.enabled = !this.enabled
    this.time = 0
  },

  update() {
    if (!this.enabled) return
    this.time += deltaTime
    if (this.time > this.decay) this.toggle()
  },

  draw() {
    if (!this.enabled) return
    let x = this.speed * this.time
    translate(sin(x), sin(x))
  },
}

class Pellet {
  constructor(id) {
    this.id = id
  }

  update() {
  }

  draw() {
    if (outsideView(this)) return
    push()
    translate(this.x, this.y)
    rotate(pelletAngle)
    if (this.type == 1) {
      stroke(60, 100, 100)
      rect(-2, -2, 4, 4)
    } else {
      rect(-2, -2, 4, 4)
    }
    pop()
  }
}

class Bullet {
  constructor(id, pid) {
    this.id = id
    this.pid = pid
  }

  update() {
    this.x += dt * bulletSpeed * Math.cos(this.angle)
    this.y += dt * bulletSpeed * Math.sin(this.angle)
    this.time += dt
  }

  draw() {
    if (outsideView(this)) return
    push()
    fill(0)
    stroke(100 + 20 * log(1 - Math.max(this.time, 0) / bulletDecay))
    circle(this.x, this.y, 1)
    pop()
  }
}

class Rock {
  constructor(id) {
    this.id = id
  }

  update() {
    //this.angle += dt * this.angleSpeed
    this.x += dt * this.speed * Math.cos(this.angle)
    this.y += dt * this.speed * Math.sin(this.angle)
  }

  draw() {
    if (outsideView(this)) return
    push()
    fill(0)
    stroke(0, 100 - 100 * this.health / this.maxHealth, 100)
    translate(this.x, this.y)
    //rotate(this.angle)
    beginShape()
    for (const v of this.vertices) vertex(v[0], v[1])
    endShape(CLOSE)
    pop()
  }
}

class Ship {
  constructor(id) {
    this.visible = true
    this.id = id
  }

  update() {
  }

  draw() {
    if (!this.visible) return
    if (outsideView(this)) return
    push()
    translate(this.x, this.y)
    if (this.shield) {
      push()
      rotate(pelletAngle)
      polygon(0, 0, 30, 6)
      pop()
    }
    fill(0)
    rotate(this.angle)
    triangle(-6, -5, -6, 5, 6, 0)
    pop()
  }
}

class MyShip {
  constructor(id) {
    this.id = id
  }

  update() {
    if (keyIsDown(LEFT_ARROW)) {
      this.angle -= dt * angleSpeed
      this.angle %= TAU
    }
    if (keyIsDown(RIGHT_ARROW)) {
      this.angle += dt * angleSpeed
      this.angle %= TAU
    }
    if (keyIsDown(UP_ARROW)) {
      this.x += dt * moveSpeed * Math.cos(this.angle)
      this.y += dt * moveSpeed * Math.sin(this.angle)
    }
    if (keyIsDown(DOWN_ARROW)) {
      this.x -= dt * moveSpeed * Math.cos(this.angle)
      this.y -= dt * moveSpeed * Math.sin(this.angle)
    }
    this.x = clip(this.x, 0, mapSize)
    this.y = clip(this.y, 0, mapSize)
  }

  draw() {
    push()
    translate(this.x, this.y)

    if (this.shield) {
      push()
      rotate(pelletAngle)
      polygon(0, 0, 30, 6)
      pop()
    }

    stroke(255)
    if (screenShake.enabled) {
      stroke(0, 100 * (1 - screenShake.time / screenShake.decay), 100)
    }

    if (!screenShake.enabled) {
      stroke(0, 100 * (1 - this.energy / this.maxEnergy), 100)
    }

    fill(0)
    rotate(this.angle)
    triangle(-6, -5, -6, 5, 6, 0)
    pop()
  }

}



// ----------------------------------------------------------------------------
// -- Views
// ----------------------------------------------------------------------------

let currentView = {}

function segueTo(view) {
  if (!!currentView.onExit) currentView.onExit()
  currentView = view
  if (!!currentView.onShow) currentView.onShow()
}

const loginView = {
  message: '',
  connecting: false,
  connTimeout: 3*1000,
  connTime: 0,

  onShow() {
    connectButton.mousePressed(() => this.onConnect());
    connectButton.show()
    addrInput.show()
    nickInput.show()
    logs = []
  },

  onExit() {
    this.connecting = false;
    this.connTime = 0;
    this.message = ''
    connectButton.hide()
    addrInput.hide()
    nickInput.hide()
    logs = []
    lobbyView.banner = ''
  },

  keyPressed() {
    if (keyCode == 13 /* enter */) this.onConnect()
  },

  onConnect() {
    this.connecting = true
    this.connTime = 0
    this.message = 'connecting...'

    addr = addrInput.value().split(':')
    host = addr[0]
    port = Number(addr[1])
    nick = nickInput.value()

    if (!!socket.close) socket.close()

    socket = new Net.Socket()

    socket.connect({host: host, port: port}, () => {
      sendMessage("conn")
      connected = true
    })

    socket.on('data', data => {
      messageBuffer += data
      recvMessage()
    })

    socket.on('end', () => {
      connected = false
    })
  },

  onTimeout() {
    this.connecting = false
    this.connTime = 0
    this.message = 'connection timed out'
  },

  update() {
    if (this.connecting) {
      syncConnection()
      if (connected) segueTo(lobbyView)
      this.connTime += deltaTime
      if (this.connTime > this.connTimeout) this.onTimeout()
    }
  },

  draw() {
    drawStars()
  },

  drawUI() {
    drawText(this.message, windowWidth / 2, 80, 'monospace', 18, CENTER)
    drawText('Max Adamski', 20, windowHeight - 20, 'monospace', 14, LEFT)
  }
}


const lobbyView = {
  banner: '',

  onShow() {
  },

  update() {
    syncConnection()
    if (joined) {
      generateStars()
      segueTo(gameView)
    }
    if (!connected) {
      loginView.message = 'disconnected from server'
      segueTo(loginView)
    }
  },

  draw() {
    updateViewport()
    drawStars()
  },

  keyPressed() {
    if (keyCode == 13 /* enter */) {
      sendMessage(`conn`)
      sendMessage(`join,${nick}`)
    }
  },

  drawUI() {
    drawTimer()
    drawText(this.banner, windowWidth/2, windowHeight/2, myFont, 80, CENTER)
    drawText('lobby', 20, 35, myFont, 24, LEFT)
    drawText(this.statusMessage(), 20, 60, 'monospace', 18, LEFT)

    drawText('captain\'s log', windowWidth - 20, 35, myFont, 24, RIGHT)
    //drawText(this.leaderboardText(), windowWidth - 20, 57, 'monospace', 18, RIGHT)
    drawText(logs.join('\n'), windowWidth -20, 57, 'monospace', 18, RIGHT)
  },

  statusMessage() {
    if (untilStop > 0) return 'game is in progress\npress ENTER to join the arena'
    if (untilReset > 0) return `preparing the next game\nplease wait ${timeStr(untilReset)}`
    return 'unknown game status'
  },

  leaderboardText() {
    let text = ''
    for (let i = 1; i <= 9; i++) {
      let spice = 1000
      spice = spice.toString().padStart(6, ' ')
      text += `${i}. someone somewhere ${spice} SPICE\n`
    }
    return text
  },
}

const gameView = {
  onExit() {
    resetGame()
  },

  update() {
    syncConnection()
    syncPlayer()

    updateViewport()
    screenShake.update()

    pelletAngle += dt * pelletAngleSpeed
    myShip.update()
    for (const id in pellets) pellets[id].update()
    for (const id in bullets) bullets[id].update()
    for (const id in rocks) rocks[id].update()
    for (const id in ships) ships[id].update()

    if (!connected) {
      loginView.message = 'disconnected from server'
      segueTo(loginView)
    }

    if (gameOver) {
      joined = false
      if (winningId == myId) {
        lobbyView.banner = 'you won!'
      } else {
        lobbyView.banner = 'game over'
      }
      console.log('game over, segue to lobby')
      segueTo(lobbyView)
    }
  },

  keyPressed() {
    if (keyCode == 32 /* space */) shootBullet()
  },

  draw() {
    rect(0, 0, mapSize, mapSize)
    drawStars()
    for (const id in bullets) bullets[id].draw()
    for (const id in rocks) rocks[id].draw()
    for (const id in pellets) pellets[id].draw()
    for (const id in ships) ships[id].draw()
    myShip.draw()
  },

  drawUI() {
    drawTimer()
    textFont(myFont, 24);
    textAlign(LEFT)
    text('starship status', 20, 35)
    textAlign(RIGHT)
    text('captain\'s log', windowWidth - 20, 35)

    textFont('monospace', 18)
    textAlign(LEFT)
    text(this.myStatus(), 20, 60)
    textAlign(RIGHT)
    text(logs.join('\n'), windowWidth -20, 57)
  },

  myStatus() {
    return `SPICE  ${myShip.spice}
ENERGY ${myShip.energy}
SHIELD ${myShip.shield ? 'ON' : 'OFF'}
COORDS ${round(myShip.x)} X ${round(myShip.y)}`
  },
}


// ----------------------------------------------------------------------------
// -- P5 callbacks
// ----------------------------------------------------------------------------

function preload() {
  myFont = loadFont('brfont.ttf')
}

function windowResized() {
  console.log('window resized')
  resizeCanvas(windowWidth, windowHeight)

  nickInput.position(windowWidth/2 - 170, windowHeight/2 - 50)
  addrInput.position(windowWidth/2 - 170, windowHeight/2 - 0)
  connectButton.position(windowWidth/2 - 170, windowHeight/2 + 50)

  cx = windowWidth / 2 / mapZoom
  cy = windowHeight / 2 / mapZoom
  maxTx = mapSize + mapPad - 2 * cx
  maxTy = mapSize + mapPad - 2 * cy

  // let p = max(windowWidth, windowHeight);
  // let q = windowWidth > windowHeight ? 1920 : 1080;
  // mapZoom = mapZoomInit * p / q
}

function setup() {
  const canvas = createCanvas(windowWidth, windowHeight)
  canvas.parent('game')
  frameRate(50)

  nick = randomNick();
  nickInput = createInput(nick);
  addrInput = createInput(host+':'+port);
  connectButton = createButton('CONNECT');

  windowResized()
  generateStars()
  segueTo(loginView)
}

function draw() {
  dt = deltaTime
  currentView.update()

  colorMode(HSB, 100)
  strokeCap(SQUARE)
  strokeWeight(2)
  background(0)

  noFill()
  stroke(255)

  currentView.draw()

  resetMatrix()
  fill(255)
  noStroke()

  currentView.drawUI()

  drawText('galActica', windowWidth / 2, 50, myFont, 40, CENTER)
  drawText(`FPS: ${floor(frameRate())}`, windowWidth - 20, windowHeight - 20, 'monospace', 24, RIGHT)
}

function keyPressed () {
  if (keyCode == 82 /* R */) win.reload()
  if (!!currentView.keyPressed) currentView.keyPressed()
}



// ----------------------------------------------------------------------------
// -- Misc functions
// ----------------------------------------------------------------------------


function generateStars() {
  starCount = mapSize / 5
  stars = []
  for (let i = 0; i < starCount; i++) stars.push(randomStar())
}

function drawStars() {
  push()
  strokeWeight(1)
  for (const star of stars) {
    if (outsideView(star)) continue
    stroke(star.light / 100 * 255)
    rect(star.x, star.y, 1, 1)
  }
  pop()
}

function drawTimer() {
    textFont(myFont, 36);
    textAlign(CENTER)
    text(timeStr(untilStop), windowWidth / 2, 90)
}

function updateViewport() {
  const tx = -clip(myShip.x - cx, -mapPad, maxTx)
  const ty = -clip(myShip.y - cy, -mapPad, maxTy)
  scale(mapZoom)
  translate(tx, ty)
}


function randomNick () {
  const firstName = 'spicy,evil,chaotic,lawful,crazy,drunk,speedy,nice,kind,enlightened'.split(',')
  const lastName = 'human,martian,captain,commander,emperor,racer,slime,goblin,space wizard,black hole'.split(',')
  return firstName[Math.floor(Math.random()*10)]+' '+lastName[Math.floor(Math.random()*10)]
}

function outsideView (obj) {
  return distsq(myShip, obj) > mapFog * mapFog
}

function randomStar() {
  return {
    x: random(-mapPad, mapSize + mapPad),
    y: random(-mapPad, mapSize + mapPad),
    light: random(50, 100)
  }
}

function addLog(log) {
  logs.push(log)
  if (logs.length > maxLogs) logs = logs.slice(-maxLogs + 1)
}
