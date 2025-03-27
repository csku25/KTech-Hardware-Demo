/*
rps.ino
The Arduino "language" is just C++.

This demo uses an ESP32 DevKit microcontroller,
a few buttons and LED bulbs.

Press some of the buttons on one board and the lights will
toggle on the other. Both boards are wirelessly connected through
the ESPNOW protocol, requiring the other board's MAC address.
*/

#include <esp_now.h>
#include <WiFi.h>

#define WLED_Pin 13
#define RLED_Pin 12
#define GLED_Pin 14

#define TOP_BTN_Pin 1
#define MID_BTN_Pin 23
#define BTM_BTN_Pin 5

int TOP_BTN_State;
int MID_BTN_State;
int BTM_BTN_State;

uint8_t broadcastAddress[] = {0x14, 0x2B, 0x2F, 0xDA, 0xCC, 0x3C}; //--> REPLACE WITH MAC ADDR OF OTHER RECV BOARD

int WLED_State_Send = 0;
int WLED_State_Receive;
int RLED_State_Send = 0;
int RLED_State_Receive;
int GLED_State_Send = 0;
int GLED_State_Receive;

String success;

typedef struct struct_message {
    int wled;
    int rled;
    int gled;
} struct_message_send;

struct_message send_Data;
struct_message receive_Data;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
  Serial.println(">>>>>");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&receive_Data, incomingData, sizeof(receive_Data));
  Serial.println();
  Serial.println("<<<<< Receive Data:");
  Serial.print("Bytes received: ");
  Serial.println(len);

  WLED_State_Receive = receive_Data.wled; // White LED state
  RLED_State_Receive = receive_Data.rled; // Red LED state
  GLED_State_Receive = receive_Data.gled; // Green LED state

  Serial.print("Received Data (WLED State): ");
  Serial.println(WLED_State_Receive);
  Serial.print("Received Data (RLED State): ");
  Serial.println(RLED_State_Receive);
  Serial.print("Received Data (GLED State): ");
  Serial.println(GLED_State_Receive);
  
  // Control LEDs based on received states
  digitalWrite(WLED_Pin, WLED_State_Receive);
  digitalWrite(RLED_Pin, RLED_State_Receive);
  digitalWrite(GLED_Pin, GLED_State_Receive);

  Serial.println("<<<<<");
}

void setup() {
  Serial.begin(115200);

  pinMode(WLED_Pin, OUTPUT);
  pinMode(RLED_Pin, OUTPUT);
  pinMode(GLED_Pin, OUTPUT);
  pinMode(TOP_BTN_Pin, INPUT);
  pinMode(MID_BTN_Pin, INPUT);
  pinMode(BTM_BTN_Pin, INPUT);
  
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo;
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
  TOP_BTN_State = digitalRead(TOP_BTN_Pin);
  MID_BTN_State = digitalRead(MID_BTN_Pin);
  BTM_BTN_State = digitalRead(BTM_BTN_Pin);
  
  if (TOP_BTN_State == 1) {
    WLED_State_Send = !WLED_State_Send;
    send_Data.wled = WLED_State_Send;

    Serial.println();
    Serial.print(">>>>> ");
    Serial.println("Send data");
  
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
     
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    } else {
      Serial.println("Error sending the data");
    }
    
    while (TOP_BTN_State == 1) {
      TOP_BTN_State = digitalRead(TOP_BTN_Pin);
      delay(10);
    }
  }



  if (MID_BTN_State == 1) {
    RLED_State_Send = !RLED_State_Send;
    send_Data.rled = RLED_State_Send;

    Serial.println();
    Serial.print(">>>>> ");
    Serial.println("Send data");
  
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
     
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    } else {
      Serial.println("Error sending the data");
    }
    
    while (MID_BTN_State == 1) {
      MID_BTN_State = digitalRead(MID_BTN_Pin);
      delay(10);
    }
  }



  if (BTM_BTN_State == 1) {
    GLED_State_Send = !GLED_State_Send;
    send_Data.gled = GLED_State_Send;

    Serial.println();
    Serial.print(">>>>> ");
    Serial.println("Send data");
  
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
     
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    } else {
      Serial.println("Error sending the data");
    }
    
    while (BTM_BTN_State == 1) {
      BTM_BTN_State = digitalRead(BTM_BTN_Pin);
      delay(10);
    }
  }
}
