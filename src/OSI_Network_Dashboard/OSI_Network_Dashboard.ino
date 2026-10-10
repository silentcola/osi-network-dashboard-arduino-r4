 #include <WiFiS3.h>

/* 
 * NOTE TO READERS: 
 * Copy 'arduino_secrets_example.h', rename it to 'arduino_secrets.h', 
 * and insert your own Wi-Fi credentials before compiling.
 */

#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

// Layer 4: Initialize a TCP server on port 80 (HTTP standard)
WiFiServer server(80); 

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }

  Serial.println("=================================");
  Serial.println(" OSI Network Dashboard - Phase 2 ");
  Serial.println("=================================");

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
    
    // --- ADD THIS DHCP WAIT LOOP ---
    Serial.print("Waiting for IP address...");
    while (WiFi.localIP() == IPAddress(0, 0, 0, 0)) {
      Serial.print(".");
      delay(500);
    }
    Serial.println();
   
    
    // Start listening for TCP connections
    server.begin(); 
    
    Serial.print("Dashboard running! Open a browser and go to: http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect.");
  }
}

void loop() {
  // Listen for incoming clients connecting to Port 80
  WiFiClient client = server.available(); 

  if (client) {
    Serial.println("New client connected (TCP Handshake complete).");
    String currentLine = "";
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        // A blank line (two newlines in a row) marks the end of the client's HTTP request
        if (c == '\n') {
          if (currentLine.length() == 0) {
            
            // Layer 7: Send HTTP Response Headers
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println(); 
            
            // Layer 7: Send the HTML Payload
            client.println("<!DOCTYPE HTML><html>");
            client.println("<head><title>OSI Telemetry Node</title></head>");
            client.println("<body style='font-family: Arial, sans-serif; margin: 40px; background-color: #1e1e1e; color: #fff;'>");
            
            client.println("<h2 style='color: #4CAF50;'>Live OSI Network Dashboard</h2>");
            client.println("<div style='background: #2d2d2d; padding: 20px; border-radius: 8px; border: 1px solid #444;'>");
            
            client.print("<p><b>Layer 1 (Physical)</b><br>RSSI: ");
            client.print(WiFi.RSSI());
            client.println(" dBm</p>");
            
            byte mac[6];
            WiFi.macAddress(mac);
            client.print("<p><b>Layer 2 (Data Link)</b><br>MAC Address: ");
            for (int i = 0; i < 6; i++) {
              if (mac[i] < 16) client.print("0");
              client.print(mac[i], HEX);
              if (i < 5) client.print(":");
            }
            client.println("</p>");
            
            client.print("<p><b>Layer 3 (Network)</b><br>IP Address: ");
            client.print(WiFi.localIP());
            client.print("<br>Subnet Mask: ");
            client.print(WiFi.subnetMask());
            client.print("<br>Default Gateway: ");
            client.print(WiFi.gatewayIP());
            client.println("</p>");
            
            client.println("<p><b>Layer 4 (Transport)</b><br>Protocol: TCP<br>Port: 80</p>");
            client.println("<p><b>Layer 7 (Application)</b><br>Protocol: HTTP/1.1</p>");
            
            client.println("</div></body></html>");
            client.println();
            break; 
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    // Close the connection
    client.stop();
    Serial.println("Client disconnected (Socket closed).");
  }
}