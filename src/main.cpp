#include <Arduino.h>
#include <Wire.h>

//-------------------------------------------------------------
// ENS160 related items
//-------------------------------------------------------------
#include "ScioSense_ENS160.h" // ENS160 library
// Using address 0x53 as specified (ENS160_I2CADDR_1)
ScioSense_ENS160 ens160(ENS160_I2CADDR_1); 

//-------------------------------------------------------------
// I2C Scanner Function - Unmodified from previous working version
//-------------------------------------------------------------
void i2cScan() {
  byte error, address;
  int nDevices;

  Serial.println("\n--- I2C Bus Scanning ---");
  // Explicitly initialize I2C for ESP32 WROOM: SDA=GPIO21, SCL=GPIO22
  Wire.begin(21, 22); 
  Serial.println("I2C initialized on SDA=GPIO21, SCL=GPIO22.");
  nDevices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  <-- SUCCESS");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
    delay(1); 
  }

  if (nDevices == 0) {
    Serial.println("No I2C devices found. Check wiring (SDA=21, SCL=22) and pull-up resistors!");
  } else {
    Serial.print("Scan complete. Found ");
    Serial.print(nDevices);
    Serial.println(" device(s).");
  }
  Serial.println("------------------------\n");
}


/*--------------------------------------------------------------------------
  SETUP function
 --------------------------------------------------------------------------*/
void setup() {
  Serial.begin(115200);
  delay(100); 
  Serial.println("Starting Setup...");

  // 1. Run the I2C Scanner first to confirm the sensor is visible.
  i2cScan(); 

  Serial.println("------------------------------------------------------------");
  Serial.println("ENS160 - Digital air quality sensor");
  Serial.println("------------------------------------------------------------");
  delay(500);

  // Updated print to reflect ENS160_I2CADDR_1 (0x53)
  Serial.print("Attempting ENS160 initialization (0x53)...");
  
  // 2. Attempt the ENS160 initialization
  bool ok = ens160.begin();

  if (ok && ens160.available()) {
    Serial.println("done. ENS160 is ready.");

    // 3. Set the sensor to Standard mode for eCO2 and TVOC output
    // This is the default, but setting it explicitly is good practice.
    if (ens160.setMode(ENS160_OPMODE_STD)) {
      Serial.println("Mode set to ENS160_OPMODE_STANDARD.");
    } else {
      Serial.println("Failed to set sensor mode!");
    }
    
  } else {
    Serial.println("failed!");
    Serial.println("ENS160 init failed. Check address, power, and wiring.");
    // If init fails, we enter a safe loop instead of crashing to prevent WDT reset
    while(1) { delay(100); } 
  }
}

/*--------------------------------------------------------------------------
  MAIN LOOP FUNCTION
 --------------------------------------------------------------------------*/
void loop() {
  // If we reach the loop, the setup was successful.
  if (ens160.available()) {
    // Measure and update the sensor data
    // 'true' indicates using internal temperature/humidity compensation registers (if set)
    ens160.measure(true); 

    // Print Air Quality Index (AQI) based on the current resistance
    Serial.print("AQI: ");
    Serial.print(ens160.getAQI());
    
    // Print equivalent CO2 value in parts per million (ppm)
    Serial.print(" | eCO2: ");
    Serial.print(ens160.geteCO2());
    Serial.print(" ppm");
    
    // Print Total Volatile Organic Compounds value in parts per billion (ppb)
    Serial.print(" | TVOC: ");
    Serial.print(ens160.getTVOC());
    Serial.println(" ppb");
    
  } else {
    Serial.println("Sensor not available.");
  }

  delay(500); // Wait 500ms before the next reading
}
