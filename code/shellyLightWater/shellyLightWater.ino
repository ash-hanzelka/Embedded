#include <Wire.h>
#include "Adafruit_VEML7700.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// ==== WiFi Config ====
const char* ssid = "sUpBr0 (2)";
const char* password = "JustinBieber";

// ==== Shelly Device IPs ====
const char* shellyLightIP = "172.20.10.3";  // Light control device
const char* shellyWaterIP = "172.20.10.2";   // Watering control device

// ==== PCA9547D Multiplexer for Light Sensors ====
#define SDA_PIN 13
#define SCL_PIN 14
#define PCA9547D_ADDR 0x70

// ==== VEML7700 Light Sensors ====
#define VEML7700_CHANNEL_START 5
#define VEML7700_COUNT 3
Adafruit_VEML7700 veml_sensors[VEML7700_COUNT];

// ==== Moisture Sensor ADC pins ====
#define AOUT_PIN 35   // Sensor 1
#define AOUT_PIN2 34  // Sensor 2
#define AOUT_PIN3 32  // Sensor 3

// ==== Shelly Light Control Channels (Device: 172.20.10.14) ====
const int LIGHT_CHANNELS[3] = {0, 2, 3};  // Sensor 1->B(2), Sensor 2->R(0), Sensor 3->W(3)

// ==== Shelly Water Control Channels (Device: 172.20.10.6) ====
const int W_CHANNEL = 3;  // W channel (white) - controlled by Pin 32
const int G_CHANNEL = 1;  // G channel (green) - controlled by Pin 34  
const int B_CHANNEL = 2;  // B channel (blue) - controlled by Pin 35

// ==== Light Control - Lux Thresholds ====
const float DARK_THRESHOLD = 230.0;
const float DIM_THRESHOLD = 370.0;

// ==== Moisture Control - Soil Thresholds ====
const int WET_THRESHOLD = 1500;  // Wet soil
const int DRY_THRESHOLD = 1800;  // Dry soil

// ==== Light Cycling States ====
enum CycleState { CYCLE_DARK, CYCLE_DIM, CYCLE_BRIGHT };
enum PhaseState { PHASE_ADJUSTING, PHASE_HOLDING };

struct ChamberState {
  CycleState currentCycle;
  PhaseState currentPhase;
  int targetBrightness;
  int currentBrightness;
  unsigned long phaseStartTime;
  unsigned long lastAdjustTime;
  bool cycleComplete;
};

ChamberState chambers[3];

// ==== Light Control Timing Constants ====
const unsigned long HOLD_DURATION = 30000;        // 30 seconds hold time
const unsigned long BRIGHTNESS_ADJUST_INTERVAL = 500;  // 500ms between brightness adjustments
const unsigned long BRIGHTNESS_STEP = 5;          // Brightness increment/decrement step

// ==== Moisture Control Variables ====
int lastMoistureValue1 = 0;
int lastMoistureValue2 = 0;
int lastMoistureValue3 = 0;
unsigned long lastMoistureReadTime = 0;
const unsigned long MOISTURE_INTERVAL = 1000; // 1 second
const int NUM_SAMPLES = 50;

// ==== Watering Control - Per Channel Cooldown ====
unsigned long lastWateringTime_B = 0;
unsigned long lastWateringTime_G = 0;
unsigned long lastWateringTime_W = 0;
const unsigned long MIN_WATERING_INTERVAL = 30000; // 30 seconds between waterings per channel

// ==== Global Light Cycle Control ====
bool allChambersComplete = false;
unsigned long cycleStartTime = 0;

// ==== Web Server ====
WebServer server(80);

