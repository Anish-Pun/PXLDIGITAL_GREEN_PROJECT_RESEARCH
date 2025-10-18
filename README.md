# PXLDIGITAL_GREEN_PROJECT_RESEARCH

## Zender (ESP32/...)

### Referentie
- [Ebyte LoRa E22 Device – Specificaties en Basisgebruik](https://mischianti.org/ebyte-lora-e22-device-for-arduino-esp32-or-esp8266-specs-and-basic-usage-1/)

### GPIO

| LoRa E220 | ESP32           | Opmerkingen                         |
|-----------|----------------|-------------------------------------|
| M0        | GND            | Normale modus                        |
| M1        | GND            | Normale modus                        |
| TX        | RX2             | Pull-up weerstand 4.7kΩ–5.1kΩ        |
| RX        | TX2             | Pull-up weerstand 4.7kΩ–5.1kΩ        |
| AUX       | Niet aangesloten| Optioneel voor “busy” status         |
| VCC       | 3.3 V           | Voeding                              |
| GND       | GND             | Aarde                                |

## 2. Ontvanger (Raspberry Pi)

### Referenties

### GPIO

| LoRa E220 | Raspberry Pi                 | Opmerkingen                             |
|-----------|-----------------------------|-----------------------------------------|
| VCC       | 3.3 V                        | Voeding                                 |
| GND       | GND                          | Aarde                                   |
| TX        | UART RX (GPIO 15 / /dev/ttyAMA0) | Data van LoRa module naar Pi            |
| RX        | UART TX (GPIO 14)            | Data van Pi naar LoRa module            |
| M0        | GND                          | Normale modus                            |
| M1        | GND                          | Normale modus                            |
| AUX       | Niet aangesloten             | Optioneel voor “busy” status             |

## Opmerkingen

1. UART-baudrate is zowel zender als ontvanger 9600 bps.  
2. Pull-up weerstanden (4.7kΩ–5.1kΩ) op de TX/RX lijnen verbeteren de signaalstabiliteit.  
3. Als M0 = GND, M1 = GND => Beide modules werken in **Normale modus** voor peer-to-peer communicatie.  
4. De AUX pin kan optioneel worden aangesloten op een GPIO om de status van de module te monitoren.
