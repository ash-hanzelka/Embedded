// http://192.168.1.142/ --> millers304 wifi
//  192.168.1.182 --> XO wifi
//  192.168.1.167 --> Richie wifi

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "Adafruit_VEML7700.h"

// ========== WiFi Configuration ==========
const char* ssid = "SpectrumSetup-7062";
const char* password = "righttable791";

WebServer server(80);

// ========== Soil Moisture Configuration ==========
#define AOUT_PIN 35
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
float lastTemp[3] = {0};
String lastTempStatus[3] = {""};
float lastHumidity[3] = {0};
String lastHumidityStatus[3] = {""};
float lastLux[3] = {0};
String lastLightStatus[3] = {""};

// ========== Timing Configuration ==========
unsigned long lastReadTime = 0;
const unsigned long readInterval = 10000; // 2 seconds between readings

void setup() {
  Serial.begin(115200);
  
  // Initialize WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // Initialize I2C
  Wire.begin(13, 14); // SDA=GPIO13, SCL=GPIO14
  Wire.setClock(100000);

  // Test multiplexer
  testPCA9547D();

  // Initialize environmental sensors
  initEnvironmentalSensors();

  // Setup web server
  server.on("/", HTTP_GET, handleRoot);
  server.on("/sensors", HTTP_GET, handleData);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  
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

// ========== Web Server Functions ==========
void handleRoot() {
  String html = "<html><head><meta http-equiv='refresh' content='5'>";
  html += "<style>body{font-family:Arial;margin:20px;}";
  html += ".sensor{background:#f0f0f0;padding:15px;margin-bottom:10px;border-radius:5px;}";
  html += "</style></head><body>";
  html += "<h1>Plant Monitoring System</h1>";
  
  // Soil moisture
  html += "<div class='sensor'><h2>Soil Moisture</h2>";
  html += "<p>Sensor 1: " + String(lastMoistureValue1) + " (" + lastMoistureStatus1 + ")</p>";
  html += "<p>Sensor 2: " + String(lastMoistureValue2) + " (" + lastMoistureStatus2 + ")</p>";
  html += "<p>Sensor 3: " + String(lastMoistureValue3) + " (" + lastMoistureStatus3 + ")</p></div>";
  
  // Environmental data
  html += "<div class='sensor'><h2>Environmental Sensors</h2>";
  for (int i = 0; i < 3; i++) {
    html += "<h3>Sensor Group " + String(i+1) + "</h3>";
    html += "<p>Temperature: " + String(lastTemp[i], 1) + "°C</p>";
    html += "<p>Humidity: " + String(lastHumidity[i], 1) + "%</p>";
    html += "<p>Light: " + String(lastLux[i], 1) + " lux</p>";
  }
  html += "</div>";
  html += "<p>Auto-refresh every 5 seconds</p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleData() {
  StaticJsonDocument<512> doc;
  
  // Soil moisture data
  doc["soil"]["sensor1"]["value"] = lastMoistureValue1;
  doc["soil"]["sensor1"]["status"] = lastMoistureStatus1;
  doc["soil"]["sensor2"]["value"] = lastMoistureValue2;
  doc["soil"]["sensor2"]["status"] = lastMoistureStatus2;
  doc["soil"]["sensor3"]["value"] = lastMoistureValue3;
  doc["soil"]["sensor3"]["status"] = lastMoistureStatus3;
  
  // Environmental data
  for (int i = 0; i < 3; i++) {
    doc["environment"]["temperature" + String(i+1)]["value"] = lastTemp[i];
    doc["environment"]["temperature" + String(i+1)]["status"] = lastTempStatus[i];

    doc["environment"]["humidity" + String(i+1)]["value"] = lastHumidity[i];
    doc["environment"]["humidity" + String(i+1)]["status"] = lastHumidityStatus[i];

    doc["environment"]["light" + String(i+1)]["value"] = lastLux[i];
    doc["environment"]["light" + String(i+1)]["status"] = lastLightStatus[i];
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
    Serial.printf("  Group %d: %.1f°C, %.1f%%, %.1f lux\n", 
                 i+1, lastTemp[i], lastHumidity[i], lastLux[i]);
    Serial.println("        " + lastTempStatus[i] + " " + lastHumidityStatus[i] + " " + lastLightStatus[i]);
  }

  Serial.println(WiFi.localIP());
}