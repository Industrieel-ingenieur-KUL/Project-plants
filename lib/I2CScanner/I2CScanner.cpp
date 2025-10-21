#include "I2CScanner.h"
#include <Arduino.h> // Explicitly include Arduino core for Serial object access

// Note: Wire.h is included via I2CScanner.h

/**
 * @brief Scans the I2C bus for connected devices and prints results to Serial.
 * * @param sdaPin The GPIO pin number for I2C SDA (Data).
 * @param sclPin The GPIO pin number for I2C SCL (Clock).
 */
void i2cScan(int sdaPin, int sclPin)
{
  byte error, address;
  int nDevices;

  Serial.println("\n--- I2C Bus Scanning ---");
  // Explicitly initialize I2C using the defined pins passed as arguments
  Wire.begin(sdaPin, sclPin);
  Serial.print("I2C initialized on SDA=GPIO");
  Serial.print(sdaPin);
  Serial.print(", SCL=GPIO");
  Serial.print(sclPin);
  Serial.println(".");

  nDevices = 0;

  for (address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  <-- SUCCESS");
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
    delay(1);
  }

  if (nDevices == 0)
  {
    Serial.print("No I2C devices found. Check wiring (SDA=");
    Serial.print(sdaPin);
    Serial.print(", SCL=");
    Serial.print(sclPin);
    Serial.println(") and pull-up resistors!");
  }
  else
  {
    Serial.print("Scan complete. Found ");
    Serial.print(nDevices);
    Serial.println(" device(s).");
  }
  Serial.println("------------------------\n");
}
