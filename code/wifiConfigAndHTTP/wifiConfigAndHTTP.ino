// necessary libraries
#include <WiFi.h>               // for wifi connectivity
#include <WebServer.h>          // for web server creation
#include <ArduinoJson.h>        // for JSON operations
#include <Wire.h>               // for the i2c comm
#include "Adafruit_VEML7700.h"  // for the light sensor
#include <Preferences.h>        // for storing/retrieving data from flash

// System State management
// define the system states for wifi
enum SystemState {
  WIFI_SETUP_MODE,  // device awaiting wifi config
  MONITORING_MODE   // wifi is connected and can now monitor
};

SystemState currentState = WIFI_SETUP_MODE;  // the default boot mode is wifi setup
Preferences preferences;                     // for persistent storage

WebServer server(80);  // creates web server on port 80

// Soil Moisture Configuration
#define AOUT_PIN 35  //
#define AOUT_PIN2 34
#define AOUT_PIN3 32

// Last readings storage
int lastMoistureValue1 = 0;
int lastMoistureValue2 = 0;
int lastMoistureValue3 = 0;
String lastMoistureStatus1 = "unknown";
String lastMoistureStatus2 = "unknown";
String lastMoistureStatus3 = "unknown";

// ========== Environmental Sensor Configuration ==========
#define PCA9547D_ADDR 0x70
#define SHT4X_ADDR 0x44

// SHT4x sensor configuration
#define SHT4X_CHANNEL_START 0
#define SHT4X_CHANNEL_END 2
#define SHT4X_COUNT 3

// VEML7700 sensor configuration
#define VEML7700_CHANNEL_START 5
#define VEML7700_CHANNEL_END 7
#define VEML7700_COUNT 3

// SHT4x commands
#define SHT4X_CMD_MEASURE_HIGH_PRECISION 0xFD
#define SHT4X_CMD_SOFT_RESET 0x94

// Sensor objects
Adafruit_VEML7700 veml_sensors[VEML7700_COUNT];

// Last environmental readings
float lastTemp[3] = { 0 };
String lastTempStatus[3] = { "" };
float lastHumidity[3] = { 0 };
String lastHumidityStatus[3] = { "" };
float lastLux[3] = { 0 };
String lastLightStatus[3] = { "" };

// ========== Cubbies Data Storage ==========
struct CubbyData {
  int lightLower;          // Added: lower bound for light
  int lightUpper;          // Added: upper bound for light
  int soilLower;
  int soilUpper;
  int humidityLower;       // Added: lower bound for humidity
  int humidityUpper;       // Added: upper bound for humidity
  int temperatureLower;
  int temperatureUpper;
  unsigned long lastUpdated;
  bool hasData;
};


CubbyData cubbiesData[3] = { { 0, 0, 0, 0, 0, true }, { 0, 0, 0, 0, 0, true }, { 0, 0, 0, 0, 0, true } };

// ========== Timing Configuration ==========
unsigned long lastReadTime = 0;
const unsigned long readInterval = 1000;  // 1 seconds between readings

// ========== WiFi Setup Functions ==========
void handleWifiConfig() {
  if (server.method() == HTTP_POST) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (error) {
      server.send(400, "text/plain", "Invalid JSON");
      Serial.println("\n✗✗✗✗✗ JSON parse error");
      return;
    }

    const char* ssid = doc["ssid"];
    const char* password = doc["password"];

    Serial.println("ᯤ ᯤ ᯤ Attempting to connect with:");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);

    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts++ < 30) { // Reduced from 100 to 30 attempts
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✔✔✔✔ Connected to Wi-Fi!");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());

      // Save WiFi credentials
      preferences.begin("wifi", false);
      preferences.putString("ssid", ssid);
      preferences.putString("password", password);
      preferences.end();

      // Create JSON response with IP address
      StaticJsonDocument<200> response;
      response["status"] = "success";
      response["message"] = "Connected successfully! Switching to monitoring mode...";
      response["ip"] = WiFi.localIP().toString();

      String responseString;
      serializeJson(response, responseString);

      server.send(200, "application/json", responseString);

      // Wait a moment for response to send, then switch modes
      delay(2000);
      switchToMonitoringMode();

    } else {
      Serial.println("\n!!!!!!! Failed to connect to Wi-Fi.");
      
      // IMPORTANT: Disconnect from failed WiFi attempt and restart AP mode
      WiFi.disconnect();
      delay(1000);
      
      // Clear any potentially saved bad credentials
      preferences.begin("wifi", false);
      preferences.remove("ssid");
      preferences.remove("password");
      preferences.end();

      // Create JSON error response
      StaticJsonDocument<200> response;
      response["status"] = "error";
      response["message"] = "Failed to connect to Wi-Fi. Please check credentials and try again.";

      String responseString;
      serializeJson(response, responseString);

      server.send(500, "application/json", responseString);
      
      // Restart the access point to allow retry
      Serial.println("🔄 Restarting access point for retry...");
      delay(2000); // Give time for response to be sent
      
      // Restart AP mode
      WiFi.mode(WIFI_AP);
      WiFi.softAP("FLOWERS-SETUP", "12345678");
      
      Serial.print("🚀 AP restarted at IP: ");
      Serial.println(WiFi.softAPIP());
    }
  } else {
    server.send(405, "text/plain", "Use POST");
  }
}

