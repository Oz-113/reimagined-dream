const { SerialPort } = require('serialport');
const WebSocket = require('ws');
const Database = require('better-sqlite3');

// SQLite veritabanını oluştur ve tabloyu hazırla
const db = new Database('ev_data_log.db');
db.exec(`
  CREATE TABLE IF NOT EXISTS sensor_data (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    espid INTEGER,
    kapi INTEGER,
    pir INTEGER,
    parlaklik INTEGER,
    temperature REAL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
  )
`);

const port = new SerialPort({ path: '/dev/ttyS0', baudRate: 115200 });
const socket = new WebSocket.Server({ port: 8080 });

let espData = {
  esp1: { kapi: 0, pir: 0, parlaklik: 0, temperature: 0.0 },
  esp2: { kapi: 0, pir: 0, parlaklik: 0, temperature: 0.0 },
  esp3: { kapi: 0, pir: 0, parlaklik: 0, temperature: 0.0 },
  esp4: { kapi: 0, pir: 0, parlaklik: 0, temperature: 0.0 },
  esp5: { kapi: 0, pir: 0, parlaklik: 0, temperature: 0.0 }
};
let currentEspId = null;
let mesag = '';
let currentWs = null;
let sayac=0;
let clientsCount = 0;
function insertDataToDB(data, espid) {
  const insert = db.prepare(`
    INSERT INTO sensor_data (espid, kapi, pir, parlaklik, temperature) 
    VALUES (?, ?, ?, ?, ?)
  `);
  insert.run(espid, data.kapi, data.pir, data.parlaklik, data.temperature);
}

port.write('.');
// 1 dakikada bir verileri veritabanına kaydet
setInterval(() => {
  if(clientsCount===0){
  port.write('.');
}
  for (let i = 1; i <= 5; i++) {
    const espKey = `esp${i}`;
    insertDataToDB(espData[espKey], i);
  }
}, 60000);

setInterval(() => {
  if (clientsCount > 0) { 
    port.write('.');
  }
}, 1000);

socket.on('connection', (ws) => {
  currentWs = ws;
  clientsCount++;
  ws.on('message', (message) => {
    for (let char of message) {
      processIncomingChar(char);
    }
  });
  ws.on('close', () => {
    currentWs = null;
    clientsCount--;
  });
});

port.on('data', (data) => {
  let message = data.toString();
  for (let char of message) {
    processIncomingChar(char);
  }
});

function processIncomingChar(dat) {
  switch (dat) {
    case '?':
      currentEspId = parseInt(mesag);
      mesag = '';
      break;
    case '&':
      // espData[`esp${currentEspId}`].message = mesag;
      mesag = '';
      break;
    case '$':
      if (sayac === 0) {
        espData[`esp${currentEspId}`].kapi = parseInt(mesag);
      } else if (sayac === 1) {
        espData[`esp${currentEspId}`].pir = parseInt(mesag);
      } else if(sayac === 2){
        espData[`esp${currentEspId}`].parlaklik = parseInt(mesag);
      }
      sayac = (sayac + 1) % 3;
      mesag = '';
      break;
    case '!':
      espData[`esp${currentEspId}`].temperature = parseFloat(mesag);
      mesag = '';
      break;
    case '#':
      console.log(`ESP${currentEspId} Data:`, espData[`esp${currentEspId}`]);
      if (currentWs) {
	      let messageToSend;
        if (currentEspId === 1) {
                       messageToSend = `ESP1;${espData[`esp${currentEspId}`].pir};${espData[`esp${currentEspId}`].kapi};${espData[`esp${currentEspId}`].parlaklik};${espData[`esp${currentEspId}`].temperature}`; 
        } else if (currentEspId === 2) {
                       messageToSend = `ESP2;${espData[`esp${currentEspId}`].pir};${espData[`esp${currentEspId}`].kapi};${espData[`esp${currentEspId}`].parlaklik};${espData[`esp${currentEspId}`].temperature}`; 
        } else if (currentEspId === 3) {
                       messageToSend = `ESP3;${espData[`esp${currentEspId}`].pir};${espData[`esp${currentEspId}`].kapi};${espData[`esp${currentEspId}`].parlaklik};${espData[`esp${currentEspId}`].temperature}`;    
	} else if (currentEspId === 4) {
                        messageToSend = `ESP4;${espData[`esp${currentEspId}`].pir};${espData[`esp${currentEspId}`].kapi};${espData[`esp${currentEspId}`].parlaklik};${espData[`esp${currentEspId}`].temperature}`
        } else if (currentEspId === 5) {
                       messageToSend = `ESP5;${espData[`esp${currentEspId}`].pir};${espData[`esp${currentEspId}`].kapi};${espData[`esp${currentEspId}`].parlaklik};${espData[`esp${currentEspId}`].temperature}`;        }

        currentWs.send(messageToSend);    }
      mesag = '';
      break;
    default:
      mesag += dat;
  }
}
