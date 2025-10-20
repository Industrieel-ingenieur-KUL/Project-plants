#include <Arduino.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

//-------------------------------------------------------------
// BLE Definitions
//-------------------------------------------------------------
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c4c6eb4c606b" // Custom Service UUID
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" // Custom Characteristic UUID for sensor data
#define DEVICE_NAME         "ENS160 Air Quality"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;

// Callback class to handle connection/disconnection events
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("BLE Client Connected.");
    }

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("BLE Client Disconnected. Restarting Advertising...");
      // Restart advertising to allow reconnections
      pServer->startAdvertising(); 
    }
};

//-------------------------------------------------------------
// ENS160 related items
//-------------------------------------------------------------
#include "ScioSense_ENS160.h"  // ENS160 library
// Using address 0x53 as specified (ENS160_I2CADDR_1)
ScioSense_ENS160 ens160(ENS160_I2CADDR_1); 

//-------------------------------------------------------------
// I2C Scanner Function 
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

//-------------------------------------------------------------
// BLE Setup Function
//-------------------------------------------------------------
void setupBLE() {
  Serial.println("Setting up BLE Server...");

  // Initialize BLE Device
  BLEDevice::init(DEVICE_NAME);
  
  // *** FIX: Request a larger MTU (Maximum Transmission Unit) ***
  // This is required for sending data strings longer than the default 20 bytes
  BLEDevice::setMTU(512); 
  // ************************************************************

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY 
                    );

  // Add a descriptor (to allow notifications/subscriptions)
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections and stability
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("BLE Advertising started. Device name: " DEVICE_NAME);
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

    // -------------------------------------------------------------------
    // EXPLANATION OF MEASUREMENTS (NEW CODE)
    // -------------------------------------------------------------------
    Serial.println("\n--- Sensor Output Explanation ---");
    Serial.println("AQI: Air Quality Index (1=Excellent to 5=Poor)");
    Serial.println("eCO2: Equivalent CO2 concentration (in parts per million - ppm)");
    Serial.println("TVOC: Total Volatile Organic Compounds (in parts per billion - ppb)");
    Serial.println("-----------------------------------\n");
    // -------------------------------------------------------------------

    // 3. Set the sensor to Standard mode for eCO2 and TVOC output
    // Corrected the constant from _STD to _STANDARD
    if (ens160.setMode(ENS160_OPMODE_STD)) {
      Serial.println("Mode set to ENS160_OPMODE_STANDARD.");
    } else {
      Serial.println("Failed to set sensor mode!");
    }
    
    // 4. Initialize the BLE system
    setupBLE();

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
  // Only proceed if the sensor is available
  if (ens160.available()) {
    // Measure and update the sensor data
    ens160.measure(true); 

    // Read the sensor values
    int aqi = ens160.getAQI();
    int eco2 = ens160.geteCO2();
    int tvoc = ens160.getTVOC();
    
    // Format the data into a single string
    String dataString = "AQI:" + String(aqi) + 
                        " | eCO2:" + String(eco2) + 
                        "ppm | TVOC:" + String(tvoc) + "ppb";

    // Print to Serial (for local debugging)
    Serial.println(dataString);

    // Send data via BLE if a client is connected
    if (deviceConnected) {
      pCharacteristic->setValue(dataString.c_str());
      pCharacteristic->notify(); // Send the notification
    }
  } else {
    Serial.println("Sensor not available.");
  }

  // Check for disconnected clients and restart advertising if needed
  if (!deviceConnected && pServer) {
    // This is handled in the MyServerCallbacks::onDisconnect, but good to check
    // If the device was advertising and a client disconnected, it should restart automatically.
  }

  delay(500); // Wait 500ms before the next reading/notification
}
