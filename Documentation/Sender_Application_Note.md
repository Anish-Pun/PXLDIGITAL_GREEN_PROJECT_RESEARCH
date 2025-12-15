## 2. Materiaal en methode
### Hardware
Het systeem is opgebouwd uit de volgende componenten:
- **ESP32 Dev Board** – centrale microcontroller voor het uitlezen van sensoren, GPS-gegevens en LoRa-communicatie.
- **LoRa E220 Module (868/915 MHz, 22 dBm, SMA Male)** – draadloze transmissie van sensordata over lange afstand.
- **LoRa Antenne (868/915 MHz, Omni-Directional, 15 cm)** – SMA Male connector, gebruikt voor LoRa transmissie.
- **Neo-6M / NEO-M8N GPS Module** – ontvangst van locatiegegevens.
- **DS18B20 Temperatuursensor (Waterproof, 100 cm kabel)** – digitale temperatuursensor voor externe omgevingstemperatuur.
- **4.7 kΩ pull-up weerstand** – gebruikt voor correcte OneWire-communicatie met de DS18B20 temperatuursensor.
- **Lithium-ion batterij 103450 (3.7 V, 2000 mAh)** – voeding voor het systeem.
- **TP4056 USB Type-C Batterijlader** – laadmodule voor de Li-ion batterij.
- **XL6019 DC-DC Step-Up Converter (3.3V–35V → 5V/6V/9V/12V/24V)** – omzetting van batterijspanning van 3.7V naar stabiele 5V voor de ESP32.
- **Breadboard en jumper wires** – voor tijdelijke verbindingen tijdens prototyping.
- **Soldeerplaatje en kabels** – voor vaste verbindingen.
- **Plastic behuizing** – bescherming van de elektronica en sensoren.

### Software
Het systeem wordt ondersteund door de volgende software-omgeving en bibliotheken:

- **Ontwikkelomgeving:**  
  - Visual Studio Code met **PlatformIO** (voor projectbeheer, build en upload naar ESP32)
  - Arduino IDE

- **Bibliotheken:**  
  - `LoRa_E220` – voor communicatie met de E220 LoRa-module
  - `TinyGPSPlus` – uitlezen en verwerken van GPS-data
  - `DallasTemperature` – uitlezen van DS18B20 sensoren
  - `esp_sleep.h` – voor het beheren van **light sleep** en/of **deep sleep** om energie te besparen op de ESP32

- **Overige software tools:**  
  - **Python**
  - **Node-RED**

### Integratie
- LoRa-module configureert zender via UART2  
- GPS-data wordt gelezen via UART1
- Temperatuursensor (DS18B20) via OneWire  
- ESP32 schakelt tussen actieve modus en light-sleep

### Methode
Sensorwaarden worden gelezen en verwerkt in JSON-formaat.  
De JSON-payload bevat GPS-locatie, satellietinformatie, temperatuur en timestamp.  
Payload wordt verzonden via de LoRa-module.  

---

## 3. Resultaten
Dit hoofdstuk beschrijft de resultaten van de implementatie en testen van het ESP32-gebaseerde LoRa datalogging-systeem.

### 3.1 ESP32 en LoRa-transmissie
De eerste fase van het project bestond uit het configureren van de ESP32 en de LoRa E220-module voor draadloze datatransmissie. De LoRa-module werd aangesloten op de ESP32 via UART2. In de initiële testfase werden alle verbindingen gerealiseerd met behulp van een breadboard en jumper wires, wat snelle iteraties mogelijk maakte tijdens het ontwikkelen en testen van de communicatie.

De ESP32 werd succesvol geconfigureerd om periodiek Open Transmission (OT) datapakketten te verzenden, in plaats van GPS-fixes, zodat continue monitoring van sensorwaarden mogelijk werd. De transmissie werd getest in een zender-ontvangeropstelling waarbij gestructureerde JSON-berichten werden verstuurd. De communicatie bleek stabiel wanneer de juiste kanaalinstellingen en het toegestane zendvermogen werden toegepast.

Tijdens het onderzoeksproces werd vastgesteld dat eerdere configuraties niet volledig voldeden aan de Europese LoRa-regelgeving (ETSI EN 300 220-1/2), met name voor het 868 MHz spectrum. De relevante regels die in acht werden genomen zijn:
- Maximaal zendvermogen: 14 dBm EIRP voor de toegewezen subbanden.
- Duty cycle: maximaal 1% per uur per kanaal, om spectrum congestie te beperken.
- Kanaalgebruik: uitsluitend binnen de toegestane 868,0–868,6 MHz frequentieband voor korte-range toepassingen.
- Aantal pakketten: beperkt door duty cycle, zodat de transmissies legaal bleven.

Na het aanpassen van deze parameters bleef het LoRa-verkeer binnen de wettelijke limiet.

Voor de draadloze communicatie werd een externe 868 MHz omni-directionele SMA-antenne gebruikt. Het gebruik van deze antenne leidde tot een merkbare verbetering in signaalsterkte en verbindingsbetrouwbaarheid, met name in stedelijke omgevingen met gebouwen.

De ESP32 functioneerde probleemloos als LoRa-zender. Pogingen om een Raspberry Pi 5 als ontvanger te gebruiken leverden echter beperkingen op: tijdens deze tests werden gemiste pakketten en instabiele UART-communicatie waargenomen. Om deze reden werd besloten verdere LoRa-tests uit te voeren met een tweede ESP32 als ontvanger.

Bereiktesten met twee ESP32-systemen, uitgevoerd in een bebouwde omgeving, toonden een maximale betrouwbare communicatieafstand van ongeveer 1,1 km aan. De tests bevestigden dat het systeem stabiel functioneert zolang alle Europese LoRa-regels worden nageleefd, en dat de antennepositie en configuratie van groot belang zijn voor optimale prestaties.