// ------------------ SETUP ------------------
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting Combined ESP32 Light & Moisture Control System...");

  // Init WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // Init I2C for light sensors
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);
  Serial.printf("I2C initialized (SDA=%d, SCL=%d)\n", SDA_PIN, SCL_PIN);

  // Init VEML light sensors
  for (int i = 0; i < VEML7700_COUNT; i++) {
    int channel = VEML7700_CHANNEL_START + i;
    if (!initVEML7700(channel, i)) {
      Serial.printf("✗ Failed to init VEML #%d on channel %d\n", i + 1, channel);
    } else {
      Serial.printf("✓ VEML #%d ready on channel %d\n", i + 1, channel);
    }
  }

  // Initialize light chamber states
  initializeChamberStates();
  cycleStartTime = millis();
  Serial.println("Starting light cycling sequence...");

  // Setup web server endpoint for moisture data
  server.on("/moisture", HTTP_GET, []() {
    StaticJsonDocument<200> doc;
    doc["value1"] = lastMoistureValue1;
    doc["value2"] = lastMoistureValue2;
    doc["value3"] = lastMoistureValue3;

    String json;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    serializeJson(doc, json);
    server.send(200, "application/json", json);
  });

  server.begin();
  Serial.println("Web server started");
  Serial.println("=== System Ready ===");
}

// ------------------ MAIN LOOP ------------------
void loop() {
  server.handleClient();

  // Process Light Control System
  processLightControl();
  
  // Process Moisture Control System
  processMoistureControl();
  
  delay(200);
}

// ------------------ LIGHT CONTROL SYSTEM ------------------
void processLightControl() {
  // Check if all chambers completed their cycles
  if (allChambersComplete) {
    Serial.println("All light chambers completed their cycles. Restarting...");
    initializeChamberStates();
    cycleStartTime = millis();
    allChambersComplete = false;
    delay(5000); // 5 second pause between full cycles
    return;
  }

  // Process each light chamber
  bool allComplete = true;
  for (int i = 0; i < VEML7700_COUNT; i++) {
    float lux = readVEMLlux(VEML7700_CHANNEL_START + i, i);
    
    // Only process if chamber hasn't completed all cycles
    if (!chambers[i].cycleComplete) {
      processLightChamber(i, lux);
      allComplete = false;
    }
    
    // Print status
    printLightChamberStatus(i, lux);
    delay(50);
  }
  
  allChambersComplete = allComplete;
}

void processLightChamber(int idx, float lux) {
  ChamberState& chamber = chambers[idx];
  unsigned long currentTime = millis();

  switch (chamber.currentPhase) {
    case PHASE_ADJUSTING:
      // Check if we've reached the target condition
      if (isInTargetRange(lux, chamber.currentCycle)) {
        chamber.currentPhase = PHASE_HOLDING;
        chamber.phaseStartTime = currentTime;
        Serial.printf("Light Chamber %d: Reached target %s condition (%.2f lux), starting hold phase\n", 
                     idx + 1, getCycleString(chamber.currentCycle), lux);
      } else {
        // Adjust brightness if enough time has passed
        if (currentTime - chamber.lastAdjustTime >= BRIGHTNESS_ADJUST_INTERVAL) {
          adjustLightBrightness(idx, lux);
          chamber.lastAdjustTime = currentTime;
        }
      }
      break;

    case PHASE_HOLDING:
      // Hold for the specified duration
      if (currentTime - chamber.phaseStartTime >= HOLD_DURATION) {
        // Move to next cycle
        advanceToNextCycle(idx);
      }
      // Check if we're still in target range, adjust if needed
      else if (!isInTargetRange(lux, chamber.currentCycle)) {
        chamber.currentPhase = PHASE_ADJUSTING;
        chamber.lastAdjustTime = currentTime;
        Serial.printf("Light Chamber %d: Lost target condition, resuming adjustment\n", idx + 1);
      }
      break;
  }
}

