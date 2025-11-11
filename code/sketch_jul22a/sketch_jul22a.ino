#include <Wire.h>
#include "Adafruit_VEML7700.h"

// PCA9547D I2C multiplexer configuration
#define SDA_PIN 13
#define SCL_PIN 14
#define PCA9547D_ADDR 0x70

// Sensor I2C addresses
#define SHT4X_ADDR 0x44

// SHT4x sensor configuration - channels 0, 1, 2
#define SHT4X_CHANNEL_START 0
#define SHT4X_CHANNEL_END 2
#define SHT4X_COUNT 3

// VEML7700 sensor configuration - channels 5, 6, 7
#define VEML7700_CHANNEL_START 5
#define VEML7700_CHANNEL_END 7
#define VEML7700_COUNT 3

// SHT4x commands
#define SHT4X_CMD_MEASURE_HIGH_PRECISION 0xFD
#define SHT4X_CMD_READ_SERIAL 0x89
#define SHT4X_CMD_SOFT_RESET 0x94

// Create VEML7700 sensor objects for each channel
Adafruit_VEML7700 veml_sensors[VEML7700_COUNT];

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("PCA9547D Multi-Channel Sensor Test");
  Serial.println("SHT45 sensors on channels 0-2, VEML7700 sensors on channels 5-7");
  Serial.println("================================================================");
  
  // Initialize I2C with custom pins
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000); // 100kHz I2C clock
  
  Serial.printf("I2C initialized with SDA: %d, SCL: %d\n", SDA_PIN, SCL_PIN);
  
  // Test PCA9547D communication
  testPCA9547D();
  
  // Initialize all SHT45 sensors
  Serial.println("\nInitializing SHT45 sensors...");
  for (int channel = SHT4X_CHANNEL_START; channel <= SHT4X_CHANNEL_END; channel++) {
    if (initSHT4x(channel)) {
      Serial.printf("✓ SHT45 #%d (Channel %d) initialized successfully\n", channel + 1, channel);
    } else {
      Serial.printf("✗ Failed to initialize SHT45 #%d on Channel %d\n", channel + 1, channel);
    }
  }
  
  // Initialize all VEML7700 sensors
  Serial.println("\nInitializing VEML7700 sensors...");
  for (int channel = VEML7700_CHANNEL_START; channel <= VEML7700_CHANNEL_END; channel++) {
    int sensor_index = channel - VEML7700_CHANNEL_START;
    if (initVEML7700(channel, sensor_index)) {
      Serial.printf("✓ VEML7700 #%d (Channel %d) initialized successfully\n", sensor_index + 1, channel);
    } else {
      Serial.printf("✗ Failed to initialize VEML7700 #%d on Channel %d\n", sensor_index + 1, channel);
    }
  }
  
  Serial.println("\nSetup complete! Starting sensor readings...");
}

void loop() {
  Serial.println("\n============================================================");
  Serial.println("=== Reading All Sensor Data ===");
  Serial.println("============================================================");
  
  // Read all SHT4x sensors on channels 0-2
  Serial.println("\n--- SHT4x Temperature & Humidity Sensors ---");
  for (int channel = SHT4X_CHANNEL_START; channel <= SHT4X_CHANNEL_END; channel++) {
    Serial.printf("\nSHT4x Sensor #%d (Channel %d):\n", channel + 1, channel);
    readSHT4xData(channel);
    delay(200); // Small delay between readings
  }
  
  // Read all VEML7700 sensors on channels 5-7
  Serial.println("\n--- VEML7700 Light Sensors ---");
  for (int channel = VEML7700_CHANNEL_START; channel <= VEML7700_CHANNEL_END; channel++) {
    int sensor_index = channel - VEML7700_CHANNEL_START;
    Serial.printf("\nVEML7700 Sensor #%d (Channel %d):\n", sensor_index + 1, channel);
    readVEML7700Data(channel, sensor_index);
    delay(200); // Small delay between readings
  }
  
  delay(200); // Wait before next reading cycle
}

void testPCA9547D() {
  Serial.println("\nTesting PCA9547D communication...");
  
  // Try to communicate with the multiplexer
  Wire.beginTransmission(PCA9547D_ADDR);
  byte error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.printf("✓ PCA9547D confirmed at address 0x%02X\n", PCA9547D_ADDR);
    
    // Read current channel selection
    Wire.requestFrom(PCA9547D_ADDR, 1);
    if (Wire.available()) {
      byte currentChannel = Wire.read();
      Serial.printf("Current channel register: 0x%02X\n", currentChannel);
      
      if (currentChannel & 0x08) {
        int activeChannel = currentChannel & 0x07;
        Serial.printf("Active channel: %d\n", activeChannel);
      } else {
        Serial.println("No channel currently selected");
      }
    }
  } else {
    Serial.printf("✗ Communication error with PCA9547D at 0x%02X (error: %d)\n", PCA9547D_ADDR, error);
    Serial.println("Check SDA/SCL connections and pull-up resistors!");
  }
}

