#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

WebServer server(80);

void handleWifiConfig() {
  if (server.method() == HTTP_POST) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (error) {
      server.send(400, "text/plain", "Invalid JSON");
      Serial.println("❌ JSON parse error");
      return;
    }

    const char* ssid = doc["ssid"];
    const char* password = doc["password"];

    Serial.println("📡 Attempting to connect with:");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);

    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts++ < 100) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ Connected to Wi-Fi!");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      
      // Create JSON response with IP address
      StaticJsonDocument<200> response;
      response["status"] = "success";
      response["message"] = "Connected successfully";
      response["ip"] = WiFi.localIP().toString();
      
      String responseString;
      serializeJson(response, responseString);
      
      server.send(200, "application/json", responseString);
    } else {
      Serial.println("\n❌ Failed to connect to Wi-Fi.");
      
      // Create JSON error response
      StaticJsonDocument<200> response;
      response["status"] = "error";
      response["message"] = "Failed to connect to Wi-Fi";
      
      String responseString;
      serializeJson(response, responseString);
      
      server.send(500, "application/json", responseString);
    }
  } else {
    server.send(405, "text/plain", "Use POST");
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.softAP("FLOWERS-SETUP", "12345678");
  Serial.print("🚀 AP started at IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", "<h2>ESP32 is online and ready for Wi-Fi setup.</h2>");
  });

  server.on("/wifi", HTTP_POST, handleWifiConfig);

  server.begin();
}

void loop() {
  server.handleClient();
}