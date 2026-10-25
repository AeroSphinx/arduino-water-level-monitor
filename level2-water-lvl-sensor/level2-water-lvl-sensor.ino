// =============================================================================
//  Water Level Sensor with I2C LCD, 3 indicator LEDs, and 7-segment display
//  Board: Arduino Uno R3
//
//  Wiring summary
//    Water sensor : S -> A0, + -> 5V, - -> GND
//    I2C LCD      : SDA -> A4, SCL -> A5, VCC -> 5V, GND -> GND
//    7-segment    : segments a..g -> D2..D8 (220 ohm each), COM -> GND
//    LEDs         : Green (LOW) -> D9, Yellow (MEDIUM) -> D10, Red (HIGH) -> D11
//                   (220 ohm each, cathodes to GND)
//    D0 and D1 are NOT used (they are the USB serial pins).
//
//  Behavior
//    Empty  : all LEDs off, display shows the scaled level (0)
//    LOW    : green LED
//    MEDIUM : yellow LED
//    HIGH   : red LED
//    The 7-segment display shows the water level on a 0-9 scale.
// =============================================================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- Pins ----------
const uint8_t SENSOR_PIN = A0;

// 7-segment segments a, b, c, d, e, f, g
const uint8_t SEG_PINS[7] = {4, 5, 8, 7, 6, 3, 2};

const uint8_t LED_GREEN  = 9;
const uint8_t LED_YELLOW = 10;
const uint8_t LED_RED    = 11;

// ---------- Display type ----------
// false = common cathode (COM to GND), true = common anode (COM to 5V)
const bool COMMON_ANODE = false;

// ---------- Calibration ----------
// Edit these after watching the Serial Monitor (9600 baud) while raising the water.
const int LOW_MIN     = 1;    // reading at or above this = LOW    (green)
const int MEDIUM_MIN  = 200;  // reading at or above this = MEDIUM (yellow)
const int HIGH_MIN    = 250;  // reading at or above this = HIGH   (red)
const int SENSOR_FULL = 300;  // reading that shows 9 on the 7-segment display

const unsigned long UPDATE_MS = 250;  // how often the readout refreshes

// ---------- Categories ----------
const uint8_t CAT_EMPTY  = 0;
const uint8_t CAT_LOW    = 1;
const uint8_t CAT_MEDIUM = 2;
const uint8_t CAT_HIGH   = 3;

// ---------- Digit bitmasks (bit0 = a ... bit6 = g, 1 = segment on) ----------
const uint8_t DIGITS[10] = {
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F  // 9
};

unsigned long lastUpdate = 0;

// ---------- Helpers ----------

// Average several readings to smooth out sensor noise
int readSensor() {
  long sum = 0;
  for (uint8_t i = 0; i < 8; i++) {
    sum += analogRead(SENSOR_PIN);
    delay(2);
  }
  return (int)(sum / 8);
}

// Convert a sensor reading into a category (no gaps between ranges)
uint8_t getCategory(int value) {
  if (value >= HIGH_MIN)   return CAT_HIGH;
  if (value >= MEDIUM_MIN) return CAT_MEDIUM;
  if (value >= LOW_MIN)    return CAT_LOW;
  return CAT_EMPTY;
}

// Convert a sensor reading into a 0-9 digit for the 7-segment display
uint8_t getDigit(int value) {
  long d = map(value, 0, SENSOR_FULL, 0, 9);
  return (uint8_t)constrain(d, 0, 9);
}

// Text shown on the LCD (always 6 characters so old text is overwritten)
const char* categoryName(uint8_t cat) {
  switch (cat) {
    case CAT_LOW:    return "LOW   ";
    case CAT_MEDIUM: return "Medium";
    case CAT_HIGH:   return "HIGH  ";
    default:         return "Empty ";
  }
}

// Light only the LED that matches the category
void showLeds(uint8_t cat) {
  digitalWrite(LED_GREEN,  cat == CAT_LOW    ? HIGH : LOW);
  digitalWrite(LED_YELLOW, cat == CAT_MEDIUM ? HIGH : LOW);
  digitalWrite(LED_RED,    cat == CAT_HIGH   ? HIGH : LOW);
}

// Show one digit (0-9) on the 7-segment display
void showDigit(uint8_t digit) {
  if (digit > 9) return;
  for (uint8_t i = 0; i < 7; i++) {
    bool on = (DIGITS[digit] >> i) & 1;
    if (COMMON_ANODE) on = !on;
    digitalWrite(SEG_PINS[i], on ? HIGH : LOW);
  }
}

// Read the sensor and refresh the LCD, LEDs, 7-segment and Serial output
void updateAll() {
  int value = readSensor();
  uint8_t cat = getCategory(value);
  uint8_t digit = getDigit(value);

  lcd.setCursor(0, 0);
  lcd.print("Value :");
  lcd.print(value);
  lcd.print("    ");

  lcd.setCursor(0, 1);
  lcd.print("W Level :");
  lcd.print(categoryName(cat));

  showLeds(cat);
  showDigit(digit);

  Serial.print("Value: ");
  Serial.print(value);
  Serial.print("  Category: ");
  Serial.print(cat);
  Serial.print("  Digit: ");
  Serial.println(digit);
}

// ---------- Arduino entry points ----------
void setup() {
  Serial.begin(9600);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  for (uint8_t i = 0; i < 7; i++) pinMode(SEG_PINS[i], OUTPUT);

  // One-second lamp test BEFORE the LCD starts: every LED and segment on
  Serial.println("Lamp test");
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_RED, HIGH);
  showDigit(8);
  delay(1000);

  // Start the LCD. The timeout stops the sketch from freezing if the
  // LCD wiring (SDA/SCL) is wrong.
  Wire.begin();
  Wire.setWireTimeout(3000, true);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  updateAll();
  lastUpdate = millis();
}

void loop() {
  if (millis() - lastUpdate >= UPDATE_MS) {
    lastUpdate = millis();
    updateAll();
  }
}
