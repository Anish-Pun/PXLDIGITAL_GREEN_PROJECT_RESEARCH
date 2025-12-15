/*
 * LoRa E220
 * E220       ----- esp32
 * M0         ----- GND / 21
 * M1         ----- GND
 * TX         ----- RX2 (PullUP)
 * RX         ----- TX2 (PullUP)
 * AUX        ----- D18 (Pullup)
 * VCC        ----- 3.3v
 * GND        ----- GND
 * 
 * Neo M6     ----- esp32
 * RX         ----- D15
 * TX         ----- D2
 * VCC        ----- 3.3V
 * GND        ----- GND
 * 
 * DS18B20    ----- esp32
 * DQ         ----- D23
 * VCC        ----- 3.3V
 * GND        ----- GND
 */

#include "Arduino.h"            // header voor arduino combinatie
#define FREQUENCY_868           // lora frequentie
#include "LoRa_E220.h"          // Lora header
#include "TinyGPSPlus.h"        // GPS header
#include "esp_sleep.h"          // ESP sleep modes header
#include <OneWire.h>            // temperatuur header
#include <DallasTemperature.h>  // temperatuur header

// ---------- LDR pins ----------------

const int ldrPin = 34;      //ADC pin voor LDR
const int LED = 25;         //GPIO pin voor LED
const int threshold = 200; //drempelwaarde LDR led aan/uit
 
// ---------- esp32 pins --------------

LoRa_E220 e22ttl(&Serial2, 18, 21, 19); //  RX AUX M0 M1
TinyGPSPlus gps;
HardwareSerial gpsSerial(1); // Use UART1 for GPS

// Note: Wire E220 TX -> ESP32 GPIO16 (RX2), E220 RX -> ESP32 GPIO17 (TX2)
static constexpr int E220_RX2_PIN = 16; // ESP32 RX2 pin (input from E220 TX)
static constexpr int E220_TX2_PIN = 17; // ESP32 TX2 pin (output to E220 RX)
static constexpr int GPS_RX_PIN   = 4;  // ESP32 RX (input from GPS TX)
static constexpr int GPS_TX_PIN   = 5;  // ESP32 TX (output to GPS RX)
// Send interval
static constexpr unsigned long SEND_INTERVAL_MS = 5000; // 5 seconds
static unsigned long lastSendMs = 0;
 
// GPIO where the DS18B20 is connected to
const int oneWireBus = 23;    // kan veranderen van bordje tot bordje


// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// -------------------------------------

/// @brief Deze functie bevat printsatementst die alle informatie printen over de configuratie van de Lora module
/// @param configuration De lora configuratie waarvan de data geprint moet worden
void printParameters(struct Configuration configuration);   
/// @brief print functie voor de temperatuur inputs te printen 
void printGetTemps();

void setup() {
    // baudrate van de seriale communicatie
    Serial.begin(9600);
    while(!Serial) {};
    delay(500);


    // Start the DS18B20 sensor 
    sensors.begin();    // deze sensor gebruikt geen uart hierdoor moet  ook geen seriele communicatie poort openen

    
    // Initialize GPS serial
    gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN); // seriele comunicatie poort openen voor de GPS module
    Serial.println("GPS serial started");

    Serial.println("Starting E220 configuration...");
    // Ensure UART2 uses the correct pins to talk to the E220
    Serial2.begin(9600, SERIAL_8N1, E220_RX2_PIN, E220_TX2_PIN); // Seriele communicatie poort openen voor de Lora module
    // Start module
    e22ttl.begin();

    // Read current configuration
    ResponseStructContainer c = e22ttl.getConfiguration();             // een responte struct container c aan gemaakt en deze gelijkgesteld aan de standaard lora configuratie.
    Configuration configuration = *(Configuration*) c.data;            //
    Serial.println(c.status.getResponseDescription());                 //
    Serial.println(c.status.code);                                     //

    // ---------------------- Configure module ----------------------
    configuration.ADDH = 0x00;      
    configuration.ADDL = 0x01;    // node address 
    configuration.CHAN = 0x12;    // Channel for 868 MHz (check datasheet)
    
    // UART and Air settings
    configuration.SPED.uartBaudRate = UART_BPS_9600;        // baudrate voor de UART
    configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;  // 2.4 kbps
    configuration.SPED.uartParity = MODE_00_8N1;            // uart pariteits bit --> geen parititeitsbit 8databits 1 stopbit
  
    // Zendvermogen voor EU-wetgeving
    configuration.OPTION.transmissionPower = POWER_10; // TX ≈10 dBm, +5 dBi antenne → EIRP legaal
    // ----------------------------------------------------------------
    configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION; // transparent transmission worgt ervoor dat we kunnen versturen en verzenden van alle e channels met hetzelfde address

    // Save configuration to module
    ResponseStatus rs = e22ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE); // de configuratie wordt nu opgeslagen in de lora module en slaat deze op in de responsestatus struct
    Serial.println(rs.getResponseDescription());                                        // de response status discriptie printen
    Serial.println(rs.code);                                                            //de response status code printen

    // Verify configuration
    c = e22ttl.getConfiguration();
    configuration = *(Configuration*) c.data;
    Serial.println(c.status.getResponseDescription());
    Serial.println(c.status.code);
    printParameters(configuration);

    Serial.println("Hi, I'm going to send message!");                                   // module klaar maken om dingen te vezenden
    Serial.println("Will send real GPS data every 5s.");

    pinMode(LED, OUTPUT);
}