void handleSetupRoot() {
  String html = "<!DOCTYPE html><html><head><title>Plant Monitor Setup</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:Arial;margin:20px;background:#f0f0f0;}";
  html += ".container{max-width:400px;margin:0 auto;background:white;padding:20px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1);}";
  html += "h1{color:#2c3e50;text-align:center;}";
  html += "input{width:100%;padding:10px;margin:10px 0;border:1px solid #ddd;border-radius:5px;box-sizing:border-box;}";
  html += "button{width:100%;padding:12px;background:#3498db;color:white;border:none;border-radius:5px;cursor:pointer;font-size:16px;}";
  html += "button:hover{background:#2980b9;}";
  html += ".status{margin-top:10px;padding:10px;border-radius:5px;display:none;}";
  html += ".success{background:#d4edda;color:#155724;border:1px solid #c3e6cb;}";
  html += ".error{background:#f8d7da;color:#721c24;border:1px solid #f5c6cb;}";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>🌱 Plant Monitor Setup</h1>";
  html += "<p>Connect your plant monitoring system to WiFi:</p>";
  html += "<form id='wifiForm'>";
  html += "<input type='text' id='ssid' placeholder='WiFi Network Name (SSID)' required>";
  html += "<input type='password' id='password' placeholder='WiFi Password' required>";
  html += "<button type='submit'>Connect to WiFi</button>";
  html += "</form>";
  html += "<div id='status' class='status'></div>";
  html += "</div>";
  html += "<script>";
  html += "document.getElementById('wifiForm').addEventListener('submit', function(e) {";
  html += "e.preventDefault();";
  html += "const ssid = document.getElementById('ssid').value;";
  html += "const password = document.getElementById('password').value;";
  html += "const statusDiv = document.getElementById('status');";
  html += "statusDiv.style.display = 'block';";
  html += "statusDiv.className = 'status';";
  html += "statusDiv.innerHTML = 'Connecting...';";
  html += "fetch('/wifi', {";
  html += "method: 'POST',";
  html += "headers: {'Content-Type': 'application/json'},";
  html += "body: JSON.stringify({ssid: ssid, password: password})";
  html += "}).then(response => response.json()).then(data => {";
  html += "if (data.status === 'success') {";
  html += "statusDiv.className = 'status success';";
  html += "statusDiv.innerHTML = 'Connected! IP: ' + data.ip + '<br>Switching to monitoring mode...';";
  html += "} else {";
  html += "statusDiv.className = 'status error';";
  html += "statusDiv.innerHTML = 'Error: ' + data.message;";
  html += "}";
  html += "}).catch(error => {";
  html += "statusDiv.className = 'status error';";
  html += "statusDiv.innerHTML = 'Connection failed. Please try again.';";
  html += "});";
  html += "});";
  html += "</script></body></html>";

  server.send(200, "text/html", html);
}

void setupWiFiAccessPoint() {
  Serial.println("🔧 Starting WiFi Setup Mode...");

  WiFi.mode(WIFI_AP);
  WiFi.softAP("FLOWERS-SETUP", "12345678");

  Serial.print("🚀 AP started at IP: ");
  Serial.println(WiFi.softAPIP());

  // Setup routes for configuration mode
  server.on("/", HTTP_GET, handleSetupRoot);
  server.on("/wifi", HTTP_POST, handleWifiConfig);

  server.begin();
  Serial.println("Setup server started");
}

// ========== Mode Switching ==========
void switchToMonitoringMode() {
  Serial.println("🔄 Switching to monitoring mode...");

  // Stop the server
  server.stop();

  // Stop AP mode
  WiFi.softAPdisconnect(true);

  // Change system state
  currentState = MONITORING_MODE;

  // Initialize sensors
  initializeSensors();

  // Setup monitoring server routes
  setupMonitoringServer();

  Serial.println("✅ Monitoring mode activated!");
  Serial.print("Monitor at: http://");
  Serial.println(WiFi.localIP());
}

// Modified tryStoredWiFiConnection to be more strict about timeout
bool tryStoredWiFiConnection() {
  preferences.begin("wifi", true);
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");
  preferences.end();

  if (ssid.length() > 0) {
    Serial.println("📡 Attempting to connect with stored credentials...");
    Serial.print("SSID: ");
    Serial.println(ssid);

    WiFi.begin(ssid.c_str(), password.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts++ < 30) { // Reduced timeout
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ Connected to stored WiFi!");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      return true;
    } else {
      Serial.println("\n❌ Failed to connect with stored credentials.");
      
      // Clear bad stored credentials
      preferences.begin("wifi", false);
      preferences.remove("ssid");
      preferences.remove("password");
      preferences.end();
      
      WiFi.disconnect();
    }
  }

  return false;
}

// ========== Sensor Initialization ==========
void initializeSensors() {
  // Initialize I2C
  Wire.begin(13, 14);  // SDA=GPIO13, SCL=GPIO14
  Wire.setClock(100000);

  // Test multiplexer
  testPCA9547D();

  // Initialize environmental sensors
  initEnvironmentalSensors();
}

void initEnvironmentalSensors() {
  // Initialize SHT4x sensors
  for (int channel = SHT4X_CHANNEL_START; channel <= SHT4X_CHANNEL_END; channel++) {
    initSHT4x(channel);
  }

  // Initialize VEML7700 sensors
  for (int channel = VEML7700_CHANNEL_START; channel <= VEML7700_CHANNEL_END; channel++) {
    int sensor_index = channel - VEML7700_CHANNEL_START;
    initVEML7700(channel, sensor_index);
  }
}

bool initSHT4x(int channel) {
  if (!selectChannel(channel)) return false;

  Wire.beginTransmission(SHT4X_ADDR);
  Wire.write(SHT4X_CMD_SOFT_RESET);
  return Wire.endTransmission() == 0;
}

bool initVEML7700(int channel, int sensor_index) {
  if (!selectChannel(channel)) return false;

  if (veml_sensors[sensor_index].begin()) {
    veml_sensors[sensor_index].setGain(VEML7700_GAIN_1);
    veml_sensors[sensor_index].setIntegrationTime(VEML7700_IT_100MS);
    return true;
  }
  return false;
}

bool selectChannel(int channel) {
  if (channel < 0 || channel > 7) return false;

  Wire.beginTransmission(PCA9547D_ADDR);
  Wire.write(0x08 | channel);
  return Wire.endTransmission() == 0;
}

void testPCA9547D() {
  Wire.beginTransmission(PCA9547D_ADDR);
  if (Wire.endTransmission() == 0) {
    Serial.println("PCA9547D multiplexer detected");
  } else {
    Serial.println("PCA9547D multiplexer not found!");
  }
}

