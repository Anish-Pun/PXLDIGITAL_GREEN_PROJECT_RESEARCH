# **Receiver Application Note**
*Made by Anish*

## **3.1 Project 1**

### **3.1.1 Design A — LoRa-ontvanger (Raspberry Pi + E220)**

Voor dit project heb ik een ontvanger gebouwd op basis van een Raspberry Pi die via UART verbonden is met een E220 LoRa-module.
De ontvanger krijgt JSON-pakketten binnen die door mijn ESP32-zender worden doorgestuurd.

Mijn belangrijkste doelen waren:

* een stabiele UART-verbinding opzetten
* LoRa-pakketten correct kunnen ontvangen
* JSON correct kunnen uitlezen
* deze data foutloos doorgeven aan Node-RED

Ik heb vooral op betrouwbaarheid en eenvoud gewerk. Zo kon ik problemen snel opsporen en oplossen.

### **3.1.2 Design B — Node-RED Dataverwerking**

In Node-RED heb ik de volledige flow gebouwd om de ontvangen data te verwerken en te visualiseren.

In mijn flow heb ik o.a. gebruikt:

* **Serial In Node** om data van de Pi te lezen
* **JSON Parser** om de LoRa-data om te zetten naar bruikbare waarden
* **Function nodes** voor extra bewerking zoals:

  * validatie van data
  * formatteren
  * foutafhandeling
  * temperatuurconversie

Daarnaast heb ik **extra functionaliteit toegevoegd**:

* een **function block** waar ik custom logica verwerk
* een **LCD-node** die verbonden is met een fysiek LCD-scherm

Op dit LCD-scherm toon ik:

* welke devices verbonden zijn met de ontvanger
* de temperatuur van elk device
* een connectiestatus per device

Zo kan iemand zonder dashboard toch onmiddellijk op het LCD-scherm zien welke sensoren actief zijn.

---

## **3.2 Project 2**

### **3.2.1 Optie X — Real-time Dashboard**

Naast het LCD-scherm heb ik ook een dashboard gemaakt in Node-RED waar ik:

* temperatuur
* lichtniveau
* GPS
* tijdstip van laatste update
* verbindingsstatus

in real-time toon.

Het dashboard is overzichtelijk opgebouwd zodat alles direct duidelijk is voor de gebruiker.

### **3.2.2 Optie Y — LCD-module**
Ik heb een LCD-node gekoppeld aan een fysiek LCD-scherm dat ik op de ontvanger (Raspberry Pi) heb aangesloten.
Dit scherm toont automatisch:

* alle devices die LoRa-data sturen
* hun actuele temperatuur
* in één oogopslag of het device nog verbonden is

Dit maakt de ontvanger veel praktischer als standalone systeem.

---

## **3.3 Totaalproject**

Het volledige ontvangerproject dat ik heb gebouwd bestaat uit:

1. Raspberry Pi + LoRa-module
2. UART-communicatie
3. JSON-ontvangst
4. Node-RED dataverwerking
5. Real-time dashboard
6. Fysiek LCD-scherm met status

Het systeem ontvangt data van de ESP32-zender, verwerkt deze automatisch, en toont alle informatie live aan de gebruiker via het dashboard én het LCD-scherm.

---

# **4 Gebruikerservaring**

Ik heb tijdens de ontwikkeling sterk rekening gehouden met de gebruiker.
Het systeem is duidelijk, overzichtelijk en makkelijk te interpreteren.

### **Hoe de gebruiker het systeem ervaart**

* Het LCD-scherm toont meteen welke devices verbonden zijn.
* De temperatuur is direct zichtbaar zonder iets te moeten openen.
* In het dashboard ziet de gebruiker live alle metingen.
* De updates gebeuren automatisch, zonder knoppen of instellingen.

---

# **5 Discussie**

Het resultaat van mijn ontvanger voldoet aan mijn verwachtingen.
De LoRa-ontvangst is stabiel en de verwerking in Node-RED werkt zonder problemen.

### **Belangrijkste inzichten**

* JSON werkt uitstekend voor eenvoudige parsing.
* Een extra LCD-scherm verhoogt de bruikbaarheid enorm.
* Node-RED is heel flexibel voor debugging en visualisatie.

---

# **6 Conclusie**

Mijn ontvangerproject werkt betrouwbaar en gebruiksvriendelijk.
Dankzij de combinatie van het Node-RED dashboard en het LCD-scherm kan de gebruiker zowel digitaal als fysiek de status van alle devices volgen.

Ik kan concluderen dat:

* de LoRa-ontvangst functioneel is,
* de JSON-verwerking correct verloopt,
* het dashboard professioneel en duidelijk is,
* het LCD-scherm het project naar een hoger niveau tilt.

Het systeem is klaar voor verdere uitbreiding en voldoet aan de basisvereisten en meer.

---

# **7 Referenties**

*komt nog...*

---

# **8 Bijlagen**

*komt nog...*

---
