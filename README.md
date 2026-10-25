# Arduino Water Level Monitor

A complete hardware and software solution for monitoring water levels using an Arduino Uno. This project reads analog data from a water level sensor, smooths the signal, and provides real-time feedback through an I2C LCD, a 7-segment display (0-9 scale), and three status LEDs (Green, Yellow, Red). 

## Features
* **Real-Time Display:** I2C LCD shows raw sensor values and categorical status (Empty, Low, Medium, High).
* **0-9 Scale Readout:** A single-digit 7-segment display dynamically maps the water depth to a 0-9 scale.
* **Visual Alerts:** Three indicator LEDs trigger based on customizable depth thresholds.
* **Signal Smoothing:** Takes an average of 8 consecutive analog readings to eliminate sensor noise.
* **Startup Lamp Test:** Validates all LEDs and 7-segment segments automatically on boot.

## Hardware Requirements
* 1x Arduino Uno R3
* 1x Analog Water Level Sensor
* 1x 16x2 I2C LCD Display
* 1x 7-Segment Display (Common Cathode by default; configurable)
* 3x LEDs (Green, Yellow, Red)
* 10x 220Ω Resistors (7 for display, 3 for LEDs)
* Jumper wires & Breadboard

## Wiring & Pinout

### 1. Water Sensor
| Sensor Pin | Arduino Pin |
| :--- | :--- |
| Signal (S) | A0 |
| VCC (+) | 5V |
| GND (-) | GND |

### 2. I2C LCD Display
| LCD Pin | Arduino Pin |
| :--- | :--- |
| SDA | A4 |
| SCL | A5 |
| VCC | 5V |
| GND | GND |

### 3. Indicator LEDs
*Connect a 220Ω resistor in series with each LED. Cathodes go to GND.*
| LED Color | State | Arduino Pin |
| :--- | :--- | :--- |
| Green | LOW | D9 |
| Yellow | MEDIUM | D10 |
| Red | HIGH | D11 |

### 4. 7-Segment Display (a-g)
*Connect a 220Ω resistor in series with each segment pin. Connect COM to GND (or 5V if using Common Anode).*
| Segment | Arduino Pin |
| :---: | :---: |
| **a** | D4 |
| **b** | D5 |
| **c** | D8 |
| **d** | D7 |
| **e** | D6 |
| **f** | D3 |
| **g** | D2 |

## Installation & Setup

1. **Install Dependencies:** You will need the `LiquidCrystal_I2C` library. Install it via the Arduino IDE Library Manager.
2. **Clone the Repository:**
   ```bash
   git clone [https://github.com/yourusername/arduino-water-level-monitor.git](https://github.com/yourusername/arduino-water-level-monitor.git)
