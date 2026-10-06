//https://docs.52pi.com/md/kz-0073/arduino/p5/
#include <WiFiS3.h>

/*NOTE TO READERS: 
  Copy 'arduino_secrets_example.h', rename it to 'arduino_secrets.h', 
  and insert your own Wi-Fi credentials before compiling.*/
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; 
  }

  Serial.println("=================================");
  Serial.println(" OSI Network Dashboard - Phase 1 ");
  Serial.println("=================================");
  Serial.println();

  
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("ERROR: WiFi module not found");
    while (true);
  }

  
  Serial.print("Connecting to: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);

  
  int attempts = 0;
  while (status != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    status = WiFi.status();
    attempts++;
  }

  Serial.println();

  if (status == WL_CONNECTED) {
    Serial.println("Connected successfully!");
    printNetworkInfo();
  } else {
    Serial.println("Failed to connect.");
  }
}

void loop() {
  // keep the connection alive and gives update every 15 seconds
  delay(15000);
  Serial.println("----- Status Update -----");
  printNetworkInfo();
}

void printNetworkInfo() {
  Serial.println();
  Serial.println("=== Current Network Information ===");

  // Layer 1 - Physical
  Serial.print("Layer 1 (Physical)  - RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

  // Layer 2 - Data Link
  Serial.print("Layer 2 (Data Link) - MAC:  ");
  printMacAddress();

  // Layer 3 - Network
  Serial.print("Layer 3 (Network)   - IP:   ");
  Serial.println(WiFi.localIP());
  Serial.print("                  - Subnet: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("                  - Gateway: ");
  Serial.println(WiFi.gatewayIP());

  Serial.println("===================================");
  Serial.println();
}

// method to get the MacAddress
void printMacAddress() {
  byte mac[6];
  WiFi.macAddress(mac);
  for (int i = 0; i < 6; i++) {
    if (mac[i] < 16) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
}