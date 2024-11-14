

#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 15
#define DHTTYPE DHT11
#define IRPIN 26
#define LDRPIN 27
#define PIRPIN 14

#include <esp_now.h>
#include <WiFi.h>

DHT dht(DHTPIN, DHTTYPE);

long duration;

 //uint8_t broadcastAddress[] = { 0x34,0x98,0x7A,0xA6,0x6C,0x44}; //lilygo t display mac
uint8_t broadcastAddress[] = { 0xAC,0x15,0x18,0xE5,0x04,0xDC}; //esp32 mac


typedef struct struct_message {
 
  float temp;
  bool kapi;
  bool parlaklik;
  bool pir;
  int espid = 005;
} struct_message;
typedef struct req{
bool req = 0;

}req;

struct_message gonder;
req incoming;

esp_now_peer_info_t peerInfo;


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  logprint(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incoming, incomingData, sizeof(incoming));
  
  Serial.print(incoming.req);
  }

void setup() {
  dht.begin();
 pinMode(IRPIN,INPUT);
 pinMode(PIRPIN,INPUT);
 pinMode(LDRPIN,INPUT);
  Serial.begin(115200);
 

  WiFi.mode(WIFI_STA);

 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

 
  esp_now_register_send_cb(OnDataSent);
  
 
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
       
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {



  
  gonder.kapi =  !digitalRead(IRPIN);
  gonder.temp = dht.readTemperature(); 
  gonder.pir = digitalRead(PIRPIN);
  gonder.parlaklik = !digitalRead(LDRPIN);

 if(incoming.req == 1){
  delay(200);
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &gonder, sizeof(gonder));
incoming.req = 0;
  
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
}
}
