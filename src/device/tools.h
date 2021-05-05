#ifndef TOOLS_H
#define TOOLS_H




void saveConfigValue(String name, String value);

String readConfigValue(String topic, String name);

String getConfigFilename(String name);

String split(String s, char parser, int index);

void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base);

//String deviceAddress2String(DeviceAddress deviceAddress);

boolean setupWifiSTA(const String & ssid, const String & password, const String & newMacStr);
boolean setupWifiAP(const String & ssid, const String & pwd);

void setupIo();
#endif