// Helper function to select a specific channel
bool selectChannel(int channel) {
  if (channel < 0 || channel > 7) {
    Serial.printf("Invalid channel %d! Must be 0-7\n", channel);
    return false;
  }
  
  byte channelSelect = 0x08 | channel;
  Wire.beginTransmission(PCA9547D_ADDR);
  Wire.write(channelSelect);
  byte error = Wire.endTransmission();
  
  return (error == 0);
}

// Helper function to disable all channels
void disableAllChannels() {
  Wire.beginTransmission(PCA9547D_ADDR);
  Wire.write(0x00);
  Wire.endTransmission();
}

// SHT4x sensor functions using direct I2C communication
bool initSHT4x(int channel) {
  // Select channel for SHT4x
  if (!selectChannel(channel)) {
    Serial.printf("Failed to select SHT4x channel %d\n", channel);
    return false;
  }
  
  // Try to communicate with SHT4x
  Wire.beginTransmission(SHT4X_ADDR);
  byte error = Wire.endTransmission();
  
  if (error != 0) {
    Serial.printf("SHT4x not found on channel %d (error: %d)\n", channel, error);
    return false;
  }
  
  // Send soft reset command
  Wire.beginTransmission(SHT4X_ADDR);
  Wire.write(SHT4X_CMD_SOFT_RESET);
  Wire.endTransmission();
  delay(10); // Wait for reset
  
  Serial.printf("✓ SHT4x found on channel %d\n", channel);
  return true;
}

void readSHT4xData(int channel) {
  // Select channel for SHT4x
  if (!selectChannel(channel)) {
    Serial.printf("Failed to select SHT4x channel %d\n", channel);
    return;
  }
  
  // Send measurement command
  Wire.beginTransmission(SHT4X_ADDR);
  Wire.write(SHT4X_CMD_MEASURE_HIGH_PRECISION);
  byte error = Wire.endTransmission();
  
  if (error != 0) {
    Serial.printf("Failed to send measurement command (error: %d)\n", error);
    return;
  }
  
  delay(10); // Wait for measurement (high precision takes ~8.5ms)
  
  // Read 6 bytes of data
  Wire.requestFrom(SHT4X_ADDR, 6);
  
  if (Wire.available() < 6) {
    Serial.println("Failed to read SHT4x data");
    return;
  }
  
  byte data[6];
  for (int i = 0; i < 6; i++) {
    data[i] = Wire.read();
  }
  
  // Convert raw data to temperature and humidity
  uint16_t temp_raw = (data[0] << 8) | data[1];
  uint16_t hum_raw = (data[3] << 8) | data[4];
  
  float temperature = -45.0 + 175.0 * temp_raw / 65535.0;
  float humidity = -6.0 + 125.0 * hum_raw / 65535.0;
  
  // Clamp humidity to valid range
  if (humidity > 100.0) humidity = 100.0;
  if (humidity < 0.0) humidity = 0.0;
  
  // Display results
  Serial.printf("  Temperature: %.2f°C (%.2f°F)\n", 
                temperature, 
                temperature * 9.0/5.0 + 32.0);
  Serial.printf("  Humidity: %.2f%% RH\n", humidity);
  
  // Additional environmental assessments
  if (temperature < 0) {
    Serial.println("  Temperature: Freezing");
  } else if (temperature < 16) {
    Serial.println("  Temperature: Cold");
  } else if (temperature < 24) {
    Serial.println("  Temperature: Comfortable");
  } else if (temperature < 30) {
    Serial.println("  Temperature: Warm");
  } else {
    Serial.println("  Temperature: Hot");
  }
  
  if (humidity < 30) {
    Serial.println("  Humidity: Dry");
  } else if (humidity < 60) {
    Serial.println("  Humidity: Comfortable");
  } else if (humidity < 80) {
    Serial.println("  Humidity: Humid");
  } else {
    Serial.println("  Humidity: Very Humid");
  }
}

