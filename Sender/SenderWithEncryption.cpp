/*
 * LoRa E220
 * Start device or reset to send a message
 * https://mischianti.org
 *
 * E220       ----- esp32
 * M0         ----- GND / 21
 * M1         ----- GND
 * TX         ----- RX2 (PullUP)
 * RX         ----- TX2 (PullUP)
 * AUX        ----- D18 (Pullup)
 * VCC        ----- 3.3v
 * GND        ----- GND
 */
#include "Arduino.h"
#define FREQUENCY_868
#include "LoRa_E220.h"
#include "TinyGPSPlus.h"
#include "AESLib.h"
#include "esp_sleep.h"

#define NODE_ID 1 // Unique node ID

// AES key for this node (16 bytes)
byte aes_key[16] = {0x89,0x04,0x47,0xad,0x1b,0xd4,0xf9,0x20,0x03,0xc3,0x56,0x0a,0x86,0x09,0xac,0xf6}; //Temporary key for testing 
byte aes_iv[16];  // IV is random for each message
 
// ---------- esp32 pins --------------

LoRa_E220 e22ttl(&Serial2, 18, 21, 19); //  RX AUX M0 M1
TinyGPSPlus gps;
HardwareSerial gpsSerial(1); // Use UART1 for GPS

// Note: Wire E220 TX -> ESP32 GPIO16 (RX2), E220 RX -> ESP32 GPIO17 (TX2)
static constexpr int E220_RX2_PIN = 16; // ESP32 RX2 pin (input from E220 TX)
static constexpr int E220_TX2_PIN = 17; // ESP32 TX2 pin (output to E220 RX)
static constexpr int GPS_RX_PIN   = 4;  // ESP32 RX (input from GPS TX)
static constexpr int GPS_TX_PIN   = 2;  // ESP32 TX (output to GPS RX)

// Send interval
static constexpr unsigned long SEND_INTERVAL_MS = 60000; // 60 seconds
static unsigned long lastSendMs = 0;
 
// AES encryption
AESLib aes;

//LoRa_E22 e22ttl(&Serial2, 22, 4, 18, 21, 19, UART_BPS_RATE_9600); //  esp32 RX <-- e22 TX, esp32 TX --> e22 RX AUX M0 M1
// -------------------------------------
 
void printParameters(struct Configuration configuration);
void printModuleInformation(struct ModuleInformation moduleInformation);
void generateRandomIV(byte *iv);

void setup() {
    Serial.begin(9600);
    while(!Serial) {};
    delay(500);

    Serial.println("Starting E220 configuration...");

    // Initialize GPS serial
    gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Serial.println("GPS serial started");

    // Ensure UART2 uses the correct pins to talk to the E220
    Serial2.begin(9600, SERIAL_8N1, E220_RX2_PIN, E220_TX2_PIN);

    // Start module
    e22ttl.begin();

    // Read current configuration
    ResponseStructContainer c = e22ttl.getConfiguration();
    Configuration configuration = *(Configuration*) c.data;
    Serial.println(c.status.getResponseDescription());
    Serial.println(c.status.code);

    // ---------------------- Configure module ----------------------
    configuration.ADDH = 0x00;
    configuration.ADDL = 0x01;    // Example node address
    configuration.CHAN = 0x12;    // Channel for 868 MHz (check datasheet)
    
    // UART and Air settings
    configuration.SPED.uartBaudRate = UART_BPS_9600;
    configuration.SPED.airDataRate = AIR_DATA_RATE_010_24; // 2.4 kbps
    configuration.SPED.uartParity = MODE_00_8N1;
    // ----------------------------------------------------------------
    configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;

    // Save configuration to module
    ResponseStatus rs = e22ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
    Serial.println(rs.getResponseDescription());
    Serial.println(rs.code);

    // Verify configuration
    c = e22ttl.getConfiguration();
    configuration = *(Configuration*) c.data;
    Serial.println(c.status.getResponseDescription());
    Serial.println(c.status.code);
    printParameters(configuration);

    Serial.println("Node ready! Sending GPS data every 60s...");
}

