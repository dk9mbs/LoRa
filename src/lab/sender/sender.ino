/* Lib:
 * https://github.com/sandeepmistry/arduino-LoRa
 * https://github.com/lora-aprs/APRS-Decoder-Lib
 */
#include <SPI.h>
#include <LoRa.h>
#include <APRS-Decoder.h>

#define ENABLE_SEND true

//LoRa
const long frequency=433.775E6; //given in MHz
const int spreading_factor=12; //6-12
const int cr_denominator=5; // 5-8
const int bandwitdh=125E3; // given in KHz
// LoRa HW
const int cs_pin=15;
const int reset_pin=5;
const int dio0_pin=4;

//APRS
const String aprs_path="WIDE1-1,WIDE2-2";
const String aprs_source="DK9MBS-1";
const String aprs_destination="APRS";
const int aprs_timeout=60000;

int counter = 0;


void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Sender");

  LoRa.setPins(cs_pin,reset_pin,dio0_pin);
  
  
  if (!LoRa.begin(frequency)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  LoRa.setSpreadingFactor(spreading_factor);
  LoRa.setSignalBandwidth(bandwitdh);
  LoRa.setCodingRate4(cr_denominator);

  LoRa.enableCrc();
  LoRa.setTxPower(17);
}

void loop() {
  static unsigned long lastLoop=0;

  if(millis()>lastLoop+aprs_timeout || lastLoop==0) {

    String data;
    data=build_aprs_message(aprs_source, aprs_destination, aprs_path);
    Serial.print(data);

#if ENABLE_SEND
    LoRa.beginPacket();
    // Header:
    LoRa.write('<');
    LoRa.write(0xFF);
    LoRa.write(0x01);
    // APRS Data:
    LoRa.write((const uint8_t *)data.c_str(), data.length());
    LoRa.endPacket();
#endif

    Serial.println(" ok");
    
    lastLoop=millis();
  }

}


String build_aprs_message(const String & source, const String & destination, const String & path) {
  APRSMessage msg;
  msg.setSource(source);
  msg.setDestination(destination);
  msg.setPath(path);
  msg.getBody()->setData("!5201.89N/01021.86E>144.650 LoRa APRS Tracker (Homemade) by DK9MBS");

  String data = msg.encode();
  return data;
}
