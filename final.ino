#include "WiFi.h"
#include "esp_wifi.h"

#define BLYNK_TEMPLATE_ID "TMPL3aYciGYOV"
#define BLYNK_TEMPLATE_NAME "Final"
#define BLYNK_AUTH_TOKEN "ULMnT96CGSGKqXuk7-N0php3c1CKBbvZ"
#include <BlynkSimpleEsp32.h>

const char *softAP_SSID = "ESP32";
String softAP_password = "123456789";
const int maxStations = 10; 
String macAddresses[maxStations]; 
int numConnectedStations = 0;
int rssiValues[maxStations]; 
IPAddress ipAddresses[maxStations];
bool st_update=false;
bool isSoftAPOn = false;

void PrintStations() {
  wifi_sta_list_t stationList;
  esp_wifi_ap_get_sta_list(&stationList);
  delay(2000);
  Serial.print("Print stations ");
  for (int i = 0; i < stationList.num; i++) {
    wifi_sta_info_t station = stationList.sta[i];
    String macAddress = "";
    for (int j = 0; j < 6; j++) {
      char str[3];
      sprintf(str, "%02x", (int)station.mac[j]);
      macAddress += str;
      if (j < 5) {
        macAddress += ":";
      }
    }
    macAddresses[i] = macAddress;
    rssiValues[i] = station.rssi; 
    Serial.print("MAC Address: ");
    Serial.print(macAddresses[i]);
    Serial.print(", RSSI: ");
    Serial.println(rssiValues[i]);
    st_update=true;
  }
  numConnectedStations = stationList.num; 
}



void setup() {
  Serial.begin(9600);
  
  WiFi.begin("leenu", "leenu392");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.print("Connected to WiFi, IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.mode(WIFI_AP);
  WiFi.softAP(softAP_SSID, softAP_password);
  Serial.print("SoftAP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  
  PrintStations();
  delay(15000);
  String allMacAddresses;
  if (st_update)
  {
    Blynk.begin(BLYNK_AUTH_TOKEN, "leenu", "leenu392");
   for (int i = 0; i < numConnectedStations; i++) {
    Blynk.begin(BLYNK_AUTH_TOKEN, "leenu", "leenu392");
    Serial.print("loop: ");
    
    allMacAddresses += "MAC: " + macAddresses[i] +  " , RSSI: " + String(rssiValues[i]) + " ; \n";
    
    if (i < numConnectedStations - 1) {
      allMacAddresses += "\n"; 
    }
   }
  }

  String message = "Devices connected: " + String(numConnectedStations);
  String gatewayAddressMsg = "Gateway Address: " + WiFi.softAPIP().toString(); 
  String esp_ip="Esp32 Ip address :"+WiFi.localIP().toString();

  String m = " ";

  Serial.println(allMacAddresses);

  Blynk.virtualWrite(V6, gatewayAddressMsg);
  Blynk.virtualWrite(V7, esp_ip);
  Blynk.virtualWrite(V1, m);
  Blynk.virtualWrite(V2, m);
  Blynk.virtualWrite(V1, message);
  Blynk.virtualWrite(V2, allMacAddresses);
  
  st_update=false;
   
  Blynk.run();
}

BLYNK_WRITE(V5) {
  String command = param.asStr();
  if (command != softAP_password && command.length()>8) 
  {
    delay(1000);
    
    WiFi.softAPdisconnect(true); 
    delay(1000);
    WiFi.softAP(softAP_SSID, command); 
    Serial.println("SoftAP password changed to: " + command);
    softAP_password=command;
  }
}


BLYNK_WRITE(V3) { 
  int switchState = param.asInt();
  Serial.println(switchState);
  if (switchState == 1) {    
      Blynk.disconnect();
      WiFi.mode(WIFI_AP_STA);fr
      WiFi.softAP(softAP_SSID, softAP_password);
      delay(3000);
      Serial.println("Connected");
      loop();
  } 
  else 
  {
     WiFi.softAPdisconnect(true);
      Serial.println("DisConnected");
  }
}
