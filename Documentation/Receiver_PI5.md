# Raspberry Pi 5

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
