/*
  ESP-NOW Demo - Receive
  esp-now-demo-rcv.ino
  Reads data from Initiator
  
  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/

// Include Libraries
#include <esp_now.h>
#include <WiFi.h>
TaskHandle_t task_loop1;
esp_now_peer_info_t peerInfo;
// Define a data structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
  int dist;
  int espid;
} struct_message;
typedef struct req {bool req;}req;

int last_sent_id;
int hedef_id;
int pos = 0;
 bool req_ser = 0;
  long tim = 0;
// Create a structured object
struct_message incoming;

 req request;
String uzaklik = "";


uint8_t addr_lilygo_pin[] = { 0x34,0x98,0x7A,0xA6,0x6C,0x44};

uint8_t addr_lilygo_nopin[] = { 0x2c,0xbc,0xbb,0xb7,0x54,0x10}; 

// Callback function executed when data is received
void serialencode(){


  Serial2.print(incoming.espid);
  Serial2.print("?");
  Serial2.print(incoming.a);
  Serial2.print("&");
    Serial2.print(incoming.dist);
    Serial2.print("$");
  Serial2.print(incoming.b);
Serial2.print("$");
  Serial2.print(incoming.d);
  Serial2.print("$");
   Serial2.print(incoming.c);
  Serial2.print("!");
  Serial2.print("#");
  last_sent_id = incoming.espid;
}


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incoming, incomingData, sizeof(incoming));
 
switch (incoming.espid){
  case 001:
  Serial.print("Data received: ");
  Serial.println(len);
  Serial.print("mesg: ");
  Serial.println(incoming.a);
  Serial.print("Integer Value: ");
  Serial.println(incoming.b);
  Serial.print("sıcaklık value: ");
  Serial.println(incoming.c);
  Serial.print("Boolean Value: ");
  Serial.println(incoming.d);
  Serial.print("distance: ");
  Serial.println(incoming.dist);
  Serial.println();
if(req_ser == 1 && last_sent_id != 001){serialencode();req_ser = 0;}

  break;
  case 002:
    Serial.print("Data received: ");
  Serial.println(len);
  Serial.print("mesg: ");
  Serial.println(incoming.a);
  Serial.print("int 1-100: ");
  Serial.println(incoming.b);
  Serial.print("float value: ");
  Serial.println(incoming.c);
  Serial.print("Boolean Value: ");
  Serial.println(incoming.d);
  Serial.print("int 100-200: ");
  Serial.println(incoming.dist);
  Serial.println();
 if(req_ser == 1 && last_sent_id != 002){serialencode();req_ser = 0;}
break;
}

}


void setup() {
   
 xTaskCreatePinnedToCore(                            //arduinonun çalışmadığı core'a sabit fonksiyon(int main ama yan sanayi gibi bişey) :91
    mainamaikinci,              
    "log display handler",                
    10000,                
    NULL,                 
    tskIDLE_PRIORITY,                    
    &task_loop1,            
    !ARDUINO_RUNNING_CORE); 





  // Set up Serial Monitor
  pinMode(18,OUTPUT);
  Serial.begin(115200);
  Serial2.begin(115200,SERIAL_8N1,17,16);
  
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false; 
  memcpy(peerInfo.peer_addr, addr_lilygo_pin, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
    memcpy(peerInfo.peer_addr, addr_lilygo_nopin, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
  // Register callback function
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
    esp_now_register_send_cb(OnDataSent);

}
 
void loop() {
  if(Serial2.available()){
if(Serial2.read() == '.'){
 req_ser = 1;
  request.req = 1;
}

  }




 }
 void mainamaikinci(void * parameter){
  vTaskDelay(4000);
for(;;){

if(request.req == 1){
  
 esp_err_t result_req001 = esp_now_send(addr_lilygo_pin, (uint8_t *) &request, sizeof(request));
  esp_err_t result_req002 = esp_now_send(addr_lilygo_nopin, (uint8_t *) &request, sizeof(request));
  request.req = 0;
  }
if(millis()>tim +1000){

  tim=millis();
 }

}
 }

