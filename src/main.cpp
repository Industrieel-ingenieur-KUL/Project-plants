#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_ENS160.h>

// --- OLED DEFINITIONS ---
#define SCREEN_WIDTH 128    // OLED display width, in pixels (Kept 128 for library compatibility)
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET    -1    
#define SCREEN_ADDRESS 0x3C 

// --- I2C PIN DEFINITIONS ---
#define SDA_PIN 5
#define SCL_PIN 6

// --- OBJECT DECLARATIONS ---
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_AHTX0 aht;
Adafruit_ENS160 ens160;

// Global variables for sensor readings
sensors_event_t humidity, temp;
float TVOC, eCO2;

// --- LAYOUT CONSTANTS ---
const int START_X = 28; // The horizontal starting position
const int START_Y = 24; // The vertical starting position for the first data line
const int LINE_HEIGHT = 8; // The vertical increment for each line

// ====================================================================
void setup() {
    Serial.begin(115200);

    // 1. Initialize I2C Bus
    Wire.begin(SDA_PIN, SCL_PIN);
    
    // 2. Initialize OLED Display
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed. Check wiring/address."));
        for(;;)delay(100); 
    }
    display.display();
    delay(100); 

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(START_X, 8); // Setup message is higher up, at y=8
    display.println(F("QT Py Sensor Init..."));
    display.display();

    // 3. Initialize AHT2x Sensor
    if (!aht.begin()) {
      Serial.println("Could not find AHT sensor. Check wiring.");
    } else {
      Serial.println("AHT2x found.");
    }
    
    // 4. Initialize ENS160 Sensor
    if (!ens160.begin()) {
      Serial.println("Could not find ENS160 sensor. Check wiring/address.");
    } else {
      Serial.println("ENS160 found.");
      ens160.setOperatingMode(ENS160_OPMODE_STANDARD); 
    }
    
    delay(2000);
}

// ====================================================================
void loop() {
    // 1. Read AHT2x Sensor
    aht.getEvent(&humidity, &temp); 
    
    // 2. Read ENS160 Sensor
    // Update the ENS160 with the latest T/H for better accuracy
    ens160.setTempCompensation(temp.temperature);
    ens160.setHumidityCompensation(humidity.relative_humidity);
    
    // Get TVOC and eCO2 values
    TVOC = ens160.getTVOC();
    eCO2 = ens160.geteCO2();
    
    // 3. Display Data on OLED
    display.clearDisplay();
    display.setTextSize(1); 
    display.setTextColor(SSD1306_WHITE); 

    // --- Line 1: Temperature ---
    display.setCursor(START_X, START_Y); 
    display.print(F("T: "));
    display.print(temp.temperature, 1); // 1 decimal place to save space
    display.println(F("C")); // 12 characters: T: 25.5 C (approx)

    // --- Line 2: Humidity ---
    display.setCursor(START_X, START_Y + LINE_HEIGHT * 1); // 24 + 8 = 32
    display.print(F("H: "));
    display.print(humidity.relative_humidity, 0); // 0 decimal places to save space
    display.println(F("%")); // 12 characters: H: 50 % (approx)

    // --- Line 3: TVOC ---
    display.setCursor(START_X, START_Y + LINE_HEIGHT * 2); // 24 + 16 = 40
    display.print(F("TVOC: "));
    display.print((int)TVOC); // Print as integer
    display.println(F("ppb")); // 12 characters: TVOC: 000 ppb (approx)

    // --- Line 4: eCO2 ---
    display.setCursor(START_X, START_Y + LINE_HEIGHT * 3); // 24 + 24 = 48
    display.print(F("eCO2: "));
    display.print((int)eCO2); // Print as integer
    display.println(F("ppm")); // 12 characters: eCO2: 0000 ppm (approx)

    // --- Line 5: AQI Status (Takes the last available line) ---
    display.setCursor(START_X, START_Y + LINE_HEIGHT * 4); // 24 + 32 = 56
    display.print(F("AQI: "));
    
    // Print the first 7 characters of the status string
    String aqiStr = ens160.getAQIString();
    if (aqiStr.length() > 7) {
        aqiStr = aqiStr.substring(0, 7);
    }
    display.println(aqiStr); // 12 characters: AQI: Good (approx)

    display.display();
    
    // 4. Print to Serial Monitor
    Serial.print(F("Temp: ")); Serial.print(temp.temperature, 1); Serial.println(F(" *C"));
    Serial.print(F("Hum: ")); Serial.print(humidity.relative_humidity, 0); Serial.println(F(" %"));
    Serial.print(F("TVOC: ")); Serial.print((int)TVOC); Serial.println(F(" ppb"));
    Serial.print(F("eCO2: ")); Serial.print((int)eCO2); Serial.println(F(" ppm"));
    Serial.print(F("AQI Status: ")); Serial.println(ens160.getAQIString());
    Serial.println("---");
    
    delay(2000); 
}