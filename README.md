# PXLDIGITAL_GREEN_PROJECT_RESEARCH

## Zender (ESP32/...)

### Referentie
- [Ebyte LoRa E22 Device – Specificaties en Basisgebruik](https://mischianti.org/ebyte-lora-e22-device-for-arduino-esp32-or-esp8266-specs-and-basic-usage-1/)

### GPIO

| LoRa E220 | ESP32           | Opmerkingen                         |
|-----------|----------------|-------------------------------------|
| M0        | 21 / D21        | Normale modus                        |
| M1        | 19 / D19        | Normale modus                        |
| TX        | RX2 / D16             | Pull-up weerstand 4.7kΩ–5.1kΩ        |
| RX        | TX2 / D17            | Pull-up weerstand 4.7kΩ–5.1kΩ        |
| AUX       | AUX             | Pull-up weerstand 4.7kΩ–5.1kΩ         |
| VCC       | 3.3 V           | Voeding                              |
| GND       | GND             | Aarde                                |

## 2. Ontvanger (Raspberry Pi)

### Referenties

### GPIO

| LoRa E220 | Raspberry Pi                 | Opmerkingen /dev/ttyAMA0               |
|-----------|-----------------------------|-----------------------------------------|
| VCC       | 3.3 V                        | Voeding                                 |
| GND       | GND                          | Aarde                                   |
| TX        | UART RX (GPIO 15 / ) | Pull-up weerstand 4.7kΩ–5.1kΩ            |
| RX        | UART TX (GPIO 14)            | Pull-up weerstand 4.7kΩ–5.1kΩ            |
| M0        | GND                          | Normale modus                            |
| M1        | GND                          | Normale modus                            |
| AUX       | Niet aangesloten             | Optioneel voor status             |

## Opmerkingen

1. UART-baudrate is zowel zender als ontvanger 9600 bps.  
2. Pull-up weerstanden (4.7kΩ–5.1kΩ) op de TX/RX/AUX lijnen verbeteren de signaalstabiliteit.  
3. Als M0 = GND, M1 = GND => Beide modules werken in **Normale modus** voor peer-to-peer communicatie.  
4. De AUX pin kan optioneel worden aangesloten op een GPIO om de status van de module te monitoren.