// ========== Cubbies Handler ==========
void handleCubbies() {

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");

  if (server.method() == HTTP_POST) {
    // Set CORS headers
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");

    String body = server.arg("plain");
    Serial.println("📦 Received cubbies data: " + body);

    // Parse JSON
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.println("❌ JSON parsing failed: " + String(error.c_str()));
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON format\"}");
      return;
    }

    // Validate and store cubby data
    bool validData = true;
    String errorMessage = "";
    int updatedCubbies = 0;

    for (int i = 0; i < 3; i++) {
      String cubbyKey = "cubby" + String(i + 1);

      // Check if this cubby exists in the incoming data
      if (doc.containsKey(cubbyKey)) {
        JsonObject cubby = doc[cubbyKey];

        // Validate that all required fields are present (updated to include new fields)
        if (cubby.containsKey("lightLower") && cubby.containsKey("lightUpper") && 
            cubby.containsKey("soilLower") && cubby.containsKey("soilUpper") && 
            cubby.containsKey("humidityLower") && cubby.containsKey("humidityUpper") && 
            cubby.containsKey("temperatureLower") && cubby.containsKey("temperatureUpper")) {

          // Store the data in cubbiesData array
          cubbiesData[i].lightLower = cubby["lightLower"];
          cubbiesData[i].lightUpper = cubby["lightUpper"];
          cubbiesData[i].soilLower = cubby["soilLower"];
          cubbiesData[i].soilUpper = cubby["soilUpper"];
          cubbiesData[i].humidityLower = cubby["humidityLower"];
          cubbiesData[i].humidityUpper = cubby["humidityUpper"];
          cubbiesData[i].temperatureLower = cubby["temperatureLower"];
          cubbiesData[i].temperatureUpper = cubby["temperatureUpper"];
          cubbiesData[i].hasData = true;
          cubbiesData[i].lastUpdated = millis();

          updatedCubbies++;
          
          // Add debug logging
          Serial.printf("✅ Updated %s: lightL=%d, lightU=%d, soilL=%d, soilU=%d, humidityL=%d, humidityU=%d, tempL=%d, tempU=%d\n", 
                       cubbyKey.c_str(), 
                       cubbiesData[i].lightLower,
                       cubbiesData[i].lightUpper,
                       cubbiesData[i].soilLower,
                       cubbiesData[i].soilUpper,
                       cubbiesData[i].humidityLower,
                       cubbiesData[i].humidityUpper,
                       cubbiesData[i].temperatureLower,
                       cubbiesData[i].temperatureUpper);
        } else {
          Serial.println("⚠️ " + cubbyKey + " missing required sensor fields");
          validData = false;
          errorMessage += cubbyKey + " missing sensor data; ";
        }
      }
    }

    if (validData && updatedCubbies > 0) {
      StaticJsonDocument<200> response;
      response["status"] = "success";
      response["message"] = "Cubbies data received successfully";
      response["updated_cubbies"] = updatedCubbies;

      String responseString;
      serializeJson(response, responseString);
      server.send(200, "application/json", responseString);
      
      // Add debug: print current stored data
      Serial.println("Current stored cubby data:");
      for (int i = 0; i < 3; i++) {
        Serial.printf("Cubby %d: lightL=%d, lightU=%d, soilL=%d, soilU=%d, humidityL=%d, humidityU=%d, tempL=%d, tempU=%d, hasData=%s\n", 
                     i+1, cubbiesData[i].lightLower, cubbiesData[i].lightUpper, 
                     cubbiesData[i].soilLower, cubbiesData[i].soilUpper,
                     cubbiesData[i].humidityLower, cubbiesData[i].humidityUpper,
                     cubbiesData[i].temperatureLower, cubbiesData[i].temperatureUpper,
                     cubbiesData[i].hasData ? "true" : "false");
      }
    } else {
      StaticJsonDocument<300> response;
      response["status"] = "error";
      response["message"] = errorMessage.length() > 0 ? errorMessage : "No valid cubby data found";

      String responseString;
      serializeJson(response, responseString);
      server.send(400, "application/json", responseString);
    }

  } else if (server.method() == HTTP_GET) {
    // Return current cubbies data
    server.sendHeader("Access-Control-Allow-Origin", "*");

    StaticJsonDocument<1024> doc;

    for (int i = 0; i < 3; i++) {
      String cubbyKey = "cubby" + String(i + 1);

      // Always include sensor data fields, use 0 if no data
      doc[cubbyKey]["lightLower"] = cubbiesData[i].hasData ? cubbiesData[i].lightLower : 0;
      doc[cubbyKey]["lightUpper"] = cubbiesData[i].hasData ? cubbiesData[i].lightUpper : 0;
      doc[cubbyKey]["soilLower"] = cubbiesData[i].hasData ? cubbiesData[i].soilLower : 0;
      doc[cubbyKey]["soilUpper"] = cubbiesData[i].hasData ? cubbiesData[i].soilUpper : 0;
      doc[cubbyKey]["humidityLower"] = cubbiesData[i].hasData ? cubbiesData[i].humidityLower : 0;
      doc[cubbyKey]["humidityUpper"] = cubbiesData[i].hasData ? cubbiesData[i].humidityUpper : 0;
      doc[cubbyKey]["temperatureLower"] = cubbiesData[i].hasData ? cubbiesData[i].temperatureLower : 0;
      doc[cubbyKey]["temperatureUpper"] = cubbiesData[i].hasData ? cubbiesData[i].temperatureUpper : 0;
      doc[cubbyKey]["has_data"] = cubbiesData[i].hasData;

      // Optionally include last_updated if there's data
      if (cubbiesData[i].hasData) {
        doc[cubbyKey]["last_updated"] = cubbiesData[i].lastUpdated;
      }
    }

    String json;
    serializeJson(doc, json);
    server.send(200, "application/json", json);

  } else if (server.method() == HTTP_OPTIONS) {
    // Handle CORS preflight
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(200, "text/plain", "");
  } else {
    server.send(405, "text/plain", "Method not allowed");
  }
}

// ========== Monitoring Server Setup ==========
void setupMonitoringServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/sensors", HTTP_GET, handleData);
  server.on("/cubbies", HTTP_POST, handleCubbies);
  server.on("/cubbies", HTTP_GET, handleCubbies);
  server.on("/cubbies", HTTP_OPTIONS, handleCubbies);
  server.on("/reset", HTTP_GET, handleReset);
  server.begin();
  Serial.println("HTTP monitoring server started");
  Serial.print("📡 Cubbies endpoint available at: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/cubbies");
}

void handleReset() {
  Serial.println("🔄 Resetting WiFi settings...");

  // Clear stored credentials
  preferences.begin("wifi", false);
  preferences.clear();
  preferences.end();

  server.send(200, "text/html", "<h2>WiFi settings cleared. Restarting...</h2>");
  delay(2000);
  ESP.restart();
}

// ========== Soil Moisture Functions ==========
void readSoilMoisture() {
  lastMoistureValue1 = analogRead(AOUT_PIN);
  lastMoistureValue2 = analogRead(AOUT_PIN2);
  lastMoistureValue3 = analogRead(AOUT_PIN3);

  lastMoistureStatus1 = lastMoistureValue1 > 2200 ? "dry" : "wet";
  lastMoistureStatus2 = lastMoistureValue2 > 2200 ? "dry" : "wet";
  lastMoistureStatus3 = lastMoistureValue3 > 2200 ? "dry" : "wet";
}

