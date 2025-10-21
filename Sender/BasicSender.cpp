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
#include "esp_system.h"  // for esp_random() to seed RNG
 
// ---------- esp32 pins --------------

LoRa_E220 e22ttl(&Serial2, 18, 21, 19); //  RX AUX M0 M1
// Note: Wire E220 TX -> ESP32 GPIO16 (RX2), E220 RX -> ESP32 GPIO17 (TX2)
static constexpr int E220_RX2_PIN = 16; // ESP32 RX2 pin (input from E220 TX)
static constexpr int E220_TX2_PIN = 17; // ESP32 TX2 pin (output to E220 RX)
// Hasselt reference location
static constexpr double BASE_LAT = 50.93069;
static constexpr double BASE_LON = 5.33749;
// Send interval
static constexpr unsigned long SEND_INTERVAL_MS = 5000; // 5 seconds
static unsigned long lastSendMs = 0;
 
//LoRa_E22 e22ttl(&Serial2, 22, 4, 18, 21, 19, UART_BPS_RATE_9600); //  esp32 RX <-- e22 TX, esp32 TX --> e22 RX AUX M0 M1
// -------------------------------------
 
void printParameters(struct Configuration configuration);
void printModuleInformation(struct ModuleInformation moduleInformation);

void setup() {
    Serial.begin(9600);
    while(!Serial) {};
    delay(500);

    Serial.println("Starting E220 configuration...");

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
    Serial.println("Hi, I'm going to send message!");
        Serial.println("Will send fake GPS near Hasselt every 5s.");
 
        // Seed RNG for coordinate jitter
        randomSeed(esp_random());
}

void loop() {
    // Periodic sender: fake GPS near Hasselt every 5 seconds
    unsigned long now = millis();
    if (now - lastSendMs >= SEND_INTERVAL_MS) {
        lastSendMs = now;
        // +/- 0.01 degrees (~1.1 km latitude, ~0.7 km longitude at this latitude)
        double latOffset = (double)random(-1000, 1001) / 100000.0; // -0.01000 .. +0.01000
        double lonOffset = (double)random(-1000, 1001) / 100000.0;
        double lat = BASE_LAT + latOffset;
        double lon = BASE_LON + lonOffset;

        char buf[160];
        // JSON with timestamp
        snprintf(buf, sizeof(buf), "{\"device\":\"Anish-Sending\",\"lat\":%.6f,\"lon\":%.6f,\"ts\":%lu}", lat, lon, now);

        String payload = String(buf) + "\n"; // newline-delimited for easy parsing
        ResponseStatus s = e22ttl.sendMessage(payload);
        Serial.print("Send: ");
        Serial.println(buf);
        Serial.print("Result: ");
        Serial.println(s.getResponseDescription());
    }

  // If something available
  if (e22ttl.available()>1) {
      // read the String message
    ResponseContainer rc = e22ttl.receiveMessage();
    // Is something goes wrong print error
    if (rc.status.code!=1){
        rc.status.getResponseDescription();
    }else{
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