### 3.2 GPS-integratie
Na de succesvolle LoRa-transmissietests werd de GPS-module (Neo-6M / NEO-M8N) geïntegreerd en aangesloten op de ESP32 via UART1. De GPS-module werd getest in verschillende omstandigheden om de betrouwbaarheid van de locatiebepaling te evalueren.

Bij testen binnenshuis werd vastgesteld dat de GPS-module geen geldige locatiegegevens kon verkrijgen. De module rapporteerde geen fix en het aantal zichtbare satellieten bleef onvoldoende voor positiebepaling. Dit gedrag is consistent met de beperkingen van GNSS-systemen in afgesloten ruimtes.

Vervolgens werd de GPS-module buitenshuis getest, waarbij binnen enkele minuten een geldige GPS-fix werd verkregen. Na deze initiële fix werden locatiegegevens stabiel en periodiek bijgewerkt. De gemeten nauwkeurigheid lag typisch binnen een bereik van ongeveer 3 tot 5 meter.

Daarnaast werd getest of de GPS-module correct bleef functioneren wanneer deze in een plastic behuizing werd geplaatst. Hierbij werd waargenomen dat:
- Bij het opstarten (cold start) in de behuizing de tijd tot eerste fix merkbaar langer was.
- Na het verkrijgen van een eerste fix bleef de GPS-ontvangst stabiel, ook wanneer de module zich in de plastic behuizing bevond.

De GPS-gegevens werden succesvol verwerkt door de ESP32 en toegevoegd aan de verzonden LoRa-berichten.

### 3.3 Temperatuurmetingen
Na de integratie van de GPS-module werd de DS18B20 temperatuursensor toegevoegd aan het systeem. De sensor werd aangesloten via het OneWire-protocol en uitgelezen door de ESP32 met behulp van de DallasTemperature-bibliotheek.

Tijdens de testfase werd onderzocht wat het effect was van het weglaten van de pull-up weerstand op de datalijn van de DS18B20. In deze configuratie werden foutieve en instabiele temperatuurwaarden waargenomen, waaronder onrealistische temperatuursprongen en ongeldige metingen.

Na het opnieuw plaatsen van de pull-up weerstand op de OneWire-datalijn werden de temperatuurmetingen opnieuw uitgevoerd. De gemeten waarden bleken in deze configuratie stabiel en consistent, met realistische temperaturen die overeenkwamen met de omgevingscondities.

De temperatuursensor functioneerde correct in combinatie met de GPS- en LoRa-functionaliteit. De periodieke temperatuurmetingen werden succesvol toegevoegd aan de verzonden datapakketten, zonder merkbare invloed op de transmissiestabiliteit van het systeem.

### 3.4 Batterij en stroomverbruik
Na de integratie van de sensoren werd de voeding van het systeem getest op autonome werking zonder externe USB-voeding. Hiervoor werd een lithium-ion batterij (103450, 3,7 V) gecombineerd met een TP4056 laadmodule en een DC-DC buck converter.

Tijdens de testen werd geverifieerd dat de TP4056-module de batterij correct kon laden via USB, terwijl de buck converter de batterijspanning omzet naar een constante uitgangsspanning van 5 V. Deze stabiele 5 V werd gebruikt om de ESP32 Dev Board te voeden, zonder gebruik te maken van een USB-C kabel naar een laptop of powerbank.

Metingen toonden aan dat de ESP32 betrouwbaar bleef functioneren bij batterijvoeding, waarbij geen spanningsonderbrekingen of resets werden waargenomen tijdens LoRa-transmissies of GPS-activiteiten. Het systeem schakelde correct tussen actieve modus en light-sleep.

Het stroomverbruik werd gemeten met een USB-digital tester en gaf de volgende waarden:
- Light-sleep modus: ongeveer 90 mA
- Actieve modus tijdens LoRa-transmissie: ongeveer 150 mA

Deze resultaten bevestigen dat het systeem geschikt is voor autonome werking op batterijvoeding en dat de combinatie van batterij, TP4056 en buck converter een stabiele voedingsoplossing vormt voor de ESP32 en aangesloten modules.

### 3.6 Mechanische en elektrische afwerking
Na de functionele tests van de afzonderlijke modules werd eerst de fysieke positionering van de componenten bepaald voor plaatsing in de plastic behuizing. Hierbij werd rekening gehouden met:
- GPS-module aan de bovenkant voor maximale satellietontvangst.
- LoRa-antenne aan de voorkant voor optimale signaaluitstraling.
- USB-C connector aan de achterkant voor eenvoudige toegang om de batterij op te laden.
- DS18B20 temperatuursensor naar buiten geleid voor nauwkeurige temperatuurmetingen.

Vervolgens werd het systeem permanent opgebouwd op een soldeerplaat om betrouwbaarheid te verhogen. Tijdens het solderen werden alle componenten definitief gepositioneerd met aandacht voor:
- Correcte routing van voedings- en signaallijnen
- Minimale kabellengtes voor UART- en voedingsverbindingen
- Mechanische stabiliteit van modules en connectoren

Na het solderen werd de volledige schakeling gecontroleerd met een multimeter. Hierbij werd expliciet getest op:
- Aanwezigheid van kortsluitingen tussen voedingslijnen
- Correcte continuïteit van signaalverbindingen

Pas na deze elektrische controle werd het systeem onder spanning gezet en getest. Het systeem functioneerde correct, wat bevestigt dat zowel de mechanische als elektrische integratie succesvol was.

---