// ========== Environmental Sensor Functions ==========
void readEnvironmentalSensors() {
  // Read temperature/humidity sensors
  for (int channel = SHT4X_CHANNEL_START; channel <= SHT4X_CHANNEL_END; channel++) {
    if (selectChannel(channel)) {
      readSHT4xData(channel);
    }
  }

  // Read light sensors
  for (int channel = VEML7700_CHANNEL_START; channel <= VEML7700_CHANNEL_END; channel++) {
    int sensor_index = channel - VEML7700_CHANNEL_START;
    if (selectChannel(channel)) {
      lastLux[sensor_index] = veml_sensors[sensor_index].readLux();

      String lightStatus;
      lightStatus = lastLux[sensor_index] > 150 ? "dim" : "dark";
      lightStatus = lastLux[sensor_index] > 500 ? "bright" : lightStatus;
      lastLightStatus[sensor_index] = lightStatus;
    }
  }
}

void readSHT4xData(int channel) {
  if (!selectChannel(channel)) return;

  Wire.beginTransmission(SHT4X_ADDR);
  Wire.write(SHT4X_CMD_MEASURE_HIGH_PRECISION);
  if (Wire.endTransmission() != 0) return;

  delay(10);

  Wire.requestFrom(SHT4X_ADDR, 6);
  if (Wire.available() < 6) return;

  byte data[6];
  for (int i = 0; i < 6; i++) data[i] = Wire.read();

  uint16_t temp_raw = (data[0] << 8) | data[1];
  uint16_t hum_raw = (data[3] << 8) | data[4];

  lastTemp[channel] = -45.0 + 175.0 * temp_raw / 65535.0;
  String tempStatus;
  tempStatus = lastTemp[channel] > 23 ? "warm" : "cold";
  tempStatus = lastTemp[channel] > 27 ? "hot" : tempStatus;
  lastTempStatus[channel] = tempStatus;

  lastHumidity[channel] = max(0.0, min(100.0, -6.0 + 125.0 * hum_raw / 65535.0));
  String humidityStatus;
  humidityStatus = lastHumidity[channel] > 40 ? "normal" : "dry";
  humidityStatus = lastHumidity[channel] > 65 ? "humid" : humidityStatus;
  lastHumidityStatus[channel] = humidityStatus;
}