// VEML7700 sensor functions using Adafruit library
bool initVEML7700(int channel, int sensor_index) {
  // Select channel for VEML7700
  if (!selectChannel(channel)) {
    Serial.printf("Failed to select VEML7700 channel %d\n", channel);
    return false;
  }
  
  // Initialize the VEML7700 sensor using Adafruit library
  if (!veml_sensors[sensor_index].begin()) {
    Serial.printf("VEML7700 #%d not found on channel %d\n", sensor_index + 1, channel);
    return false;
  }
  
  Serial.printf("✓ VEML7700 #%d found on channel %d\n", sensor_index + 1, channel);
  
  // Configure sensor settings (optional - you can customize these)
  veml_sensors[sensor_index].setGain(VEML7700_GAIN_1);
  veml_sensors[sensor_index].setIntegrationTime(VEML7700_IT_100MS);
  
  // Set interrupt thresholds (optional)
  veml_sensors[sensor_index].setLowThreshold(10);
  veml_sensors[sensor_index].setHighThreshold(10000);
  veml_sensors[sensor_index].interruptEnable(false); // Disable interrupts for now
  
  // Print current configuration
  Serial.printf("  Gain: ");
  switch (veml_sensors[sensor_index].getGain()) {
    case VEML7700_GAIN_1: Serial.println("1"); break;
    case VEML7700_GAIN_2: Serial.println("2"); break;
    case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
    case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
  }
  
  Serial.printf("  Integration Time: ");
  switch (veml_sensors[sensor_index].getIntegrationTime()) {
    case VEML7700_IT_25MS: Serial.println("25ms"); break;
    case VEML7700_IT_50MS: Serial.println("50ms"); break;
    case VEML7700_IT_100MS: Serial.println("100ms"); break;
    case VEML7700_IT_200MS: Serial.println("200ms"); break;
    case VEML7700_IT_400MS: Serial.println("400ms"); break;
    case VEML7700_IT_800MS: Serial.println("800ms"); break;
  }
  
  delay(100); // Wait for configuration
  return true;
}

void readVEML7700Data(int channel, int sensor_index) {
  // Select channel for VEML7700
  if (!selectChannel(channel)) {
    Serial.printf("Failed to select VEML7700 channel %d\n", channel);
    return;
  }
  
  // Read data using Adafruit library
  uint16_t als_raw = veml_sensors[sensor_index].readALS();
  uint16_t white_raw = veml_sensors[sensor_index].readWhite();
  float lux = veml_sensors[sensor_index].readLux();
  
  // Display results
  Serial.printf("  Ambient Light: %.2f lux\n", lux);
  Serial.printf("  ALS Raw: %d\n", als_raw);
  Serial.printf("  White Raw: %d\n", white_raw);
  
  // Check interrupt status
  uint16_t irq = veml_sensors[sensor_index].interruptStatus();
  if (irq & VEML7700_INTERRUPT_LOW) {
    Serial.println("  ** Low threshold triggered");
  }
  if (irq & VEML7700_INTERRUPT_HIGH) {
    Serial.println("  ** High threshold triggered");
  }
  
  // Light level description
  if (lux < 1) {
    Serial.println("  Light Level: Very Dark");
  } else if (lux < 10) {
    Serial.println("  Light Level: Dark");
  } else if (lux < 100) {
    Serial.println("  Light Level: Dim");
  } else if (lux < 1000) {
    Serial.println("  Light Level: Normal Indoor");
  } else if (lux < 10000) {
    Serial.println("  Light Level: Bright Indoor");
  } else {
    Serial.println("  Light Level: Very Bright/Outdoor");
  }
}

void scanI2CDevices() {
  Serial.println("\nScanning for I2C devices...");
  int deviceCount = 0;
  
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.printf("Device found at address 0x%02X\n", addr);
      deviceCount++;
    }
  }
  
  if (deviceCount == 0) {
    Serial.println("No I2C devices found!");
    Serial.println("Check wiring and pull-up resistors (4.7kΩ on SDA and SCL)");
  } else {
    Serial.printf("Scan complete. Found %d device(s)\n", deviceCount);
  }
}

void testAllChannels() {
  for (int channel = 0; channel < 8; channel++) {
    Serial.printf("\nTesting Channel %d:\n", channel);
    
    // Select channel (bit 3 = enable, bits 2-0 = channel)
    byte channelSelect = 0x08 | channel;
    
    Wire.beginTransmission(PCA9547D_ADDR);
    Wire.write(channelSelect);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.printf("  ✓ Channel %d selected successfully\n", channel);
      
      // Scan for devices on this channel
      scanChannelDevices(channel);
    } else {
      Serial.printf("  ✗ Failed to select channel %d (error: %d)\n", channel, error);
    }
  }
  
  // Disable all channels
  Wire.beginTransmission(PCA9547D_ADDR);
  Wire.write(0x00);
  Wire.endTransmission();
  Serial.println("\nAll channels disabled");
}

void scanChannelDevices(int channel) {
  int deviceCount = 0;
  
  for (byte addr = 1; addr < 127; addr++) {
    // Skip the multiplexer's address (0x70)
    if (addr == 0x70) continue;
    
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.printf("    Device found at 0x%02X\n", addr);
      deviceCount++;
    }
  }
  
  if (deviceCount == 0) {
    Serial.println("    No devices found on this channel");
  } else {
    Serial.printf("    Total devices found: %d\n", deviceCount);
  }
}