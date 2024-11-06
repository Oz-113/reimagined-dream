const { SerialPort } = require('serialport');
const WebSocket = require('ws');

const port = new SerialPort({ path: '/dev/ttyS0', baudRate: 115200 });
const socket = new WebSocket.Server({ port: 8080 }, () => {
    console.log('WebSocket sunucusu 8080 portunda başlatıldı');
});

let sayac = 0;
let espid;
let intler = [0, 0, 0]; // [mesafe, potansiyometre, boolean]
let dafloat = 0.0; // sıcaklık
let mesag = '';
let currentWs = null; // Bağlı WebSocket nesnesini saklayacağız

setInterval(() => {
    port.write('.');  // Seri porta '.' gönder
}, 1500);

socket.on('connection', (ws) => {
    console.log('WebSocket bağlantısı açıldı');
    currentWs = ws; // Bağlı WebSocket nesnesini güncelle

    ws.on('close', () => {
        console.log('WebSocket bağlantısı kapatıldı');
        currentWs = null; // Bağlantı kapatıldığında null yap
    });
});

port.on('data', (data) => {
    // Seri porttan veri okuma
    let message = data.toString();
    for (let char of message) {
        processIncomingChar(char);
    }
});

function processIncomingChar(dat) {
    switch (dat) {
        case '?':
            espid = parseInt(mesag);
            console.log(`ESP ID: ${espid}`);
            mesag = '';
            break;
        case '#':
            console.log("-----------------");
            if (currentWs) { // Eğer bağlı bir WebSocket varsa
                // ESP ID'ye göre sıcaklık ve mesafe değerlerini gönder
                let messageToSend;
                if (espid === 1) {
                    // ESP ID 1 için
                    messageToSend = `ESP1;${intler[0]};${dafloat}`; // intler[0] mesafe
                } else if (espid === 2) {
                    // ESP ID 2 için
                    messageToSend = `ESP2;${intler[0]};${dafloat}`; // intler[0] mesafe
                }
                currentWs.send(messageToSend); // Veriyi gönder
            }
            break;
        case '&':
            console.log(`mesaj: ${mesag}`);
            mesag = '';
            break;
        case '$':
            intler[sayac] = parseInt(mesag);
            if (sayac === 0) {
                console.log(`mesafe: ${intler[sayac]}`);
            } else if (sayac === 1) {
                console.log(`potansiyometre: ${intler[sayac]}`);
            } else if (sayac === 2) {
                console.log(`boolean: ${intler[sayac]}`);
            }
            sayac = (sayac + 1) % 3; // Değerleri döngüsel olarak al
            mesag = '';
            break;
        case '!':
            dafloat = parseFloat(mesag);
            console.log(`sıcaklık: ${dafloat}`);
            mesag = '';
            break;
        default:
            mesag += dat; // Gelen karakteri mesaj değişkenine ekle
            break;
    }
}
