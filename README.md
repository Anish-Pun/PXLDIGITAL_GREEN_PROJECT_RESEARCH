# PXLDIGITAL_GREEN_PROJECT_RESEARCH

## Zender ESP32

### E220-900T22D
- [Ebyte LoRa E220-900T22D – Manual + Configer Tool](https://www.cdebyte.com/products/E220-900T22D/4#Downloads/)

| LoRa E220     | Opmerkingen                         |
|---------------|-------------------------------------|
| Baud Rate     | 9600 bps                            |
| Communicatie  | UART (TX/RX)                        |
| Configuratie  | M0, M1: 3.3V / HIGH                 |
| Transmission  | M0, M1: GND / LOW                   |
| Busy status   | AUX: HIGH = Available, LOW = Busy   |
| VCC           | 3.3 V - 5 V                         |
| GND           | GND                                 |

## Zender (ESP32/...)

#### Referentie componenten
- [Ebyte LoRa E220 Device – Specificaties en Basisgebruik](https://mischianti.org/ebyte-lora-e220-llcc68-device-for-arduino-esp32-or-esp8266-specs-and-basic-use-1/)
- [U-Blox NEO 6m datasheet](https://content.u-blox.com/sites/default/files/products/documents/NEO-6_DataSheet_%28GPS.G6-HW-09005%29.pdf)
- [analog One wire thermometer - DS18B20](https://www.analog.com/media/en/technical-documentation/data-sheets/ds18b20.pdf)

### GPIO -- Long range signal -- LoRa E220

| LoRa E220 | ESP32           | Opmerkingen                         |
|-----------|-----------------|-------------------------------------|
| M0        | 21 / D21        | Normale modus (GND) /  Configuratie modus (+3.3V)                        |
| M1        | 19 / D19        | Normale modus (GND) /  Configuratie modus (+3.3V)                        |
| TX        | RX2 / D16       | Communicatie UART       |
| RX        | TX2 / D17       | Communicatie  UART       |
| AUX       | AUX             | Busy status E220         |
| VCC       | 3.3 V - 5 V     | Voeding                              |
| GND       | GND             | Aarde                                |

#### extra benodigheid:
- Lora gluestick antenna: Om het bereik van de lora te vergroten hebben wij een Lora gluestick antenna gebruikt die werkt op 840hz

#### Code Preview
```
#include "LoRa_E220.h"          // Lora header

LoRa_E220 e22ttl(&Serial2, 18, 21, 19); //  RX AUX M0 M1

static constexpr int E220_RX2_PIN = 16; // ESP32 RX2 pin (input from E220 TX)
static constexpr int E220_TX2_PIN = 17; // ESP32 TX2 pin (output to E220 RX)

// baudrate van de seriale communicatie
Serial.begin(9600);

 Serial2.begin(9600, SERIAL_8N1, E220_RX2_PIN, E220_TX2_PIN);   
    // Seriele communicatie poort openen voor de Lora module
    // Start module
e22ttl.begin();

// Read current configuration
ResponseStructContainer c = e22ttl.getConfiguration();             
    // een responte struct container c aan gemaakt en deze gelijkgesteld aan de standaard lora configuratie.
Configuration configuration = *(Configuration*) c.data;            //
Serial.println(c.status.getResponseDescription());                 //
Serial.println(c.status.code);
    
    // ---------------------- Configure module ----------------------
configuration.ADDH = 0x00;      
configuration.ADDL = 0x01;    // node address 
configuration.CHAN = 0x12;    // Channel for 868 MHz (check datasheet)
    
    // UART and Air settings
configuration.SPED.uartBaudRate = UART_BPS_9600;        // baudrate voor de UART
configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;  // 2.4 kbps
configuration.SPED.uartParity = MODE_00_8N1;            // uart pariteits bit --> geen parititeitsbit 8databits 1 stopbit
    // ----------------------------------------------------------------
configuration.
TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION; 
    // transparent transmission worgt ervoor dat we kunnen versturen en verzenden van alle e channels met hetzelfde address

    // Save configuration to module
ResponseStatus rs = e22ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE); 
    // de configuratie wordt nu opgeslagen in de lora module en slaat deze op in de responsestatus struct
Serial.println(rs.getResponseDescription());                                                    
    // de response status discriptie printen
Serial.println(rs.code);                                                              
    //de response status code printen

    // Verify configuration
c = e22ttl.getConfiguration();
configuration = *(Configuration*) c.data;
Serial.println(c.status.getResponseDescription());
Serial.println(c.status.code);
printParameters(configuration);

void loop()
{

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
if (Serial.available()) 
{
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
```

#### bevindingen:
- We hebben een eerste afstands test gedaan om de afstand van de Lora te testen. Met de huidige Gluestick antenna van 15cm hebben we een afstand van 1.1KM verbinding met onze reciever. Hierna was de powerbank uit gevallen. De Test was uitgevoerd op 4 november
- De E220 recievers kregen de berichten binnen van de andere recievers. De berichten die verstuurd waren met hetzelfde module kregen elkaar berichten binnen. dit komt omdat we een groot deel van onze configuratie vrij standaard huiden. Een van de aanpassingen die we gemaakt hebben is de addresshead die veranderd hebben. 
- Voor stabielere UART verbinding kan je een 4.7Kohm weerstand plaatsen bij de TX en RX verbinding tussen de ESP32.


#### Mogelijke fout meldingen
- Verkeerde UART Verbinding: Bij deze error build de code wel maar krijg je een error bij het uploaden van de code. Hier krijg je een foutmelding dat zegt dat er geen data voor de TX of RX lijn wordt verstuurd.
- Verkeerde AUX verbinding:  Als je de code build of upload op het bordje krijg je niet direct fout meldingen maar krijg je geen coherend signaal. 
- Verbinding met de interene UART van de ESP32 verbinding: De code build en upload ook bij deze fout. Als je debugs gebruikt voor de lora communicatie zelf kan je zien dat hier connectie error worden gegeven en dat er een grootte mogleijkeheid is dat er iets fout is met de wiring. 




### GPIO -- GPS -- NEO-6M

| NEO-6M | ESP32           | Opmerkingen                         |
|-----------|----------------|-------------------------------------|
| RX        | D15             | Communicatie  UART       |
| TX        | D4              | Communicatie  UART         |
| VCC       | 3.3 V           | Voeding                  |
| GND       | GND             | Aarde                    |

#### bevindingen:
- Als je binnen of iin een bedekte plaats zit krijg je geen data binnen. Dit komt omdat je verbinding moet maken met gps-satelliet, deze signalen kunnen niet worden ontvangen als je de gps ontvanger bedekt is. 

### GPIO -- Temperatuur -- DS18B20 temperature sensor

| DS18B20 | ESP32           | Opmerkingen                         |
|-----------|----------------|-------------------------------------|
| VCC       | 3.3 V          | Voeding      |
| DQ        | 23 / D23       |       |
| GND       | GND            | Aarde                                      |

#### bevindingen:
- Om de temperatuur sensor te verbinden moet je een weerstand van ongeveer 4.7K ohm, als dit niet het geval is zal de temperatuur sensor foute lezingen geven. De mogelijke lezing gaat waarschrijnlijk rond de -125°. 
- De maximale en minimale temperaturen die deze sensor kan opnemen zijn -55° tot 125°.  

### schematic design - breadbord

![transciever schematic](/Afbeeldingen/LDRadded.jpg)    

### bronVermelding: 
Wij hebben deze website geraadpleegd om de code te maken:
- [code voor temperatuur sensor!](https://randomnerdtutorials.com/esp32-ds18b20-temperature-arduino-ide/)
- [code voor de Lora module!](https://mischianti.org/ebyte-lora-e220-llcc68-device-for-arduino-esp32-or-esp8266-specs-and-basic-use-1/)
## 2. Ontvanger (Raspberry Pi)

### Referenties

### GPIO

| LoRa E220 | Raspberry Pi                 | Opmerkingen /dev/ttyAMA0               |
|-----------|------------------------------|----------------------------------------|
| VCC       | 3.3 V - 5 V                  | Voeding                                |
| GND       | GND                          | Aarde                                  |
| TX        | UART RX (GPIO 15 )           | Communicatie UART                      |
| RX        | UART TX (GPIO 14)            | Communicatie UART                      |
| M0        | GND  / GPIO 21               | Normale modus (GND) /  Configuratie modus (+3.3V)                           |
| M1        | GND  / GPIO 20               | Normale modus (GND) /  Configuratie modus (+3.3V)                         |
| AUX       | GPIO 18                      | Busy status E220                       |

![GPIO Raspberry Pi](/GPIO's/Raspberry-Pi-5-Pinout--189012982.jpg)    

## Opmerkingen

1. UART-baudrate is zowel zender als ontvanger 9600 bps.  
2. Pull-up weerstanden (4.7kΩ–5.1kΩ) op de TX/RX/AUX lijnen verbeteren de signaalstabiliteit. (optioneel)
3. Als M0 = GND, M1 = GND => Beide modules werken in **Normale modus** voor peer-to-peer communicatie.  
4. De AUX pin kan optioneel worden aangesloten op een GPIO om de status van de module te monitoren.