void adjustLightBrightness(int idx, float lux) {
  ChamberState& chamber = chambers[idx];
  int newBrightness = chamber.currentBrightness;

  switch (chamber.currentCycle) {
    case CYCLE_DARK:
      // For dark condition, we want to reduce brightness
      if (lux >= DARK_THRESHOLD) {
        newBrightness = max(0, chamber.currentBrightness - (int)BRIGHTNESS_STEP);
      }
      break;

    case CYCLE_DIM:
      // For dim condition, adjust to stay in range
      if (lux < DARK_THRESHOLD) {
        newBrightness = min(100, chamber.currentBrightness + (int)BRIGHTNESS_STEP);
      } else if (lux > DIM_THRESHOLD) {
        newBrightness = max(0, chamber.currentBrightness - (int)BRIGHTNESS_STEP);
      }
      break;

    case CYCLE_BRIGHT:
      // For bright condition, increase brightness
      if (lux <= DIM_THRESHOLD) {
        newBrightness = min(100, chamber.currentBrightness + (int)BRIGHTNESS_STEP);
      }
      break;
  }

  if (newBrightness != chamber.currentBrightness) {
    setLightBrightness(idx, newBrightness);
    chamber.currentBrightness = newBrightness;
  }
}

void setLightBrightness(int idx, int brightness) {
  int channel = LIGHT_CHANNELS[idx];
  Serial.printf("Setting Light Chamber %d (Channel %d) -> %d%% brightness\n", idx + 1, channel, brightness);

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping light brightness update");
    return;
  }

  HTTPClient http;
  String url = String("http://") + shellyLightIP + "/rpc/Light.Set";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  String payload;
  if (brightness == 0)
    payload = String("{\"id\":") + channel + ",\"on\":false}";
  else
    payload = String("{\"id\":") + channel + ",\"on\":true,\"brightness\":" + brightness + "}";

  int code = http.POST(payload);
  if (code > 0) {
    Serial.printf("Light Shelly response: HTTP %d\n", code);
  } else {
    Serial.printf("Light Shelly request failed: %s\n", http.errorToString(code).c_str());
  }
  http.end();
}

// ------------------ MOISTURE CONTROL SYSTEM ------------------
void processMoistureControl() {
  unsigned long now = millis();
  if (now - lastMoistureReadTime >= MOISTURE_INTERVAL) {
    lastMoistureReadTime = now;

    // Read sensors
    lastMoistureValue1 = analogRead(AOUT_PIN);  // Pin 35 -> controls B channel
    lastMoistureValue2 = analogRead(AOUT_PIN2); // Pin 34 -> controls G channel
    lastMoistureValue3 = readMoistureAverage(AOUT_PIN3); // Pin 32 -> controls W channel

    Serial.println("=== Moisture Readings ===");
    Serial.printf("Moisture Sensor 1 (Pin 35 -> B channel): %d\n", lastMoistureValue1);
    Serial.printf("Moisture Sensor 2 (Pin 34 -> G channel): %d\n", lastMoistureValue2);
    Serial.printf("Moisture Sensor 3 (Pin 32 -> W channel): %d\n", lastMoistureValue3);

    // Control Shelly watering channels based on respective sensors with cooldown protection
    
    // Pin 35 (Sensor 1) controls B channel
    if (lastMoistureValue1 >= DRY_THRESHOLD) {
      Serial.println("Moisture Sensor 1: Soil is dry, checking B channel cooldown...");
      shellyControlWateringChannelWithCooldown(B_CHANNEL, true, &lastWateringTime_B);
      if ((millis() - lastWateringTime_B) < 1000) { // Just watered
        delay(10000); // 10 second watering
        Serial.println("10 seconds elapsed, turning watering B channel OFF.");
        shellyControlWateringChannel(B_CHANNEL, false);
      }
    } else {
      Serial.println("Moisture Sensor 1: Soil is wet, ensuring watering B channel is OFF.");
      shellyControlWateringChannel(B_CHANNEL, false);
    }

    // Pin 34 (Sensor 2) controls G channel
    if (lastMoistureValue2 >= DRY_THRESHOLD) {
      Serial.println("Moisture Sensor 2: Soil is dry, checking G channel cooldown...");
      shellyControlWateringChannelWithCooldown(G_CHANNEL, true, &lastWateringTime_G);
      if ((millis() - lastWateringTime_G) < 1000) { // Just watered
        delay(10000); // 10 second watering
        Serial.println("10 seconds elapsed, turning watering G channel OFF.");
        shellyControlWateringChannel(G_CHANNEL, false);
      }
    } else {
      Serial.println("Moisture Sensor 2: Soil is wet, ensuring watering G channel is OFF.");
      shellyControlWateringChannel(G_CHANNEL, false);
    }

    // Pin 32 (Sensor 3) controls W channel  
    if (lastMoistureValue3 >= DRY_THRESHOLD) {
      Serial.println("Moisture Sensor 3: Soil is dry, checking W channel cooldown...");
      shellyControlWateringChannelWithCooldown(W_CHANNEL, true, &lastWateringTime_W);
      if ((millis() - lastWateringTime_W) < 1000) { // Just watered
        delay(10000); // 10 second watering
        Serial.println("10 seconds elapsed, turning watering W channel OFF.");
        shellyControlWateringChannel(W_CHANNEL, false);
      }
    } else {
      Serial.println("Moisture Sensor 3: Soil is wet, ensuring watering W channel is OFF.");
      shellyControlWateringChannel(W_CHANNEL, false);
    }
  }
}

