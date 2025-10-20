#include <Arduino.h>
#include <Wire.h>



//-------------------------------------------------------------
// ENS160 related items
//-------------------------------------------------------------
#include "ScioSense_ENS160.h"  // ENS160 library
// The default addresses for ENS160 are 0x52 (ENS160_I2CADDR_0) or 0x53 (ENS160_I2CADDR_1)
ScioSense_ENS160 ens160(ENS160_I2CADDR_1); 

//-------------------------------------------------------------
// I2C Scanner Function
// Scans addresses 0x01 to 0x77 for responding devices.
//-------------------------------------------------------------
void i2cScan() {
  byte error, address;
  int nDevices;

  Serial.println("\n--- I2C Bus Scanning ---");
  Wire.begin(); // Initialize I2C (uses default pins: GPIO8/SDA, GPIO9/SCL on ESP32-C3)
  nDevices = 0;

  for (address = 1; address < 127; address++) {
    // Start transmission to the current address
    Wire.beginTransmission(address);
    // End transmission and get the status code
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
    // Delay slightly to prevent bus overload
    delay(1); 
  }

  if (nDevices == 0) {
    Serial.println("No I2C devices found. Check wiring and pull-up resistors!");
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
  
  // A small delay is sufficient for the monitor to catch up
  delay(100); 
  Serial.println("Starting Setup...");


  // 1. Run the I2C Scanner first to confirm the sensor is visible.
  i2cScan(); 

  Serial.println("------------------------------------------------------------");
  Serial.println("ENS160 - Digital air quality sensor");
  Serial.println("------------------------------------------------------------");
  delay(500);

  Serial.print("Attempting ENS160 initialization (0x52)...");
  
  // 2. Attempt the ENS160 initialization
  bool ok = ens160.begin();

  if (ok && ens160.available()) {
    Serial.println("done. ENS160 is ready.");
  } else {
    Serial.println("failed!");
    Serial.println("ENS160 init failed. Check address, power, and wiring.");
    // If init fails, we enter a safe loop instead of crashing to prevent WDT reset
    // You must press the reset button to exit this.
    while(1) { delay(100); } 
  }
}

/*--------------------------------------------------------------------------
  MAIN LOOP FUNCTION
 --------------------------------------------------------------------------*/
void loop() {
  // If we reach the loop, the setup was successful.
  Serial.println("Looping successfully...");
  delay(500); // Changed delay from 2000ms to 500ms
}
