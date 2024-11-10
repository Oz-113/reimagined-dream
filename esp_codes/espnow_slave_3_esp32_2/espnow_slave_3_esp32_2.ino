
/*
  ESP-NOW Demo - Transmit
  esp-now-demo-xmit.ino
  Sends data to Responder
  
  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/

// Include Libraries
#include <esp_now.h>
#include <WiFi.h>

// Variables for test data
int int_value;
float float_value;
bool bool_value = true;
long duration;
// MAC Address of responder - edit as required
 //uint8_t broadcastAddress[] = { 0x34,0x98,0x7A,0xA6,0x6C,0x44}; //lilygo mac
uint8_t broadcastAddress[] = { 0xAC,0x15,0x18,0xE5,0x04,0xDC}; //esp32 mac

// Define a data structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
  int dist;
  int espid = 003;
} struct_message;
typedef struct req{
bool req = 0;

}req;
// Create a structured object
struct_message gonder;
req incoming;
// Peer info
esp_now_peer_info_t peerInfo;

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incoming, incomingData, sizeof(incoming));
  
  Serial.print(incoming.req);
  }

void setup() {

 
  // Set up Serial Monitor
  Serial.begin(115200);
 
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {

  // Create test data

  // Generate a random integer

  int_value = random(11,20);


  // Use integer to make a new float
  float_value = (random(11,20) * 1.3);

  // Invert the boolean value
  bool_value = !bool_value;
  
  // Format structured data
  strcpy(gonder.a, "Hi from esp-slave-3-esp32-2");
  gonder.b = int_value;
  gonder.c = float_value;
  gonder.d = bool_value;
  gonder.dist = random(100,200);
  // Serial.println(int_value);
  // Send message via ESP-NOW
 if(incoming.req == 1){
  delay(300);
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