// === Watering Shelly Control with Cooldown Protection ===
void shellyControlWateringChannelWithCooldown(int channel, bool turnOn, unsigned long* lastWateringTime) {
  if (!turnOn) {
    // Always allow turning OFF
    shellyControlWateringChannel(channel, false);
    return;
  }

  // Check cooldown period for turning ON
  unsigned long now = millis();
  bool enoughTimePassedSinceLastWatering = (now - *lastWateringTime) >= MIN_WATERING_INTERVAL;
  
  if (enoughTimePassedSinceLastWatering) {
    shellyControlWateringChannel(channel, true);
    *lastWateringTime = now;
  } else {
    unsigned long remainingCooldown = (MIN_WATERING_INTERVAL - (now - *lastWateringTime)) / 1000;
    Serial.printf("Watering Channel %d: Soil dry but waiting %lu more seconds before next watering\n", 
                 channel, remainingCooldown);
  }
}

void shellyControlWateringChannel(int channel, bool turnOn) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, cannot control watering Shelly.");
    return;
  }

  const int MAX_RETRIES = 3;
  int attempt = 0;
  bool success = false;

  while (attempt < MAX_RETRIES && !success) {
    attempt++;
    
    HTTPClient http;
    String url = String("http://") + shellyWaterIP + "/rpc/Light.Set";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(5000); // 5 second timeout

    String payload;
    if (turnOn)
      payload = String("{\"id\":") + channel + ",\"on\":true,\"brightness\":100}";
    else
      payload = String("{\"id\":") + channel + ",\"on\":false}";

    int code = http.POST(payload);
    
    if (code == 200) {
      Serial.printf("Watering Shelly Channel %d -> %s SUCCESS on attempt %d (HTTP %d)\n", channel, (turnOn ? "ON" : "OFF"), attempt, code);
      success = true;
    } else if (code == -1) {
      Serial.printf("Watering Shelly Channel %d -> %s FAILED attempt %d: Connection error (HTTP -1)\n", channel, (turnOn ? "ON" : "OFF"), attempt);
      if (attempt < MAX_RETRIES) {
        Serial.println("Retrying in 2 seconds...");
        delay(2000);
      }
    } else {
      Serial.printf("Watering Shelly Channel %d -> %s attempt %d (HTTP %d - Unexpected response)\n", channel, (turnOn ? "ON" : "OFF"), attempt, code);
      if (attempt < MAX_RETRIES) {
        delay(1000);
      }
    }

    http.end();
  }

  if (!success) {
    Serial.printf("CRITICAL: Failed to control Watering Shelly Channel %d after %d attempts. Check device at %s\n", channel, MAX_RETRIES, shellyWaterIP);
  }
}

// === Moisture Reading (average of 50 samples) ===
int readMoistureAverage(int pin) {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  return sum / NUM_SAMPLES;
}

