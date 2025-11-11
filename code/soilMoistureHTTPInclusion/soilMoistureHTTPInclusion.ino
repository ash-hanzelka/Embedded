// run this, modifying ssid/password as needed
// 

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// 34, 35, & 32
#define AOUT_PIN 35
#define AOUT_PIN2 34
#define AOUT_PIN3 32

const char* ssid = "Zuperior WiFi";
const char* password = "Qwerty123!";

WebServer server(80);

int lastMoistureValue1 = 0;
int lastMoistureValue2 = 0;
int lastMoistureValue3 = 0;

String lastStatus1 = "unknown";
String lastStatus2 = "unknown";
String lastStatus3 = "unknown";

unsigned long lastReadTime1 = 0;
unsigned long lastReadTime2 = 0;
unsigned long lastReadTime3 = 0;

const unsigned long interval = 1000; // 1 second

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

  delay(1000);

  server.on("/moisture", HTTP_GET, []() {
    StaticJsonDocument<200> doc;
    doc["value1"] = lastMoistureValue1;
    doc["value2"] = lastMoistureValue2;
    doc["value3"] = lastMoistureValue3;

    doc["status1"] = lastStatus1;
    doc["status2"] = lastStatus2;
    doc["status3"] = lastStatus3;

    String json;
    server.sendHeader("Access-Control-Allow-Origin", "*");  // ADD THIS LINE
    serializeJson(doc, json);
    server.send(200, "application/json", json);
  });

  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();

  unsigned long now = millis();
  if (now - lastReadTime1 >= interval) {
    lastReadTime1 = now;

    lastMoistureValue1 = analogRead(AOUT_PIN);
    lastMoistureValue2 = analogRead(AOUT_PIN2);
    lastMoistureValue3 = analogRead(AOUT_PIN3);

    lastStatus1 = lastMoistureValue1 > 2200 ? "dry" : "wet";
    lastStatus2 = lastMoistureValue2 > 2200 ? "dry" : "wet";
    lastStatus3 = lastMoistureValue3 > 2200 ? "dry" : "wet";

    Serial.print("Updated moisture 1: ");
    Serial.print(lastMoistureValue1);
    Serial.print(" - ");
    Serial.println(lastStatus1);
    Serial.println(WiFi.localIP());

    Serial.print("Updated moisture 2: ");
    Serial.print(lastMoistureValue2);
    Serial.print(" - ");
    Serial.println(lastStatus2);

    Serial.print("Updated moisture 3: ");
    Serial.print(lastMoistureValue3);
    Serial.print(" - ");
    Serial.println(lastStatus3);
  }
}
