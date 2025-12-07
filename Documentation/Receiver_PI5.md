## 2. Ontvanger (Raspberry Pi 5)

### GPIO (milan)

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

### Enable UART (milan)

```
cat /boot/firmware/config.txt
```
Verwacht resultaat op het einde van het .txt bestand:
```
[all]
dtparam=uart0=on
```
Deze lijn geeft aan dat UART0 enabled is op de raspberry pi via serial communicatie.