void loop() {
  // Continuously read GPS data
  /*
  while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
  }
  */

  unsigned long now = millis();
  if (now - lastSendMs >= SEND_INTERVAL_MS) {
    lastSendMs = now;

    // GPS duty cycle: read for max 1s
    unsigned long start = millis();
    while (millis() - start < 1000) {
      while (gpsSerial.available() > 0) gps.encode(gpsSerial.read());
    }

    // Prepare payload
    String payload;
    char buf[256];

    if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid()) {
      char timestr[20];
      snprintf(timestr, sizeof(timestr), "%04d%02d%02d%02d%02d%02d", gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());

      snprintf(buf, sizeof(buf), "{\"node_id\":%d,\"lat\":%.6f,\"lon\":%.6f,\"alt\":%.2f,\"sats\":%d,\"ts\":%s}", NODE_ID, gps.location.lat(), gps.location.lng(), gps.altitude.meters(), gps.satellites.value(), timestr);
    } else if (gps.location.isValid()) {
      snprintf(buf, sizeof(buf), "{\"node_id\":%d,\"lat\":%.6f,\"lon\":%.6f,\"alt\":%.2f,\"sats\":%d,\"ts\":null}", NODE_ID, gps.location.lat(), gps.location.lng(), gps.altitude.meters(), gps.satellites.value());
    } else {
      snprintf(buf, sizeof(buf), "{\"node_id\":%d,\"status\":\"no_gps\",\"ts\":null}", NODE_ID);
    }

    payload = String(buf);

    // Encrypt payload
    byte payload_bytes[256];
    memcpy(payload_bytes, payload.c_str(), payload.length());
    generateRandomIV(aes_iv);

    byte encrypted[256];
    uint16_t enc_len = aes.encrypt(payload_bytes, payload.length(), encrypted, aes_key, 128, aes_iv);

    // Combine IV + ciphertext
    byte packet[272];
    memcpy(packet, aes_iv, 16);
    memcpy(packet + 16, encrypted, enc_len);
    uint16_t packet_len = 16 + enc_len;

    //DEBUG: print encrypted bytes
    Serial.print("IV (hex): ");
    for (int i = 0; i < 16; i++) {
      if (aes_iv[i] < 0x10) Serial.print('0');
      Serial.print(aes_iv[i], HEX);
      Serial.print(' ');
    }
    Serial.println();

    Serial.print("Encrypted bytes (hex): ");
    for (int i = 0; i < enc_len; ++i) {
      if (encrypted[i] < 0x10) Serial.print('0');
      Serial.print(encrypted[i], HEX);
      Serial.print(' ');
    }
    Serial.println();

    ResponseStatus s = e22ttl.sendMessage((char * ) packet);
    //ResponseStatus s = e22ttl.sendFixedMessage(0x00, 0xFF, 0x12, (char * ) packet, packet_len);

    Serial.print("Send: ");
    Serial.println(payload);
    Serial.print("Result: ");
    Serial.println(s.getResponseDescription());

    esp_sleep_enable_timer_wakeup(SEND_INTERVAL_MS * 1000); // microseconds
    Serial.println("Entering light sleep...");
    esp_light_sleep_start();
  }

  // If something available
  if (e22ttl.available() > 1) {
    // read the String message
    ResponseContainer rc = e22ttl.receiveMessage();
    // Is something goes wrong print error
    if (rc.status.code != 1) {
      rc.status.getResponseDescription();
    } else {
      // Print the data received
      Serial.println(rc.data);
    }
  }
  if (Serial.available()) {
    String input = Serial.readString();
    e22ttl.sendMessage(input);
  }
}

void printParameters(struct Configuration configuration) {
    Serial.println("----------------------------------------");
 
    Serial.print(F("HEAD : "));  Serial.print(configuration.COMMAND, HEX);Serial.print(" ");Serial.print(configuration.STARTING_ADDRESS, HEX);Serial.print(" ");Serial.println(configuration.LENGHT, HEX);
    Serial.println(F(" "));
    Serial.print(F("AddH : "));  Serial.println(configuration.ADDH, HEX);
    Serial.print(F("AddL : "));  Serial.println(configuration.ADDL, HEX);
    Serial.println(F(" "));
    Serial.print(F("Chan : "));  Serial.print(configuration.CHAN, DEC); Serial.print(" -> "); Serial.println(configuration.getChannelDescription());
    Serial.println(F(" "));
    Serial.print(F("SpeedParityBit     : "));  Serial.print(configuration.SPED.uartParity, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTParityDescription());
    Serial.print(F("SpeedUARTDatte     : "));  Serial.print(configuration.SPED.uartBaudRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTBaudRateDescription());
    Serial.print(F("SpeedAirDataRate   : "));  Serial.print(configuration.SPED.airDataRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getAirDataRateDescription());
    Serial.println(F(" "));
    Serial.print(F("OptionSubPacketSett: "));  Serial.print(configuration.OPTION.subPacketSetting, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getSubPacketSetting());
    Serial.print(F("OptionTranPower    : "));  Serial.print(configuration.OPTION.transmissionPower, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getTransmissionPowerDescription());
    Serial.print(F("OptionRSSIAmbientNo: "));  Serial.print(configuration.OPTION.RSSIAmbientNoise, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getRSSIAmbientNoiseEnable());
    Serial.println(F(" "));
    Serial.print(F("TransModeWORPeriod : "));  Serial.print(configuration.TRANSMISSION_MODE.WORPeriod, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getWORPeriodByParamsDescription());
    Serial.print(F("TransModeEnableLBT : "));  Serial.print(configuration.TRANSMISSION_MODE.enableLBT, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getLBTEnableByteDescription());
    Serial.print(F("TransModeEnableRSSI: "));  Serial.print(configuration.TRANSMISSION_MODE.enableRSSI, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getRSSIEnableByteDescription());
    Serial.print(F("TransModeFixedTrans: "));  Serial.print(configuration.TRANSMISSION_MODE.fixedTransmission, BIN);Serial.print(" -> "); Serial.println(configuration.TRANSMISSION_MODE.getFixedTransmissionDescription());
 
 
    Serial.println("----------------------------------------");
}
void printModuleInformation(struct ModuleInformation moduleInformation) {
    Serial.println("----------------------------------------");
    Serial.print(F("HEAD: "));  Serial.print(moduleInformation.COMMAND, HEX);Serial.print(" ");Serial.print(moduleInformation.STARTING_ADDRESS, HEX);Serial.print(" ");Serial.println(moduleInformation.LENGHT, DEC);
 
    Serial.print(F("Model no.: "));  Serial.println(moduleInformation.model, HEX);
    Serial.print(F("Version  : "));  Serial.println(moduleInformation.version, HEX);
    Serial.print(F("Features : "));  Serial.println(moduleInformation.features, HEX);
    Serial.println("----------------------------------------");
 
}

void generateRandomIV(byte *iv) {
  for (int i = 0; i < 16; i++) iv[i] = (byte)esp_random();
}