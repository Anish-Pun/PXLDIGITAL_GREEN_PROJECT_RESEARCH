# FIXED_TRANSMISSION & E220-900T22D

Dit document beschrijft de configuratie, werking en toepassing van de EBYTE LoRa E220-module in combinatie met microcontrollers 
met de Wemos D1 Mini Pro als verzender en ESP32 als ontvanger. 
De focus ligt op Fixed Transmission binnen het 868 MHz ISM-spectrum. Bij Fixed Transmission wordt elk bericht verzonden met een expliciet bestemmingsadres en kanaal. 

## GPIO -- Long range signal -- E220-900T22D (milan)

| LoRa E220 | ESP32           | Wemos D1 Mini Pro   | Opmerkingen                         |
|-----------|-----------------|---------------------|-------------------------------------|
| M0        | 21 / D21        | D7                  | Normale modus (GND) /  Configuratie modus (+3.3V)                        |
| M1        | 19 / D19        | D6                  | Normale modus (GND) /  Configuratie modus (+3.3V)                        |
| TX        | RX2 / D16       | D2                  | Communicatie UART        |
| RX        | TX2 / D17       | D1                  | Communicatie  UART       |
| AUX       | AUX: D22/23     | D5                  | Busy status E220         |
| VCC       | 3.3 V - 5 V     | 5 V                 | Voeding                  |
| GND       | GND             | GND                 | Aarde                    |

Voorbeeldcode met FT_FIXED_TRANSMISSION zie: ```/Sender/Wemos_sendFixedTransmission.ino``` & ```Receiver/ESP32_receiveFixedTransmission/ESP32_receiveFixedTransmission.ino```

## Software

De software initialiseert de UART-communicatie op 9600 bps en configureert de LoRa-module via AT-achtige commando’s die door de library worden afgehandeld.

Softwarebenodigdheden:
- Arduino IDE / Visual studio code (PlatformIO)
- LoRa_E220 Arduino library
- SoftwareSerial (voor Wemos D1 Mini Pro)

### Adressering

Binnen het LoRa E220-systeem beschikt elke module over een uniek logisch adres, bestaande uit twee delen:
- ADDH (Address High byte) +
- ADDL (Address Low byte)

Samen vormen deze bytes het node-adres ADDH + ADDL waarmee modules elkaar kunnen identificeren binnen een netwerk.

#### Adres van de zender

In deze configuratie werkt de Wemos D1 Mini Pro als zender:
- configuration.ADDH = 0x00;
- configuration.ADDL = <b>0x03</b> ;

Het volledige adres van de zender is daarmee 0x00<b>03</b>. Dit adres wordt intern gebruikt door de LoRa-module om de herkomst van verzonden berichten te identificeren. 
Het zenderadres wordt niet meegegeven bij fixed transmissie, maar moet wel uniek zijn binnen het netwerk.

#### Adres van de ontvanger (Destination)

De ontvanger is geconfigureerd met:
- configuration.ADDH = 0x00 ;
- configuration.ADDL = <b>0x02</b> ;

Dit resulteert in het adres 0x00<b>02</b>. In de code van de verzendert wordt dit aangeduid met:

#define DESTINATION_ADDL 2

Bij Fixed Transmission wordt het bestemmingsadres expliciet opgenomen in elk verzonden bericht. Hierdoor accepteert uitsluitend de LoRa-module met adres 0x0002 het bericht; andere modules op hetzelfde kanaal negeren deze transmissie.

#### Relatie tussen zender en ontvanger

Samengevat:
- Zenderadres: 3 (0x0003)
- Bestemmingsadres: 2 (0x0002)

De zender (3) verstuurt data gericht naar de ontvanger (2) op een vast kanaal. 
Deze peer-to-peer-communicatie voorkomt dat andere nodes binnen hetzelfde frequentiekanaal de data verwerken.

Deze adresseringsmethode is essentieel voor schaalbare LoRa-netwerken waarin meerdere nodes gelijktijdig actief zijn zonder onderlinge interferentie.
Dat betekent dat enkel de ontvanger met een specifiek adres het bericht kan ontvangen en dus de andere apparaten geen tijd verspillen met onnodige berichten te ontvangen.

### Kanaal en frequentie
Men gebruikt in de code:
- Kanaal: ```configuration.CHAN = 0x12```
- Frequentieband =  basisfrequentie & kanaal ```850(.125) MHz + 0x12 MHz = 868(.125) MHz``` 

Het kanaal bepaalt de exacte frequentie binnen de toegestane ISM-band. 
Bij Fixed Transmission kan de channel voor een verzender en onvanger verschillen.

De ontvanger kan bijvoorbeeld ```configuration.CHAN = 0x18;``` worden, terwijl de verzender 0x12 blijft.

Men moet deze code in de verzender als gevolg ook veranderen: </br>
``` 
    e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 0x18, msg); 
```
De functie ```e220ttl.sendFixedMessage()``` verzendt een bericht naar module met een specifiek adres en kanaal.

### Andere configuratiemogelijkheden

- Transmission Mode: ```FT_FIXED_TRANSMISSION```
    Ervoor zorgen dat Fixed Transmission geactiveert wordt in de module
- RSSI: ```DISABLED```
    RSSI (Received Signal Strength Indicator) voegt extra informatie toe aan ontvangen berichten over de signaalsterkte. In deze configuratie is RSSI uitgeschakeld omdat:
    - De focus ligt op basiscommunicatie en stabiliteit.
    - Extra data-overhead wordt vermeden.
    - Indien signaalkwaliteit of bereik in detail moet worden geëvalueerd, kan RSSI worden ingeschakeld voor onderzoeksdoeleinden.
- LBT (Listen Before Talk): ```DISABLED```
    LBT zorgt ervoor dat de module eerst controleert of het kanaal vrij is voordat er wordt uitgezonden. In dit onderzoek is LBT uitgeschakeld omdat:
    - Het netwerk bestaat uit een beperkt aantal bekende nodes (4-6 verzenders, 1 ontvanger).
    - Kanaalcongestie niet wordt verwacht.
Continue en voorspelbare transmissie gewenst is.
- WOR-periode: ```2000 ms```
    WOR (Wake-On-Radio) bepaalt het interval waarin de module uit een laagvermogensmodus ontwaakt om te controleren op inkomende data.
    WOR is met name enkel relevant voor batterijgevoede ontvangers en sinds enkel verzenders batterijgevoed worden kan dit standaard blijven.

### Na configuratie

Na configuratie permanent opgeslagen wordt in het interne geheugen van de module wordt vervolgens:

1. bij het opstarten een testbericht verzonden.
2. Worden elke 5 seconden berichten verzonden met een tijdstempel.
    Interesant voor te zien of er berichten verloren geraken.
3. Kunnen handmatige berichten via de seriële monitor worden ingevoerd en verzonden.

