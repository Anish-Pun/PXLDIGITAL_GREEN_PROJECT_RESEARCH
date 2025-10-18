/*
 * LoRa E220
 * Start device or reset to send a message
 * https://mischianti.org/ebyte-lora-e22-device-for-arduino-esp32-or-esp8266-specs-and-basic-usage-1/
 *
 * E220       ----- esp32
 * M0         ----- GND
 * M1         ----- GND
 * TX         ----- RX2 (PullUP)
 * RX         ----- TX2 (PullUP)
 * AUX        ----- Not connected
 * VCC        ----- 3.3v
 * GND        ----- GND
 */
#include "Arduino.h"
#include "LoRa_E220.h"
 
LoRa_E220 e220ttl(&Serial2); // WeMos RX --> e220 TX - WeMos TX --> e220 RX
 
void setup() {
  Serial.begin(9600);
  delay(500);
 
  Serial.println("Hi, I'm going to send message!");
 
  // Startup all pins and UART
  e220ttl.begin();
 
  // Send message
  ResponseStatus rs = e220ttl.sendMessage("Hello, world?");
  // Check If there is some problem of successfully send
  Serial.println(rs.getResponseDescription()); // Success
}
 
void loop() {
  // If something available
  if (e220ttl.available()>1) {
      // read the String message
    ResponseContainer rc = e220ttl.receiveMessage();
    // Is something goes wrong print error
    if (rc.status.code!=1){
        rc.status.getResponseDescription();
    }else{
        // Print the data received
        Serial.println(rc.data);
    }
  }
  if (Serial.available()) {
      String input = Serial.readString();
      e220ttl.sendMessage(input);
  }
}