const { SerialPort } = require('serialport');
const WebSocket = require('ws');
const Database = require('better-sqlite3');

// SQLite veritabanını oluştur ve tabloyu hazırla
const db = new Database('ev_data_log.db');
db.exec(`
  CREATE TABLE IF NOT EXISTS sensor_data (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    espid INTEGER,
    message TEXT,
    int1 INTEGER,
    int2 INTEGER,
    int3 INTEGER,
    temperature REAL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
  )
`);

const port = new SerialPort({ path: '/dev/ttyS0', baudRate: 115200 });
const socket = new WebSocket.Server({ port: 8080 });

let espData = {
  esp1: { message: '', int1: 0, int2: 0, int3: 0, temperature: 0.0 },
  esp2: { message: '', int1: 0, int2: 0, int3: 0, temperature: 0.0 },
  esp3: { message: '', int1: 0, int2: 0, int3: 0, temperature: 0.0 },
  esp4: { message: '', int1: 0, int2: 0, int3: 0, temperature: 0.0 },
  esp5: { message: '', int1: 0, int2: 0, int3: 0, temperature: 0.0 }
};
let currentEspId = null;
let mesag = '';
let currentWs = null;
let sayac=0;

function insertDataToDB(data, espid) {
  const insert = db.prepare(`
    INSERT INTO sensor_data (espid, message, int1, int2, int3, temperature) 
    VALUES (?, ?, ?, ?, ?, ?)
  `);
  insert.run(espid, data.message, data.int1, data.int2, data.int3, data.temperature);
}

// 1 dakikada bir verileri veritabanına kaydet
setInterval(() => {
  for (let i = 1; i <= 5; i++) {
    const espKey = `esp${i}`;
    insertDataToDB(espData[espKey], i);
  }
}, 60000);

setInterval(() => {
    port.write('.');  // Seri porta '.' gönder
}, 1000);

socket.on('connection', (ws) => {
  currentWs = ws;
  ws.on('message', (message) => {
    for (let char of message) {
      processIncomingChar(char);
    }
  });
  ws.on('close', () => {
    currentWs = null;
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
      espData[`esp${currentEspId}`].message = mesag;
      mesag = '';
      break;
    case '$':
      if (sayac === 0) {
        espData[`esp${currentEspId}`].int1 = parseInt(mesag);
      } else if (sayac === 1) {
        espData[`esp${currentEspId}`].int2 = parseInt(mesag);
      } else if(sayac === 2){
        espData[`esp${currentEspId}`].int3 = parseInt(mesag);
      }
      sayac = (sayac + 1) % 3;
      mesag = '';
      break;
    case '!':
      espData[`esp${currentEspId}`].temperature = parseFloat(mesag);
      mesag = '';
      break;
    case '#':
      console.log(`ESP${currentEspId} Verisi:`, espData[`esp${currentEspId}`]);
      if (currentWs) { // Eğer bağlı bir WebSocket varsa
        // ESP ID'ye göre sıcaklık ve mesafe değerlerini gönder
        let messageToSend;
        if (currentEspId === 1) {
            // ESP ID 1 için
            messageToSend = `ESP1;${espData[`esp${currentEspId}`].int2};${espData[`esp${currentEspId}`].temperature}`; // intler[0] mesafe
        } else if (currentEspId === 2) {
            // ESP ID 2 için
            messageToSend = `ESP2;${espData[`esp${currentEspId}`].int2};${espData[`esp${currentEspId}`].temperature}`; // intler[0] mesafe
        } else if (currentEspId === 3) {
            // ESP ID 3 için
            messageToSend = `ESP3;${espData[`esp${currentEspId}`].int2};${espData[`esp${currentEspId}`].temperature}`; // intler[0] mesafe
        } else if (currentEspId === 4) {
            // ESP ID 4 için
            messageToSend = `ESP4;${espData[`esp${currentEspId}`].int2};${espData[`esp${currentEspId}`].temperature}`; // intler[0] mesafe
        } else if (currentEspId === 5) {
            // ESP ID 2 için
            messageToSend = `ESP5;${espData[`esp${currentEspId}`].int2};${espData[`esp${currentEspId}`].temperature}`; // intler[0] mesafe
        }
        currentWs.send(messageToSend); // Veriyi gönder
    }
      mesag = '';
      break;
    default:
      mesag += dat;
  }
}