// ========== Monitoring Web Server Functions ==========
void handleRoot() {
  String html = "<html><head><meta http-equiv='refresh' content='5'>";
  html += "<style>body{font-family:Arial;margin:20px;}";
  html += ".sensor{background:#f0f0f0;padding:15px;margin-bottom:10px;border-radius:5px;}";
  html += ".cubby{background:#e8f5e8;padding:15px;margin-bottom:10px;border-radius:5px;}";
  html += ".reset-btn{background:#e74c3c;color:white;padding:10px 20px;text-decoration:none;border-radius:5px;display:inline-block;margin-top:20px;}";
  html += "</style></head><body>";
  html += "<h1>🌱 Plant Monitoring System</h1>";

  // Soil moisture
  html += "<div class='sensor'><h2>Soil Moisture</h2>";
  html += "<p>Sensor 1: " + String(lastMoistureValue1) + " (" + lastMoistureStatus1 + ")</p>";
  html += "<p>Sensor 2: " + String(lastMoistureValue2) + " (" + lastMoistureStatus2 + ")</p>";
  html += "<p>Sensor 3: " + String(lastMoistureValue3) + " (" + lastMoistureStatus3 + ")</p></div>";

  // Environmental data
  html += "<div class='sensor'><h2>Environmental Sensors</h2>";
  for (int i = 0; i < 3; i++) {
    html += "<h3>Sensor Group " + String(i + 1) + "</h3>";
    html += "<p>Temperature: " + String(lastTemp[i], 1) + "°C (" + lastTempStatus[i] + ")</p>";
    html += "<p>Humidity: " + String(lastHumidity[i], 1) + "% (" + lastHumidityStatus[i] + ")</p>";
    html += "<p>Light: " + String(lastLux[i], 1) + " lux (" + lastLightStatus[i] + ")</p>";
  }
  html += "</div>";

  // Cubbies data - Updated to show new format
  html += "<div class='cubby'><h2>📦 Cubbies Data</h2>";
  for (int i = 0; i < 3; i++) {
    html += "<h3>Cubby " + String(i + 1) + "</h3>";
    if (cubbiesData[i].hasData) {
      html += "<p>Light: " + String(cubbiesData[i].lightLower) + "</p>";
      html += "<p>Light: " + String(cubbiesData[i].lightUpper) + "</p>";
      html += "<p>Soil Lower: " + String(cubbiesData[i].soilLower) + "</p>";
      html += "<p>Soil Upper: " + String(cubbiesData[i].soilUpper) + "</p>";
      html += "<p>Humidity: " + String(cubbiesData[i].humidityLower) + "</p>";
      html += "<p>Humidity: " + String(cubbiesData[i].humidityUpper) + "</p>";
      html += "<p>Temperature Lower: " + String(cubbiesData[i].temperatureLower) + "</p>";
      html += "<p>Temperature Upper: " + String(cubbiesData[i].temperatureUpper) + "</p>";
      html += "<p>Last Updated: " + String((millis() - cubbiesData[i].lastUpdated) / 1000) + "s ago</p>";
    } else {
      html += "<p>No data received</p>";
    }
  }
  html += "</div>";

  html += "<p>Auto-refresh every 5 seconds</p>";
  html += "<a href='/reset' class='reset-btn'>Reset WiFi Settings</a>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleData() {
  StaticJsonDocument<1024> doc;

  // Soil moisture data
  doc["soil"]["sensor1"]["value"] = lastMoistureValue1;
  doc["soil"]["sensor1"]["status"] = lastMoistureStatus1;
  doc["soil"]["sensor2"]["value"] = lastMoistureValue2;
  doc["soil"]["sensor2"]["status"] = lastMoistureStatus2;
  doc["soil"]["sensor3"]["value"] = lastMoistureValue3;
  doc["soil"]["sensor3"]["status"] = lastMoistureStatus3;

  // Environmental data
  for (int i = 0; i < 3; i++) {
    doc["environment"]["temperature" + String(i + 1)]["value"] = lastTemp[i];
    doc["environment"]["temperature" + String(i + 1)]["status"] = lastTempStatus[i];

    doc["environment"]["humidity" + String(i + 1)]["value"] = lastHumidity[i];
    doc["environment"]["humidity" + String(i + 1)]["status"] = lastHumidityStatus[i];

    doc["environment"]["light" + String(i + 1)]["value"] = lastLux[i];
    doc["environment"]["light" + String(i + 1)]["status"] = lastLightStatus[i];
  }

  // Cubbies data - Updated format
  for (int i = 0; i < 3; i++) {
    String cubbyKey = "cubby" + String(i + 1);
    doc[cubbyKey]["lightLower"] = cubbiesData[i].lightLower;
    doc[cubbyKey]["lightUpper"] = cubbiesData[i].lightUpper;
    doc[cubbyKey]["soilLower"] = cubbiesData[i].soilLower;
    doc[cubbyKey]["soilUpper"] = cubbiesData[i].soilUpper;
    doc[cubbyKey]["humidityLower"] = cubbiesData[i].humidityLower;
    doc[cubbyKey]["humidityLower"] = cubbiesData[i].humidityUpper;
    doc[cubbyKey]["temperatureLower"] = cubbiesData[i].temperatureLower;
    doc[cubbyKey]["temperatureUpper"] = cubbiesData[i].temperatureUpper;
    doc[cubbyKey]["has_data"] = cubbiesData[i].hasData;
  }

  String json;
  server.sendHeader("Access-Control-Allow-Origin", "*");
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

// ========== Debug Functions ==========
void printSensorData() {
  Serial.println("\n=== Sensor Readings ===");

  // Soil moisture
  Serial.println("Soil Moisture:");
  Serial.printf("  Sensor 1: %d (%s)\n", lastMoistureValue1, lastMoistureStatus1.c_str());
  Serial.printf("  Sensor 2: %d (%s)\n", lastMoistureValue2, lastMoistureStatus2.c_str());
  Serial.printf("  Sensor 3: %d (%s)\n", lastMoistureValue3, lastMoistureStatus3.c_str());

  // Environmental
  Serial.println("Environmental Sensors:");
  for (int i = 0; i < 3; i++) {
    Serial.printf("  Group %d: %.1f°C (%s), %.1f%% (%s), %.1f lux (%s)\n",
                  i + 1, lastTemp[i], lastTempStatus[i].c_str(),
                  lastHumidity[i], lastHumidityStatus[i].c_str(),
                  lastLux[i], lastLightStatus[i].c_str());
  }

  // Cubbies data - Updated debug output
  Serial.println("Cubbies Data:");
  for (int i = 0; i < 3; i++) {
    if (cubbiesData[i].hasData) {
      Serial.printf("  Cubby %d: Light=%d, SoilL=%d, SoilU=%d, Humidity=%d, TempL=%d, TempU=%d (Updated %lus ago)\n",
                    i + 1, cubbiesData[i].lightLower, cubbiesData[i].lightUpper, cubbiesData[i].soilLower, cubbiesData[i].soilUpper,
                    cubbiesData[i].humidityLower, cubbiesData[i].humidityUpper, cubbiesData[i].temperatureLower, cubbiesData[i].temperatureUpper,
                    (millis() - cubbiesData[i].lastUpdated) / 1000);
    } else {
      Serial.printf("  Cubby %d: No data\n", i + 1);
    }
  }

  Serial.print("Current IP: ");
  Serial.println(WiFi.localIP());
}

// ========== Main Setup and Loop ==========
void setup() {
  Serial.begin(115200);

  // Try to connect with stored credentials first
  if (tryStoredWiFiConnection()) {
    // If successful, go directly to monitoring mode
    currentState = MONITORING_MODE;
    initializeSensors();
    setupMonitoringServer();
    Serial.println("✅ Started in monitoring mode!");
  } else {
    // If no stored credentials or connection failed, start setup mode
    setupWiFiAccessPoint();
  }
}

void loop() {
  server.handleClient();

  // Only read sensors in monitoring mode
  if (currentState == MONITORING_MODE) {
    unsigned long currentTime = millis();
    if (currentTime - lastReadTime >= readInterval) {
      lastReadTime = currentTime;

      // Read all sensors
      readSoilMoisture();
      readEnvironmentalSensors();

      // Print to serial for debugging
      printSensorData();
    }
  }
}

// // necessary libraries
// #include <WiFi.h>                                           // for wifi connectivity
// #include <WebServer.h>                                      // for web server creation
// #include <ArduinoJson.h>                                    // for JSON operations
// #include <Wire.h>                                           // for the i2c comm
// #include "Adafruit_VEML7700.h"                              // for the light sensor
// #include <Preferences.h>                                    // for storing/retrieving data from flash

// // System State management
// // define the system states for wifi
// enum SystemState {
//   WIFI_SETUP_MODE,                                          // device awaiting wifi config
//   MONITORING_MODE                                           // wifi is connected and can now monitor
// };

// SystemState currentState = WIFI_SETUP_MODE;                 // the default boot mode is wifi setup
// Preferences preferences;                                    // for persistent storage

// WebServer server(80);                                       // creates web server on port 80

// // Soil Moisture Configuration
// #define AOUT_PIN 35                                         //
// #define AOUT_PIN2 34
// #define AOUT_PIN3 32

// // Last readings storage
// int lastMoistureValue1 = 0;
// int lastMoistureValue2 = 0;
// int lastMoistureValue3 = 0;
// String lastMoistureStatus1 = "unknown";
// String lastMoistureStatus2 = "unknown";
// String lastMoistureStatus3 = "unknown";

// // ========== Environmental Sensor Configuration ==========
// #define PCA9547D_ADDR 0x70
// #define SHT4X_ADDR 0x44

// // SHT4x sensor configuration
// #define SHT4X_CHANNEL_START 0
// #define SHT4X_CHANNEL_END 2
// #define SHT4X_COUNT 3

// // VEML7700 sensor configuration
// #define VEML7700_CHANNEL_START 5
// #define VEML7700_CHANNEL_END 7
// #define VEML7700_COUNT 3

// // SHT4x commands
// #define SHT4X_CMD_MEASURE_HIGH_PRECISION 0xFD
// #define SHT4X_CMD_SOFT_RESET 0x94

// // Sensor objects
// Adafruit_VEML7700 veml_sensors[VEML7700_COUNT];

// // Last environmental readings
// float lastTemp[3] = {0};
// String lastTempStatus[3] = {""};
// float lastHumidity[3] = {0};
// String lastHumidityStatus[3] = {""};
// float lastLux[3] = {0};
// String lastLightStatus[3] = {""};

// // ========== Timing Configuration ==========
// unsigned long lastReadTime = 0;
// const unsigned long readInterval = 10000; // 10 seconds between readings

// // ========== WiFi Setup Functions ==========
// void handleWifiConfig() {
//   if (server.method() == HTTP_POST) {
//     StaticJsonDocument<200> doc;
//     DeserializationError error = deserializeJson(doc, server.arg("plain"));

//     if (error) {
//       server.send(400, "text/plain", "Invalid JSON");
//       Serial.println("\n✗✗✗✗✗ JSON parse error");
//       return;
//     }

//     const char* ssid = doc["ssid"];
//     const char* password = doc["password"];

//     Serial.println("ᯤ ᯤ ᯤ Attempting to connect with:");
//     Serial.print("SSID: ");
//     Serial.println(ssid);
//     Serial.print("Password: ");
//     Serial.println(password);

//     WiFi.begin(ssid, password);

//     int attempts = 0;
//     while (WiFi.status() != WL_CONNECTED && attempts++ < 100) {
//       delay(500);
//       Serial.print(".");
//     }

//     if (WiFi.status() == WL_CONNECTED) {
//       Serial.println("\n✔✔✔✔ Connected to Wi-Fi!");
//       Serial.print("IP address: ");
//       Serial.println(WiFi.localIP());

//       // Save WiFi credentials
//       preferences.begin("wifi", false);
//       preferences.putString("ssid", ssid);
//       preferences.putString("password", password);
//       preferences.end();

//       // Create JSON response with IP address
//       StaticJsonDocument<200> response;
//       response["status"] = "success";
//       response["message"] = "Connected successfully! Switching to monitoring mode...";
//       response["ip"] = WiFi.localIP().toString();

//       String responseString;
//       serializeJson(response, responseString);

//       server.send(200, "application/json", responseString);

//       // Wait a moment for response to send, then switch modes
//       delay(2000);
//       switchToMonitoringMode();

//     } else {
//       Serial.println("\n!!!!!!! Failed to connect to Wi-Fi.");

//       // Create JSON error response
//       StaticJsonDocument<200> response;
//       response["status"] = "error";
//       response["message"] = "Failed to connect to Wi-Fi";

//       String responseString;
//       serializeJson(response, responseString);

//       server.send(500, "application/json", responseString);
//     }
//   } else {
//     server.send(405, "text/plain", "Use POST");
//   }
// }

// void handleSetupRoot() {
//   String html = "<!DOCTYPE html><html><head><title>Plant Monitor Setup</title>";
//   html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
//   html += "<style>body{font-family:Arial;margin:20px;background:#f0f0f0;}";
//   html += ".container{max-width:400px;margin:0 auto;background:white;padding:20px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1);}";
//   html += "h1{color:#2c3e50;text-align:center;}";
//   html += "input{width:100%;padding:10px;margin:10px 0;border:1px solid #ddd;border-radius:5px;box-sizing:border-box;}";
//   html += "button{width:100%;padding:12px;background:#3498db;color:white;border:none;border-radius:5px;cursor:pointer;font-size:16px;}";
//   html += "button:hover{background:#2980b9;}";
//   html += ".status{margin-top:10px;padding:10px;border-radius:5px;display:none;}";
//   html += ".success{background:#d4edda;color:#155724;border:1px solid #c3e6cb;}";
//   html += ".error{background:#f8d7da;color:#721c24;border:1px solid #f5c6cb;}";
//   html += "</style></head><body>";
//   html += "<div class='container'>";
//   html += "<h1>🌱 Plant Monitor Setup</h1>";
//   html += "<p>Connect your plant monitoring system to WiFi:</p>";
//   html += "<form id='wifiForm'>";
//   html += "<input type='text' id='ssid' placeholder='WiFi Network Name (SSID)' required>";
//   html += "<input type='password' id='password' placeholder='WiFi Password' required>";
//   html += "<button type='submit'>Connect to WiFi</button>";
//   html += "</form>";
//   html += "<div id='status' class='status'></div>";
//   html += "</div>";
//   html += "<script>";
//   html += "document.getElementById('wifiForm').addEventListener('submit', function(e) {";
//   html += "e.preventDefault();";
//   html += "const ssid = document.getElementById('ssid').value;";
//   html += "const password = document.getElementById('password').value;";
//   html += "const statusDiv = document.getElementById('status');";
//   html += "statusDiv.style.display = 'block';";
//   html += "statusDiv.className = 'status';";
//   html += "statusDiv.innerHTML = 'Connecting...';";
//   html += "fetch('/wifi', {";
//   html += "method: 'POST',";
//   html += "headers: {'Content-Type': 'application/json'},";
//   html += "body: JSON.stringify({ssid: ssid, password: password})";
//   html += "}).then(response => response.json()).then(data => {";
//   html += "if (data.status === 'success') {";
//   html += "statusDiv.className = 'status success';";
//   html += "statusDiv.innerHTML = 'Connected! IP: ' + data.ip + '<br>Switching to monitoring mode...';";
//   html += "} else {";
//   html += "statusDiv.className = 'status error';";
//   html += "statusDiv.innerHTML = 'Error: ' + data.message;";
//   html += "}";
//   html += "}).catch(error => {";
//   html += "statusDiv.className = 'status error';";
//   html += "statusDiv.innerHTML = 'Connection failed. Please try again.';";
//   html += "});";
//   html += "});";
//   html += "</script></body></html>";

//   server.send(200, "text/html", html);
// }

// void setupWiFiAccessPoint() {
//   Serial.println("🔧 Starting WiFi Setup Mode...");

//   WiFi.mode(WIFI_AP);
//   WiFi.softAP("FLOWERS-SETUP", "12345678");

//   Serial.print("🚀 AP started at IP: ");
//   Serial.println(WiFi.softAPIP());

//   // Setup routes for configuration mode
//   server.on("/", HTTP_GET, handleSetupRoot);
//   server.on("/wifi", HTTP_POST, handleWifiConfig);

//   server.begin();
//   Serial.println("Setup server started");
// }

// // ========== Mode Switching ==========
// void switchToMonitoringMode() {
//   Serial.println("🔄 Switching to monitoring mode...");

//   // Stop the server
//   server.stop();

//   // Stop AP mode
//   WiFi.softAPdisconnect(true);

//   // Change system state
//   currentState = MONITORING_MODE;

//   // Initialize sensors
//   initializeSensors();

//   // Setup monitoring server routes
//   setupMonitoringServer();

//   Serial.println("✅ Monitoring mode activated!");
//   Serial.print("Monitor at: http://");
//   Serial.println(WiFi.localIP());
// }

// bool tryStoredWiFiConnection() {
//   preferences.begin("wifi", true);
//   String ssid = preferences.getString("ssid", "");
//   String password = preferences.getString("password", "");
//   preferences.end();

//   if (ssid.length() > 0) {
//     Serial.println("📡 Attempting to connect with stored credentials...");
//     Serial.print("SSID: ");
//     Serial.println(ssid);

//     WiFi.begin(ssid.c_str(), password.c_str());

//     int attempts = 0;
//     while (WiFi.status() != WL_CONNECTED && attempts++ < 60) {
//       delay(500);
//       Serial.print(".");
//     }

//     if (WiFi.status() == WL_CONNECTED) {
//       Serial.println("\n✅ Connected to stored WiFi!");
//       Serial.print("IP address: ");
//       Serial.println(WiFi.localIP());
//       return true;
//     } else {
//       Serial.println("\n❌ Failed to connect with stored credentials.");
//     }
//   }

//   return false;
// }

// // ========== Sensor Initialization ==========
// void initializeSensors() {
//   // Initialize I2C
//   Wire.begin(13, 14); // SDA=GPIO13, SCL=GPIO14
//   Wire.setClock(100000);

//   // Test multiplexer
//   testPCA9547D();

//   // Initialize environmental sensors
//   initEnvironmentalSensors();
// }

// void initEnvironmentalSensors() {
//   // Initialize SHT4x sensors
//   for (int channel = SHT4X_CHANNEL_START; channel <= SHT4X_CHANNEL_END; channel++) {
//     initSHT4x(channel);
//   }

//   // Initialize VEML7700 sensors
//   for (int channel = VEML7700_CHANNEL_START; channel <= VEML7700_CHANNEL_END; channel++) {
//     int sensor_index = channel - VEML7700_CHANNEL_START;
//     initVEML7700(channel, sensor_index);
//   }
// }

// bool initSHT4x(int channel) {
//   if (!selectChannel(channel)) return false;

//   Wire.beginTransmission(SHT4X_ADDR);
//   Wire.write(SHT4X_CMD_SOFT_RESET);
//   return Wire.endTransmission() == 0;
// }

// bool initVEML7700(int channel, int sensor_index) {
//   if (!selectChannel(channel)) return false;

//   if (veml_sensors[sensor_index].begin()) {
//     veml_sensors[sensor_index].setGain(VEML7700_GAIN_1);
//     veml_sensors[sensor_index].setIntegrationTime(VEML7700_IT_100MS);
//     return true;
//   }
//   return false;
// }

// bool selectChannel(int channel) {
//   if (channel < 0 || channel > 7) return false;

//   Wire.beginTransmission(PCA9547D_ADDR);
//   Wire.write(0x08 | channel);
//   return Wire.endTransmission() == 0;
// }

// void testPCA9547D() {
//   Wire.beginTransmission(PCA9547D_ADDR);
//   if (Wire.endTransmission() == 0) {
//     Serial.println("PCA9547D multiplexer detected");
//   } else {
//     Serial.println("PCA9547D multiplexer not found!");
//   }
// }

// // ========== Monitoring Server Setup ==========
// void setupMonitoringServer() {
//   server.on("/", HTTP_GET, handleRoot);
//   server.on("/sensors", HTTP_GET, handleData);
//   server.on("/reset", HTTP_GET, handleReset);
//   server.begin();
//   Serial.println("HTTP monitoring server started");
// }

// void handleReset() {
//   Serial.println("🔄 Resetting WiFi settings...");

//   // Clear stored credentials
//   preferences.begin("wifi", false);
//   preferences.clear();
//   preferences.end();

//   server.send(200, "text/html", "<h2>WiFi settings cleared. Restarting...</h2>");
//   delay(2000);
//   ESP.restart();
// }

// // ========== Soil Moisture Functions ==========
// void readSoilMoisture() {
//   lastMoistureValue1 = analogRead(AOUT_PIN);
//   lastMoistureValue2 = analogRead(AOUT_PIN2);
//   lastMoistureValue3 = analogRead(AOUT_PIN3);

//   lastMoistureStatus1 = lastMoistureValue1 > 2200 ? "dry" : "wet";
//   lastMoistureStatus2 = lastMoistureValue2 > 2200 ? "dry" : "wet";
//   lastMoistureStatus3 = lastMoistureValue3 > 2200 ? "dry" : "wet";
// }

// // ========== Environmental Sensor Functions ==========
// void readEnvironmentalSensors() {
//   // Read temperature/humidity sensors
//   for (int channel = SHT4X_CHANNEL_START; channel <= SHT4X_CHANNEL_END; channel++) {
//     if (selectChannel(channel)) {
//       readSHT4xData(channel);
//     }
//   }

//   // Read light sensors
//   for (int channel = VEML7700_CHANNEL_START; channel <= VEML7700_CHANNEL_END; channel++) {
//     int sensor_index = channel - VEML7700_CHANNEL_START;
//     if (selectChannel(channel)) {
//       lastLux[sensor_index] = veml_sensors[sensor_index].readLux();

//       String lightStatus;
//       lightStatus = lastLux[sensor_index] > 150 ? "dim" : "dark";
//       lightStatus = lastLux[sensor_index] > 500 ? "bright" : lightStatus;
//       lastLightStatus[sensor_index] = lightStatus;
//     }
//   }
// }

// void readSHT4xData(int channel) {
//   if (!selectChannel(channel)) return;

//   Wire.beginTransmission(SHT4X_ADDR);
//   Wire.write(SHT4X_CMD_MEASURE_HIGH_PRECISION);
//   if (Wire.endTransmission() != 0) return;

//   delay(10);

//   Wire.requestFrom(SHT4X_ADDR, 6);
//   if (Wire.available() < 6) return;

//   byte data[6];
//   for (int i = 0; i < 6; i++) data[i] = Wire.read();

//   uint16_t temp_raw = (data[0] << 8) | data[1];
//   uint16_t hum_raw = (data[3] << 8) | data[4];

//   lastTemp[channel] = -45.0 + 175.0 * temp_raw / 65535.0;
//   String tempStatus;
//   tempStatus = lastTemp[channel] > 23 ? "warm" : "cold";
//   tempStatus = lastTemp[channel] > 27 ? "hot" : tempStatus;
//   lastTempStatus[channel] = tempStatus;

//   lastHumidity[channel] = max(0.0, min(100.0, -6.0 + 125.0 * hum_raw / 65535.0));
//   String humidityStatus;
//   humidityStatus = lastHumidity[channel] > 40 ? "normal" : "dry";
//   humidityStatus = lastHumidity[channel] > 65 ? "humid" : humidityStatus;
//   lastHumidityStatus[channel] = humidityStatus;
// }

// // ========== Monitoring Web Server Functions ==========
// void handleRoot() {
//   String html = "<html><head><meta http-equiv='refresh' content='5'>";
//   html += "<style>body{font-family:Arial;margin:20px;}";
//   html += ".sensor{background:#f0f0f0;padding:15px;margin-bottom:10px;border-radius:5px;}";
//   html += ".reset-btn{background:#e74c3c;color:white;padding:10px 20px;text-decoration:none;border-radius:5px;display:inline-block;margin-top:20px;}";
//   html += "</style></head><body>";
//   html += "<h1>🌱 Plant Monitoring System</h1>";

//   // Soil moisture
//   html += "<div class='sensor'><h2>Soil Moisture</h2>";
//   html += "<p>Sensor 1: " + String(lastMoistureValue1) + " (" + lastMoistureStatus1 + ")</p>";
//   html += "<p>Sensor 2: " + String(lastMoistureValue2) + " (" + lastMoistureStatus2 + ")</p>";
//   html += "<p>Sensor 3: " + String(lastMoistureValue3) + " (" + lastMoistureStatus3 + ")</p></div>";

//   // Environmental data
//   html += "<div class='sensor'><h2>Environmental Sensors</h2>";
//   for (int i = 0; i < 3; i++) {
//     html += "<h3>Sensor Group " + String(i+1) + "</h3>";
//     html += "<p>Temperature: " + String(lastTemp[i], 1) + "°C (" + lastTempStatus[i] + ")</p>";
//     html += "<p>Humidity: " + String(lastHumidity[i], 1) + "% (" + lastHumidityStatus[i] + ")</p>";
//     html += "<p>Light: " + String(lastLux[i], 1) + " lux (" + lastLightStatus[i] + ")</p>";
//   }
//   html += "</div>";
//   html += "<p>Auto-refresh every 5 seconds</p>";
//   html += "<a href='/reset' class='reset-btn'>Reset WiFi Settings</a>";
//   html += "</body></html>";

//   server.send(200, "text/html", html);
// }

// void handleData() {
//   StaticJsonDocument<512> doc;

//   // Soil moisture data
//   doc["soil"]["sensor1"]["value"] = lastMoistureValue1;
//   doc["soil"]["sensor1"]["status"] = lastMoistureStatus1;
//   doc["soil"]["sensor2"]["value"] = lastMoistureValue2;
//   doc["soil"]["sensor2"]["status"] = lastMoistureStatus2;
//   doc["soil"]["sensor3"]["value"] = lastMoistureValue3;
//   doc["soil"]["sensor3"]["status"] = lastMoistureStatus3;

//   // Environmental data
//   for (int i = 0; i < 3; i++) {
//     doc["environment"]["temperature" + String(i+1)]["value"] = lastTemp[i];
//     doc["environment"]["temperature" + String(i+1)]["status"] = lastTempStatus[i];

//     doc["environment"]["humidity" + String(i+1)]["value"] = lastHumidity[i];
//     doc["environment"]["humidity" + String(i+1)]["status"] = lastHumidityStatus[i];

//     doc["environment"]["light" + String(i+1)]["value"] = lastLux[i];
//     doc["environment"]["light" + String(i+1)]["status"] = lastLightStatus[i];
//   }

//   String json;
//   server.sendHeader("Access-Control-Allow-Origin", "*");
//   serializeJson(doc, json);
//   server.send(200, "application/json", json);
// }

// // ========== Debug Functions ==========
// void printSensorData() {
//   Serial.println("\n=== Sensor Readings ===");

//   // Soil moisture
//   Serial.println("Soil Moisture:");
//   Serial.printf("  Sensor 1: %d (%s)\n", lastMoistureValue1, lastMoistureStatus1.c_str());
//   Serial.printf("  Sensor 2: %d (%s)\n", lastMoistureValue2, lastMoistureStatus2.c_str());
//   Serial.printf("  Sensor 3: %d (%s)\n", lastMoistureValue3, lastMoistureStatus3.c_str());

//   // Environmental
//   Serial.println("Environmental Sensors:");
//   for (int i = 0; i < 3; i++) {
//     Serial.printf("  Group %d: %.1f°C (%s), %.1f%% (%s), %.1f lux (%s)\n",
//                  i+1, lastTemp[i], lastTempStatus[i].c_str(),
//                  lastHumidity[i], lastHumidityStatus[i].c_str(),
//                  lastLux[i], lastLightStatus[i].c_str());
//   }

//   Serial.print("Current IP: ");
//   Serial.println(WiFi.localIP());
// }

// // ========== Main Setup and Loop ==========
// void setup() {
//   Serial.begin(115200);

//   // Try to connect with stored credentials first
//   if (tryStoredWiFiConnection()) {
//     // If successful, go directly to monitoring mode
//     currentState = MONITORING_MODE;
//     initializeSensors();
//     setupMonitoringServer();
//     Serial.println("✅ Started in monitoring mode!");
//   } else {
//     // If no stored credentials or connection failed, start setup mode
//     setupWiFiAccessPoint();
//   }
// }

// void loop() {
//   server.handleClient();

//   // Only read sensors in monitoring mode
//   if (currentState == MONITORING_MODE) {
//     unsigned long currentTime = millis();
//     if (currentTime - lastReadTime >= readInterval) {
//       lastReadTime = currentTime;

//       // Read all sensors
//       readSoilMoisture();
//       readEnvironmentalSensors();

//       // Print to serial for debugging
//       printSensorData();
//     }
//   }
// }