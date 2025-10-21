#ifndef I2CSCANNER_H
#define I2CSCANNER_H

#include <Arduino.h>
#include <Wire.h>

/**
 * @brief Scans the I2C bus for connected devices and prints results to Serial.
 * * @param sdaPin The GPIO pin number for I2C SDA (Data).
 * @param sclPin The GPIO pin number for I2C SCL (Clock).
 */
void i2cScan(int sdaPin, int sclPin);

#endif // I2CSCANNER_H
