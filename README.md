# PXLDIGITAL_GREEN_PROJECT_RESEARCH

## Sender

### Sources
- [Ebyte LoRa E22 Device – Specs and Basic Usage](https://mischianti.org/ebyte-lora-e22-device-for-arduino-esp32-or-esp8266-specs-and-basic-usage-1/
### GPIO's

LoRa E220       → ESP32  
M0              → GND  
M1              → GND  
TX              → RX2 (PullUP 4.7kE/5.1kE)
RX              → TX2 (PullUP 4.7kE/5.1kE)
AUX             → Not connected
VCC             → 3.3 V
GND             → GND

## Receiver

### Sources

### GPIO's

LoRa E220       → Raspberry Pi  
VCC             → 3.3 V 
GND             → GND  
TX              → Pi UART RX pin (e.g. GPIO 15 or /dev/ttyAMA0)  
RX              → Pi UART TX pin (e.g. GPIO 14)  
M0              → GND  
M1              → GND  
AUX             → Not connected

## Opmerkingen

1. UART-baudrate is zowel zender als ontvanger 9600 bps.  
2. Pull-up weerstanden (4.7kΩ–5.1kΩ) op de TX/RX lijnen verbeteren de signaalstabiliteit.  
3. Als M0 = GND, M1 = GND => Beide modules werken in **Normale modus** voor peer-to-peer communicatie.  
4. De AUX pin kan optioneel worden aangesloten op een GPIO om de status van de module te monitoren.
