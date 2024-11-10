const Database = require('better-sqlite3');

// Veritabanını aç
const db = new Database('ev_data_log.db', { verbose: console.log });

// Tüm verileri çek
const stmt = db.prepare('SELECT * FROM sensor_data');
const allData = stmt.all(); // Tüm satırları getirir

console.log('Tüm Veriler:', allData);
