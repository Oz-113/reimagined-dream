
#include <esp_now.h>
#include <WiFi.h>
TaskHandle_t task_loop1;
esp_now_peer_info_t peerInfo;

typedef struct struct_message {

  float temp;
  bool kapi;
  bool parlaklik;
  bool pir;
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
uint8_t addr_esp32_1[] = {0xA0,0xDD,0x6C,0xB2,0x41,0xB8 };
uint8_t addr_esp32_2[] = {0x08,0x3A,0xF2,0xA4,0x42,0x64 }; 
uint8_t addr_esp32_3[] = {0xa0,0xb7,0x65,0x1f,0xe8,0x20 }; 
uint8_t addr_esp32_4[] = {0xa0,0xb7,0x65,0x24,0x57,0xf8 }; 

uint8_t * macler[] = {
  addr_lilygo_pin,
  addr_esp32_1,
  addr_esp32_2,
  addr_esp32_3,
  addr_esp32_4
  };

// Callback function executed when data is received
void serialencode(){
if(last_sent_id==incoming.espid){
  return;
}else{

  Serial2.print(incoming.espid);
  Serial2.print("?");
  // Serial2.print(incoming.a);
  // Serial2.print("&");
    Serial2.print(incoming.kapi);
    Serial2.print("$");
  Serial2.print(incoming.pir);
Serial2.print("$");
  Serial2.print(incoming.parlaklik);
  Serial2.print("$");
   Serial2.print(incoming.temp);
  Serial2.print("!");
  Serial2.print("#");
  Serial.println("serial sent!");
  // last_sent_id = incoming.espid;
  }
}


  
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

 

}


void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incoming, incomingData, sizeof(incoming));
 
serialencode();

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
  for(int i=0;i<5;i++){
  memcpy(peerInfo.peer_addr, macler[i], 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
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
  for(int i=0;i<5;i++){
// esp_err_t result = 
 esp_now_send(macler[i], (uint8_t *) &request, sizeof(request));

  }
  
  request.req = 0;
  }

if(millis()>tim +1000){

  tim=millis();
 }

}
 }

