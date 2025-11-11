/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>

#include <Wire.h>

#define AOUT_PIN 36 // ESP32 pin GPIO36 (ADC0) that connects to AOUT pin of moisture sensor

// this address is the station MAC addr of the esp32 you are SENDING to 
uint8_t broadcastAddress[] = {0xA0, 0xB7, 0x65, 0x63, 0x20, 0x34};  // noah's ESP info

String success;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  Serial.print("Bytes received: ");
  Serial.println(len);
  // Print the message
  char receivedMessage[len + 1]; // +1 for null terminator
  memcpy(receivedMessage, incomingData, len);
  receivedMessage[len] = '\0'; // Null-terminate the string

  // Serial.print("Message: ");
  // Serial.println(receivedMessage);
}
 
void setup() {
  // Init Serial Monitor
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
  int value = analogRead(AOUT_PIN); // read the analog value from sensor

  Serial.print("Moisture value: ");
  Serial.print(value);

  String message = String(value);

  if(value > 2200) {
    Serial.println(" dry");
    message += " - dry";
  }
  else {
    Serial.println(" wet");
    message += " - wet";
  }

  // Convert String to C-style string
  char testMessage[50]; // Make sure it's large enough
  message.toCharArray(testMessage, sizeof(testMessage));

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)testMessage, strlen(testMessage) + 1);
  
  if (result == ESP_OK) {
    Serial.println("Sent message!");
  } else {
    Serial.print("Error sending message: ");
    Serial.println(result);
  }

  delay(3000);  // wait 3 seconds before sending again
}