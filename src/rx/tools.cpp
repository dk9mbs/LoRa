#include "Arduino.h"
#include <ESP8266WiFi.h>

#include "config.h"
#define ENABLE_IGATE true

const String readConfigValue(String topic,String name){
  //system
#if ENABLE_IGATE
  if(name=="mode" && topic=="system") return "igate"; 
#else
  if(name=="mode" && topic=="system") return "tracker"; 
#endif
  if(name=="adminpwd" && topic=="system") return adminpwd; 

  // wlan
#if ENABLE_IGATE
  if(name=="mode" && topic=="wlan") return "client"; 
#else
  if(name=="mode" && topic=="wlan") return "ap"; 
#endif
  if(name=="ssid" && topic=="wlan") return wlanSsid;
  if(name=="password" && topic=="wlan") return wlanPwd;
  if(name=="ssidap" && topic=="wlan") return "igate.dk9mbs.de";


  //aprs iGate
  if(name=="igatecall" && topic=="aprsis") return igateCall;
  if(name=="pass" && topic=="aprsis") return aprsPass;
  if(name=="filter" && topic=="aprsis") return "m/1";
  if(name=="beacon" && topic=="aprsis") return "433.775 LoRa APRS iGATE (homemade) by DK9MBS";

  //aprs Tracker
  if(name=="call" && topic=="aprs") return aprsTrackerCall;
  if(name=="path" && topic=="aprs") return "WIDE1-1,WIDE2-2";  
  if(name=="destination" && topic=="aprs") return "APRS";

  Serial.println("Topic or name not found!:"+topic+" "+name);
}

void setupIo() {
  ESP.eraseConfig();
  WiFi.setAutoConnect(false);
}

boolean setupWifiSTA(const String & ssid, const String & password, const String & newMacStr) {
  int timeoutCounter=0;
  int timeout=20;
  
  uint8_t mac[6];
  byte newMac[6];
  //parseBytes(newMacStr, '-', newMac, 6, 16);

  //clearLcdLine(lcd,0);
  //printLcd(lcd, 0,0, "Connecting WLAN",0);
  
  WiFi.setAutoReconnect(true);

  if(newMacStr != "") {
    wifi_set_macaddr(0, const_cast<uint8*>(newMac));
    Serial.println("mac address is set");
  }
  
  //wifi_station_set_hostname(readConfigValue("hostname").c_str());
  Serial.print("Hostname ist set: ");
  //Serial.println(readConfigValue("hostname"));
  
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  Serial.print("Password:");
  Serial.println("***********");
  
  WiFi.begin(ssid, password);
  
  Serial.println("after WiFi.begin():");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    timeoutCounter++;
    //clearLcdLine(lcd,1);
    //printLcd(lcd, 0,1, String(timeoutCounter)+"/"+String(timeout),0);
    if (timeoutCounter>timeout) return false;
    
  }

  WiFi.macAddress(mac);
  
  Serial.println("WiFi connected");
  Serial.print("IP address:");
  Serial.println(WiFi.localIP());
  Serial.printf("Mac address:%02x:%02x:%02x:%02x:%02x:%02x\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  Serial.printf("Mac address:%s\n", WiFi.macAddress().c_str());
  Serial.print("Subnet mask:");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway:");
  Serial.println(WiFi.gatewayIP());

  //printLcd(lcd, 0,0, WiFi.localIP(),1);
  //printLcd(lcd, 0,1, "AG5ZL Rotor",0);

  return true;
}

boolean setupWifiAP(const String & ssid, const String & password){
  Serial.print("AP is starting...");
  String pwd;

  pwd=password;
  
  if(pwd==""){
    pwd="0000";
  }
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, pwd);

  Serial.println("ok");
  
  Serial.print("Password for AP:");
  Serial.println(pwd);

  return true;
}