// ------------------ LIGHT CYCLE MANAGEMENT ------------------
void initializeChamberStates() {
  for (int i = 0; i < 3; i++) {
    chambers[i].currentCycle = CYCLE_DARK;
    chambers[i].currentPhase = PHASE_ADJUSTING;
    chambers[i].currentBrightness = 0;
    chambers[i].phaseStartTime = 0;
    chambers[i].lastAdjustTime = 0;
    chambers[i].cycleComplete = false;
    
    // Turn off all lights initially
    setLightBrightness(i, 0);
  }
  Serial.println("All light chambers initialized to DARK cycle");
}

void advanceToNextCycle(int idx) {
  ChamberState& chamber = chambers[idx];
  
  switch (chamber.currentCycle) {
    case CYCLE_DARK:
      chamber.currentCycle = CYCLE_DIM;
      Serial.printf("Light Chamber %d: Advancing to DIM cycle\n", idx + 1);
      break;
    case CYCLE_DIM:
      chamber.currentCycle = CYCLE_BRIGHT;
      Serial.printf("Light Chamber %d: Advancing to BRIGHT cycle\n", idx + 1);
      break;
    case CYCLE_BRIGHT:
      chamber.cycleComplete = true;
      Serial.printf("Light Chamber %d: Cycle complete!\n", idx + 1);
      setLightBrightness(idx, 0); // Turn off light when complete
      return;
  }
  
  chamber.currentPhase = PHASE_ADJUSTING;
  chamber.lastAdjustTime = millis();
}

// ------------------ HELPER FUNCTIONS ------------------
bool isInTargetRange(float lux, CycleState cycle) {
  switch (cycle) {
    case CYCLE_DARK:
      return lux < DARK_THRESHOLD;
    case CYCLE_DIM:
      return lux >= DARK_THRESHOLD && lux <= DIM_THRESHOLD;
    case CYCLE_BRIGHT:
      return lux > DIM_THRESHOLD;
  }
  return false;
}

const char* getCycleString(CycleState cycle) {
  switch (cycle) {
    case CYCLE_DARK: return "DARK";
    case CYCLE_DIM: return "DIM";
    case CYCLE_BRIGHT: return "BRIGHT";
  }
  return "UNKNOWN";
}

const char* getPhaseString(PhaseState phase) {
  switch (phase) {
    case PHASE_ADJUSTING: return "ADJUSTING";
    case PHASE_HOLDING: return "HOLDING";
  }
  return "UNKNOWN";
}

void printLightChamberStatus(int idx, float lux) {
  ChamberState& chamber = chambers[idx];
  unsigned long elapsed = millis() - cycleStartTime;
  
  Serial.printf("Light Chamber %d: %.2f lux | %s-%s | Brightness: %d%% | Elapsed: %lus\n", 
               idx + 1, lux, getCycleString(chamber.currentCycle), 
               getPhaseString(chamber.currentPhase), chamber.currentBrightness, elapsed/1000);
  
  if (chamber.currentPhase == PHASE_HOLDING) {
    unsigned long holdElapsed = millis() - chamber.phaseStartTime;
    unsigned long holdRemaining = (HOLD_DURATION - holdElapsed) / 1000;
    Serial.printf("  -> Holding for %lu more seconds\n", holdRemaining);
  }
}

// ------------------ VEML LIGHT SENSOR HELPERS ------------------
bool selectChannel(int channel) {
  if (channel < 0 || channel > 7) return false;
  Wire.beginTransmission(PCA9547D_ADDR);
  Wire.write(0x08 | channel);
  return Wire.endTransmission() == 0;
}

bool initVEML7700(int channel, int index) {
  if (!selectChannel(channel)) return false;
  if (!veml_sensors[index].begin()) return false;
  veml_sensors[index].setGain(VEML7700_GAIN_1);
  veml_sensors[index].setIntegrationTime(VEML7700_IT_100MS);
  return true;
}

float readVEMLlux(int channel, int index) {
  if (!selectChannel(channel)) return 0;
  return veml_sensors[index].readLux();
}