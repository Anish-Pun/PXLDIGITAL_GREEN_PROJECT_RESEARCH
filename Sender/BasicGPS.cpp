#include <Arduino.h>
#include <TinyGPSPlus.h>
static const int RXPin = 16;           // GPS TX -> ESP32 RX2 
static const int TXPin = 17;           // GPS RX <- ESP32 TX2 
static const uint32_t GPSBaud = 9600;   // NEO-6M default

HardwareSerial GPS(2);
TinyGPSPlus gps;

// LED to indicate program is running
static const int LED_PIN = 2;

void setup() {
  Serial.begin(115200);
  delay(200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  GPS.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
}

void loop() {
  // Feed TinyGPS++ parser
  while (GPS.available()) gps.encode(GPS.read());

  static uint32_t lastPrint = 0;
  static uint32_t lastBlink = 0;
  static bool ledState = false;
  const uint32_t now = millis();

  // Blink LED every 500 ms so you can see the program is running
  if (now - lastBlink >= 500) {
    lastBlink = now;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
  }

  // Print once per second
  if (now - lastPrint >= 1000) {
    lastPrint = now;
    if (gps.location.isValid()) {
      double lon = gps.location.lng();
      double lat = gps.location.lat();
      // Print latitude then longitude in degrees with N/E suffix (4 decimals)
      double lat_abs = fabs(lat);
      double lon_abs = fabs(lon);
      char lat_dir = lat >= 0.0 ? 'N' : 'S';
      char lon_dir = lon >= 0.0 ? 'E' : 'W';
      Serial.printf("%.4f° %c, %.4f° %c\n", lat_abs, lat_dir, lon_abs, lon_dir);
    } else {
      // Print NoFix plus satellite count and hdop so you can judge reception
      unsigned int sats = gps.satellites.isValid() ? gps.satellites.value() : 0;
      double hdop = gps.hdop.isValid() ? gps.hdop.hdop() : 0.0;
      Serial.printf("NoFix sats=%u hdop=%.1f\n", sats, hdop);
    }
  }
}
