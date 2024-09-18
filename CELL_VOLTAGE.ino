#include <Wire.h>

#define BQ76940_ADDRESS 0x08  // Default I2C address of BQ76940

// BQ76940 register map (Refer to the BQ76940 datasheet for full register addresses)
#define SYS_STAT 0x00   // System status register
#define CELL_VOLTAGE1 0x04 // Cell voltage starting register

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  // Initialize BQ76940
  initializeBQ76940();
}

void loop() {
  // Read cell voltages
  readCellVoltages();
  
  // Add delay for ease of monitoring
  delay(1000);
}

void initializeBQ76940() {
  // Wake up the BQ76940 if needed (Optional, depends on your setup)
  // If the BQ76940 is in SHIP mode or SLEEP mode, you'll need to wake it up
  // Check the datasheet for more information on waking the IC.

  Serial.println("BQ76940 Initialized");
}

// Function to read cell voltages
void readCellVoltages() {
  uint16_t cellVoltage[15];  // Array to store cell voltages
  int cellStartAddr = CELL_VOLTAGE1;  // Start address for cell voltages
  
  // Start of the packet
  Serial.print("<");

  for (int i = 0; i < 15; i++) {
    // Read two bytes for each cell's voltage
    cellVoltage[i] = readBQ76940Register(cellStartAddr + (i * 2)) | (readBQ76940Register(cellStartAddr + (i * 2) + 1) << 8);
    
    // Convert to volts (1mV resolution)
    float voltage = cellVoltage[i] * 0.001;  
    
    // Send the cell voltage with a comma delimiter, except for the last voltage
    Serial.print(voltage);
    if (i < 14) {
      Serial.print(",");  // Add a comma after each voltage except the last one
    }
  }
  
  // End of the packet
  Serial.println(">");
}


// Function to read a register from BQ76940
uint8_t readBQ76940Register(uint8_t reg) {
  Wire.beginTransmission(BQ76940_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);
  
  Wire.requestFrom(BQ76940_ADDRESS, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  }
  
  return 0;
}
