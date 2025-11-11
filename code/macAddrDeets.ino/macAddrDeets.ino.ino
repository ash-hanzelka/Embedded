// to get your MAC details for communication
//


#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>


const char* ssid = "sUpBr0 (2)";
const char* password = "JustinBieber";


void setup() {
  Serial.begin(115200);

  // Variable to store the MAC address
  uint8_t baseMac[6];

  // Get MAC address of the WiFi station interface
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  Serial.print("Station MAC: ");
  for (int i = 0; i < 5; i++) {
    Serial.printf("%02X:", baseMac[i]);
  }
  Serial.printf("%02X\n", baseMac[5]);

  // set the ADC attenuation to 11 dB (up to ~3.3V input)
  analogSetAttenuation(ADC_11db);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Station MAC: ");
  Serial.println(WiFi.macAddress());

  // Get the MAC address of the Wi-Fi AP interface
  esp_read_mac(baseMac, ESP_MAC_WIFI_SOFTAP);
  Serial.print("SoftAP MAC: ");
  for (int i = 0; i < 5; i++) {
    Serial.printf("%02X:", baseMac[i]);
  }
  Serial.printf("%02X\n", baseMac[5]);

  // Get the MAC address of the Bluetooth interface
  esp_read_mac(baseMac, ESP_MAC_BT);
  Serial.print("Bluetooth MAC: ");
  for (int i = 0; i < 5; i++) {
    Serial.printf("%02X:", baseMac[i]);
  }
  Serial.printf("%02X\n", baseMac[5]);

  // Get the MAC address of the Ethernet interface
  esp_read_mac(baseMac, ESP_MAC_ETH);
  Serial.print("Ethernet MAC: ");
  for (int i = 0; i < 5; i++) {
    Serial.printf("%02X:", baseMac[i]);
  }
  Serial.printf("%02X\n", baseMac[5]);

}


void loop() {

}
