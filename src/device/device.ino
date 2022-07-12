#include <SPI.h>
#include <LoRa.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <APRS-Decoder.h>
#include "tools.h"

#ifdef ARDUINO_SAMD_MKRWAN1300
#error "This example is not compatible with the Arduino MKR WAN 1300 board!"
#endif

//LoRa
const long frequency=433.775E6; //given in MHz
const int spreading_factor=12; //6-12
const int cr_denominator=5; // 5-8
const int bandwitdh=125E3; // given in KHz

const int cs_pin=15;
const int reset_pin=5; //5
const int dio0_pin=4; // 4

// aprs
const int aprsTrackerTimeout=60000;


WiFiClient client;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  setupIo();

  String aprsMode=readConfigValue("system","mode");
  
  if(readConfigValue("wlan","mode")=="client"){
    Serial.println("connecting wlan ...");
    if(!setupWifiSTA(readConfigValue("wlan","ssid"), readConfigValue("wlan","password"), "")){
      Serial.println("Error connect wlan ... try to start ap mode!");
      setupWifiAP(readConfigValue("wlan","ssidap"), readConfigValue("system","adminpwd") );
    } else {
      Serial.println("wlan started!");
    }
    
    Serial.print("connecting aprs-is server...");
    while(!connectAprsIsServer(client, readConfigValue("aprsis","igatecall"),readConfigValue("aprsis","pass"),readConfigValue("aprsis","filter"))){
      Serial.print(".");
    }
    Serial.println("aprs-is server connected!");
    
  } else {
    setupWifiAP(readConfigValue("wlan","ssidap"), readConfigValue("system","adminpwd") );
  }

  setupLoRa();
//  LoRa.setPins(cs_pin,reset_pin,dio0_pin);
//
//  Serial.print("starting LoRa ...");
//
//  if (!LoRa.begin(frequency)) {
//    Serial.println("Starting LoRa failed!");
//    while (1);
//  }
//  Serial.println("ok");
//  LoRa.setSpreadingFactor(spreading_factor);
//  LoRa.setSignalBandwidth(bandwitdh);
//  LoRa.setCodingRate4(cr_denominator);
//
//  LoRa.enableCrc();
//  LoRa.setTxPower(17);
//  // Uncomment the next line to disable the default AGC and set LNA gain, values between 1 - 6 are supported
//  //LoRa.setGain(6);
//
//  //only if async
//  //LoRa.onReceive(onReceive);
//  //LoRa.receive();
}

void loop() {
  String packet;
  char sign;
  String payload;
  static String mode=readConfigValue("system","mode");
  static String wlanMode=readConfigValue("wlan","mode");
  
  static unsigned long lastWd=0;
  static unsigned long lastIgateBeacon=0;
  static unsigned long lastTrackerBeacon=0;
  
  int packetSize = LoRa.parsePacket();

  if(millis()>lastWd+10000 || lastWd==0) {
    bool status;
    status=true;
    
    //read wlan status
    if(wlanMode=="client" && WiFi.status()!=WL_CONNECTED){
      Serial.print("WLAN is broken!!!");
      status=false;
    } 

    // get client status (aprs-is server)
    if (wlanMode=="client" && !client.connected()){
      Serial.println("APRS-IS Server not connected!!!");
      status=false;
    }
    
    lastWd=millis();
  }

  //Send beacon via TCP 
  if( (millis()>lastIgateBeacon+900000 || lastIgateBeacon==0) && mode=="igate" ){
    sendTcpBeaconText(client, readConfigValue("aprsis","igatecall"), readConfigValue("aprsis","beacon"));
    lastIgateBeacon=millis();
  }

  //Send Beacon in tracker mode
  if( (millis()>lastTrackerBeacon+aprsTrackerTimeout || lastTrackerBeacon==0) && mode=="tracker" ) {
    String data;
    data=buildAprsTrackerMessage(readConfigValue("aprs","call"),readConfigValue("aprs","destination"),readConfigValue("aprs","path") );
    Serial.print(data);

    LoRa.beginPacket();
    // Header:
    LoRa.write('<');
    LoRa.write(0xFF);
    LoRa.write(0x01);
    // APRS Data:
    LoRa.write((const uint8_t *)data.c_str(), data.length());
    LoRa.endPacket();

    Serial.println(" ok");
    
    lastTrackerBeacon=millis();
  }

  //put all incoming messages from tcp network to the serial device
  if(wlanMode=="client" && client.available()){
    Serial.println(client.readStringUntil('\n'));
  }

  // process incoming messages
  if (packetSize) {
    Serial.println("");
    Serial.print("rx package '");

    while (LoRa.available()) {
      //packet.concat((char)(LoRa.read()));
      packet=LoRa.readString();
    }
/*
    packet=packet.substring(3);
    APRSMessage msg;
    msg.decode(packet);
    msg.setPath(readConfigValue("aprsis","igatecall")+"*");
    Serial.print(msg.encode());
  
    Serial.print("' (");
    Serial.print(LoRa.packetRssi());
    Serial.print(") ");
    Serial.print("(");
    Serial.print(LoRa.packetSnr());
    Serial.print(") ");


    if(wlanMode=="client" && WiFi.status()==WL_CONNECTED){
      client.print(packet+"\r\n");
    } 
*/
    Serial.print(packet);
    Serial.println("ok");
  
  }

  if(Serial.available()) {
    String cmd=Serial.readString();
    cmd.replace("\r","");
    cmd.replace("\n","");
    
    Serial.println("cmd>"+cmd);
    if(cmd=="lora.reset") setupLoRa();
  }
  
}


// aprs tools

String buildAprsTrackerMessage(const String & source, const String & destination, const String & path) {
  APRSMessage msg;
  msg.setSource(source);
  msg.setDestination(destination);
  msg.setPath(path);
  msg.getBody()->setData("!5201.89N/01021.86E>144.650 LoRa APRS Tracker (homemade) by DK9MBS");

  String data = msg.encode();
  return data;
}

bool sendTcpBeaconText(WiFiClient &client, const String &call, const String &msg) {
  Serial.print("igate beacon sendig via tcpip...");
  client.print(call+">APRS,TCPIP*:=5202.93N/01022.37E&"+msg+"\r\n");
  Serial.println("ok");
  return true;
}

bool connectAprsIsServer(WiFiClient &client, const String &call, const String &pass, const String &filter) {
  if(client.connect("rotate.aprs2.net",14580)) {
    String logon="user "+call+" pass "+pass;
    
    Serial.println("APRS logon: "+logon);
    Serial.println("APRS filter: "+filter);

    client.print(logon+"\r\n");

    if(filter!=""){
      client.print("#filter "+filter+"\r\n");
    }
  } else {
    Serial.println("Error Connect APRS-IS Server!");
    return false;
  }

  return true;
}

//LoRa Tools
boolean setupLoRa() {
  LoRa.setPins(cs_pin,reset_pin,dio0_pin);

  Serial.print("starting LoRa ...");

  if (!LoRa.begin(frequency)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("ok");
  LoRa.setSpreadingFactor(spreading_factor);
  LoRa.setSignalBandwidth(bandwitdh);
  LoRa.setCodingRate4(cr_denominator);
  LoRa.setSyncWord(0x12);
  LoRa.enableCrc();
  //LoRa.setTxPower(17);
  // Uncomment the next line to disable the default AGC and set LNA gain, values between 1 - 6 are supported
  //LoRa.setGain(6);

  //only if async
  //LoRa.onReceive(onReceive);
  //LoRa.receive();
  return true;

}
