#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

// ADC pins
#define AOUT_PIN 35
#define AOUT_PIN2 34
#define AOUT_PIN3 32

// WiFi
const char* ssid = "sUpBr0 (2)";
const char* password = "JustinBieber";

// Shelly RGBW PM
const char* shellyIP = "172.20.10.2";
const int W_CHANNEL = 3;

// Moisture variables
int lastMoistureValue1 = 0;
int lastMoistureValue2 = 0;
int lastMoistureValue3 = 0;

unsigned long lastReadTime = 0;
const unsigned long interval = 5000; // Check every 5 seconds (less frequent)

// Soil moisture thresholds
const int WET_THRESHOLD = 1750;
const int DRY_THRESHOLD = 2000;

// Watering control - simplified
unsigned long lastWateringTime = 0;
const unsigned long MIN_WATERING_INTERVAL = 30000; // 30 seconds between waterings

WebServer server(80);
HTTPClient http;

// === Shelly Control with Built-in Timer ===
void startWateringCycle() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected - cannot water");
    return;
  }

  String url = String("http://") + shellyIP + "/rpc/Light.Set";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(3000);
  
  // Turn on with 2-second auto-off timer built into Shelly
  String payload = String("{\"id\":") + W_CHANNEL + 
                   ",\"on\":true,\"brightness\":100,\"auto_off\":2}";
  
  Serial.print("🚰 Starting 2-second watering cycle... ");
  
  int code = http.POST(payload);
  
  if (code == 200) {
    Serial.println("SUCCESS - Shelly will auto-turn-off in 2 seconds!");
    lastWateringTime = millis();
  } else {
    Serial.printf("FAILED (HTTP %d)\n", code);
  }
  
  http.end();
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  server.on("/moisture", HTTP_GET, []() {
    StaticJsonDocument<200> doc;
    doc["value1"] = lastMoistureValue1;
    doc["value2"] = lastMoistureValue2;
    doc["value3"] = lastMoistureValue3;
    doc["lastWatering"] = (millis() - lastWateringTime) / 1000; // seconds since last watering

    String json;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    serializeJson(doc, json);
    server.send(200, "application/json", json);
  });

  // Manual watering endpoint for testing
  server.on("/water", HTTP_GET, []() {
    startWateringCycle();
    server.send(200, "text/plain", "Watering cycle started!");
  });

  server.begin();
  Serial.println("Server started");
  Serial.println("Visit /water to manually trigger watering");
  Serial.println("Visit /moisture to see sensor readings");
}

void loop() {
  server.handleClient();
  
  unsigned long now = millis();
  
  // Read sensors periodically
  if (now - lastReadTime >= interval) {
    lastReadTime = now;
    
    // Simple sensor readings
    lastMoistureValue1 = analogRead(AOUT_PIN);
    lastMoistureValue2 = analogRead(AOUT_PIN2);
    lastMoistureValue3 = analogRead(AOUT_PIN3);

    Serial.printf("Moisture: %d | %d | %d (control) | Last watering: %ds ago\n", 
                 lastMoistureValue1, lastMoistureValue2, lastMoistureValue3,
                 (now - lastWateringTime) / 1000);

    // Check if watering needed
    bool soilIsDry = lastMoistureValue3 >= DRY_THRESHOLD;
    bool enoughTimePassedSinceLastWatering = (now - lastWateringTime) >= MIN_WATERING_INTERVAL;
    
    if (soilIsDry && enoughTimePassedSinceLastWatering) {
      startWateringCycle();
    } else if (soilIsDry && !enoughTimePassedSinceLastWatering) {
      Serial.printf("Soil dry but waiting %d more seconds before next watering\n", 
                   (MIN_WATERING_INTERVAL - (now - lastWateringTime)) / 1000);
    }
  }

  delay(100);
}