/*
  LoRa register dump

  This examples shows how to inspect and output the LoRa radio's
  registers on the Serial interface
*/



#include <SPI.h>              // include libraries
#include <LoRa.h>

void setup() {
  Serial.begin(115200);               // initialize serial
  while (!Serial);
  Serial.println("");
  Serial.println("LoRa Dump Registers");

  // override the default CS, reset, and IRQ pins (optional)
  // LoRa.setPins(7, 6, 1); // set CS, reset, IRQ pin
  LoRa.setPins(15,5,4);

  if (!LoRa.begin(433E6)) {         // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                   // if failed, do nothing
  }

  LoRa.dumpRegisters(Serial);
}


void loop() {
}