void loop() {

    unsigned long now = millis();                                                       // variable now gemaakt dat de tijd tussen de berichten bijhoudt, deze variable zorgt er dan ook voor de er om de 5 seconde een vericht verstuurd kan worden

    if (now - lastSendMs >= SEND_INTERVAL_MS) {                                         // If functie die checkt als de interval tijd  is verstreken
        lastSendMs = now;

        // Lees LDR & stuur LED aan
        int ldrValue = analogRead(ldrPin);
        bool zonOp = (ldrValue > threshold);

        if (zonOp){
            digitalWrite(LED, LOW); // Nachtlamp uit bij daglicht
        } else {
            digitalWrite(LED, HIGH); // Nachtlamp aan bij duisternis
        }

        // GPS duty cycle: read for max 1s
        unsigned long start = millis();                                                // variable die ervoor zorgt dat er iedere seconde wordt gecheckt als gps data gevonden wordt op niet 
        while (millis() - start < 1000) {                                       
            while (gpsSerial.available() > 0) gps.encode(gpsSerial.read());
        }

        if (gps.location.isValid() && gps.location.age() < 2000) {              // als de locatie gevonden worden door de GPS module dat minder oud is dan 2 seconde
        double lat = gps.location.lat();                                        // Latitude van de huidige locatie
        double lon = gps.location.lng();                                        // longitude van de huidige locatie
        double alt = gps.altitude.meters();                                     // altitude in meters 
        int sats = gps.satellites.value();                                      // hoeveel heden gevonden satelieten

        sensors.requestTemperatures();
        float temparatureC = sensors.getTempCByIndex(0);

        const char* lightStr = zonOp ? "Het is licht buiten, nachtlamp staat uit" : "Het is donker buiten, nachtlamp staat aan";
        char buf[256];
        snprintf(buf, sizeof(buf), "{\"device\":\"ESP32-GPS_Maxime\",\"lat\":%.6f,\"lon\":%.6f,\"alt\":%.2f,\"sats\":%d,\"ts\":%lu,\"temp\":%.2f,\"light\":\"%s\"}", lat, lon, alt, sats, now, temparatureC ,lightStr); // printe message met de gps data 

        String payload = String(buf) + "\n";                    // newline-delimited for easy parsing
        ResponseStatus s = e22ttl.sendMessage(payload);         // variable aan maken van de sting buffer van de gps data
        Serial.print("Send: ");                                 
        Serial.println(buf);                                    // ruwe data van de GPS
        Serial.print("Result: ");
        Serial.println(s.getResponseDescription());             // ...
        printGetTemps();                                        // printen van temperatuur readings
        } 
        else 
        {
            Serial.println("Invalid GPS data");
            printGetTemps();                                    // printen van temperatuur readings
        }
        esp_sleep_enable_timer_wakeup(SEND_INTERVAL_MS * 1000); // send_ interval_ms is 5000 maar de parameter van de wakeup is in US dus * 1000
        Serial.println("Entering light sleep...");
        esp_light_sleep_start();                                // terug op starten na de sleep enable van hiervoor
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

void printGetTemps(){
    sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  Serial.print(temperatureF);
  Serial.println("ºF");  
}
