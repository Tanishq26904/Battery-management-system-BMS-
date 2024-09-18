#include <Wire.h>

#define BQ76940_ADDRESS 0x08  // I2C address of BQ76940

// BQ76940 register map (refer to datasheet)
#define SYS_STAT 0x00         // System status register
#define PROTECT1 0x06         // Protection configuration register 1
#define PROTECT2 0x07         // Protection configuration register 2
#define PROTECT3 0x08         // Protection configuration register 3
#define OVERCURRENT_THRESHOLD 0x09 // Placeholder, modify based on your desired threshold
#define CURRENT_SENSOR_REG 0x0A    // Placeholder for current reading

// Threshold values for overcurrent protection (adjust according to application)
#define OC_THRESHOLD_MV 100 // Overcurrent threshold (in mV across sense resistor)
#define SENSE_RESISTOR_OHMS 0.001  // Value of the sense resistor in ohms (e.g., 1 mΩ)

// Function prototypes
void setup();
void loop();
void configureOvercurrentProtection();
bool checkOvercurrentFault();
float readCurrent();

void setup() {
  Wire.begin();
  Serial.begin(9600);

  // Initialize BQ76940 and configure overcurrent protection
  configureOvercurrentProtection();
}

void loop() {
  // Continuously monitor for overcurrent
  if (checkOvercurrentFault()) {
    Serial.println("Overcurrent fault detected!");
    // Take necessary action here, such as shutting down the system or triggering an alert
  } else {
    Serial.println("Current is within safe limits.");
  }

  delay(1000);  // Check once per second
}

// Function to configure overcurrent protection
void configureOvercurrentProtection() {
  // Calculate the overcurrent threshold based on sense resistor and desired current limit
  uint8_t ocThreshold = (OC_THRESHOLD_MV / SENSE_RESISTOR_OHMS) / 4;  // BQ76940 uses a 4mV/LSB resolution
  
  Wire.beginTransmission(BQ76940_ADDRESS);
  Wire.write(PROTECT1);  // Address of the first protection register
  Wire.write(ocThreshold);  // Write the calculated overcurrent threshold
  Wire.endTransmission();

  Serial.println("Overcurrent protection configured.");
}

// Function to check for overcurrent fault
bool checkOvercurrentFault() {
  Wire.beginTransmission(BQ76940_ADDRESS);
  Wire.write(SYS_STAT);  // Read system status register
  Wire.endTransmission(false);

  Wire.requestFrom(BQ76940_ADDRESS, (uint8_t)1);
  uint8_t sysStat = Wire.read();

  // Check for the overcurrent flag (specific bit in SYS_STAT, refer to datasheet)
  return (sysStat & 0x04);  // Assume bit 2 represents the overcurrent flag
}

// Function to read current from BQ76940
float readCurrent() {
  int16_t currentRegVal;

  Wire.beginTransmission(BQ76940_ADDRESS);
  Wire.write(CURRENT_SENSOR_REG);  // Read current sensor register (placeholder)
  Wire.endTransmission(false);

  Wire.requestFrom(BQ76940_ADDRESS, (uint8_t)2);
  currentRegVal = Wire.read() | (Wire.read() << 8);

  // Convert register value to current (using sense resistor value)
  float current = (currentRegVal * 4) * SENSE_RESISTOR_OHMS;  // BQ76940 current sense resolution is 4mV/LSB

  return current;
}
