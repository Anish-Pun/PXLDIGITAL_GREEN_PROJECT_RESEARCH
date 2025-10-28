### E220 LoRa Module Wiring

| E220 Pin | ESP32          | Arduino Nano 33 IoT | Arduino UNO                         | Raspberry Pi Pico       |
|----------|----------------|-------------------|------------------------------------|------------------------|
| M0       | 19 (or GND)    | 4 (or GND)        | 7 via 1k & 2k voltage divider (or GND) | 10 (or GND)           |
| M1       | 21 (or GND)    | 6 (or GND)        | 6 via 1k & 2k voltage divider (or GND) | 11 (or GND)           |
| TX       | TX2 (PullUP)   | TX1 (PullUP)      | 4 (PullUP)                         | 8 (PullUP)            |
| RX       | RX2 (PullUP)   | RX1 (PullUP)      | 5 via 1k & 2k voltage divider (PullUP) | 9 (PullUP)            |
| AUX      | 18 (PullUP)    | 2 (PullUP)        | 3 (PullUP)                         | 2 (PullUP)            |
| VCC      | 3.3V / 5V      | 3.3V / 5V         | 3.3V / 5V                          | 3.3V / 5V             |
| GND      | GND            | GND               | GND                                | GND                   |

**Notes:**

- ESP32, Arduino Nano 33 IoT, and Pico use **3.3V logic** → no voltage dividers needed.  
- Arduino UNO is **5V logic**, so M0, M1, RX pins moeten **voltage dividers** (1k & 2k).  

Works: sending from uno to esp32