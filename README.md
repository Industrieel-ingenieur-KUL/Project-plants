# 🌿 ENS160 Air Quality Monitor with BLE (ESP32)

This project demonstrates how to use an **ESP32** microcontroller with the **ScioSense ENS160 air quality sensor** to measure **Air Quality Index (AQI)**, **eCO₂**, and **TVOC**, and broadcast the data via **Bluetooth Low Energy (BLE)** to a mobile app or BLE client.

---

## ✨ Features

- 📡 **BLE Broadcasting** — Streams real-time air quality data to any BLE client.  
- 🌬️ **Air Quality Monitoring** — Provides **AQI**, **eCO₂ (ppm)**, and **TVOC (ppb)** readings.  
- 🔍 **I²C Auto-Detection** — Automatically scans and confirms I²C device connectivity.  
- 🔄 **Auto Reconnect** — BLE advertising restarts automatically after client disconnection.  
- ⚙️ **High MTU Support** — Enables BLE packets up to 512 bytes.  
- 🧠 **Stable Operation** — Includes safe fallback loop if sensor initialization fails.

---

## 🧩 Hardware Setup

| Component | Description | Connection |
|------------|-------------|-------------|
| **ESP32** | Wi-Fi + BLE capable microcontroller | — |
| **ENS160 Sensor** | ScioSense digital air quality sensor | I²C interface |
| **Wiring** | SDA → GPIO 21<br>SCL → GPIO 22<br>VCC → 3.3V<br>GND → GND | — |

> 💡 Default I²C address used: **0x53** (`ENS160_I2CADDR_1`)

---

## 🔧 Required Libraries

Before uploading the sketch, make sure the following libraries are installed via the **Arduino Library Manager**:

- **ScioSense_ENS160** — for communicating with the ENS160 air quality sensor  
- **I2CScanner** — for detecting I²C devices on the bus  
- **ESP32 BLE Arduino** — provides BLE server and characteristic support  

To install:
1. Open **Arduino IDE → Tools → Manage Libraries...**
2. Search and install each library by name.

---

## ⚙️ How It Works

1. **Initialization Phase**
   - The ESP32 scans for I²C devices and verifies the ENS160 sensor connection.
   - The sensor is initialized and set to **Standard Mode**.
   - BLE service and characteristic are created, and advertising begins.

2. **Measurement Phase**
   - Sensor continuously measures **AQI**, **eCO₂**, and **TVOC** values.
   - Data is printed to the Serial Monitor.
   - If a BLE client is connected, the readings are sent as BLE notifications.

---

## 📱 BLE Service Information

| Attribute | UUID | Description |
|------------|------|-------------|
| **Service** | `4fafc201-1fb5-459e-8fcc-c4c6eb4c606b` | Air Quality Service |
| **Characteristic** | `beb5483e-36e1-4688-b7f5-ea07361b26a8` | Sends sensor data string |

### Example BLE Data:
AQI:2 | eCO2:450ppm | TVOC:32ppb


You can view this data using BLE apps like:
- **nRF Connect (Android / iOS)**
- **LightBlue (macOS / iOS)**
- **BLE Scanner (Android)**

---

## 🧠 Example Serial Output

Starting Setup...
Scanning I2C bus...
Found I2C device at address 0x53
ENS160 - Digital air quality sensor

Attempting ENS160 initialization (0x53)...done. ENS160 is ready.
Mode set to ENS160_OPMODE_STANDARD.
BLE Advertising started. Device name: ENS160 Air Quality

AQI:1 | eCO2:400ppm | TVOC:5ppb
AQI:2 | eCO2:450ppm | TVOC:30ppb


---

## 🔋 Power Notes

- The ENS160 operates at **3.3V** — connect it directly to the ESP32’s 3.3V pin.  
- **Do not** connect to 5V; the sensor is **not 5V-tolerant**.  
- Keep I²C wires short to avoid signal noise.

---

## 💡 Troubleshooting

If the ENS160 initialization fails:
- Check wiring (SDA = GPIO 21, SCL = GPIO 22)
- Confirm the I²C address (0x53 or 0x52 depending on sensor variant)
- Ensure the sensor is powered and grounded correctly
- Open the Serial Monitor (115200 baud) for detailed debug output

If BLE doesn’t show up:
- Restart ESP32 after flashing
- Make sure Bluetooth is enabled on your device
- Use a BLE scanning app (not classic Bluetooth)

---

## 📜 License

This project is open-source and released under the **MIT License**.  
Feel free to use, modify, and share it.

---

## 👨‍💻 Author

**ESP32 + ENS160 Air Quality Monitor**  
Developed with ❤️ for makers and IoT enthusiasts.

---

### 🌍 Keywords

`ESP32` `BLE` `Bluetooth Low Energy` `ENS160` `Air Quality` `TVOC` `eCO2` `AQI` `I2C` `Arduino` `IoT`

---
