#include <WiFiS3.h>
#include "Arduino_LED_Matrix.h" 

/* 
 * NOTE TO READERS: 
 * Copy 'arduino_secrets_example.h', rename it to 'arduino_secrets.h', 
 * and insert your own Wi-Fi credentials before compiling.
 */
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

WiFiServer server(80); 
ArduinoLEDMatrix matrix;

// ===== LED Matrix Icons (Visual 2D Arrays) =====

// "Straight Face" - Idle State
byte iconIdle[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0 },
  { 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

// "Wi-Fi Waves" - Active/Connected State
byte iconWiFi[8][12] = {
  { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
  { 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0 },
  { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
  { 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 }
};

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  
  matrix.begin(); // Power up the LED matrix
  matrix.renderBitmap(iconIdle, 8, 12); // Display the straight face

  Serial.println("=================================");
  Serial.println(" OSI Network Dashboard - Phase 3 ");
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
    
    Serial.print("Waiting for IP address...");
    while (WiFi.localIP() == IPAddress(0, 0, 0, 0)) {
      Serial.print(".");
      delay(500);
    }
    Serial.println();
    
    server.begin(); 
    Serial.print("Dashboard running! Open a browser and go to: http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect.");
  }
}

void loop() {
  WiFiClient client = server.available(); 

  if (client) {
    Serial.println("New TCP Client Connected!");
    matrix.renderBitmap(iconWiFi, 8, 12); 
    
    String currentLine = "";
    String httpRequest = ""; 
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (httpRequest.length() < 50) {
          httpRequest += c;
        }
        
        if (c == '\n') {
          if (currentLine.length() == 0) {
          
            if (httpRequest.indexOf("GET /api/stack") >= 0) {
              
              // Serve JSON (For Python/AI/Automation)
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type: application/json");
              client.println("Connection: close");
              client.println();
              
              client.print("{");
              client.print("\"board\":\"Arduino UNO R4 WiFi\",");
              client.print("\"layer_1_rssi_dbm\":");
              client.print(WiFi.RSSI());
              client.print(",");
              client.print("\"layer_3_ip\":\"");
              client.print(WiFi.localIP());
              client.print("\"}");
              client.println();
              
              Serial.println("-> Served JSON Telemetry API");

            } else {
              
              // Serve HTML Dashboard
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println(); 
              
              client.println("<!DOCTYPE HTML><html>");
              client.println("<head><title>OSI Telemetry Node</title></head>");
              client.println("<body style='font-family: Arial; margin: 40px; background: #1e1e1e; color: #fff;'>");
              
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
              client.print("<br>Gateway: ");
              client.print(WiFi.gatewayIP());
              client.println("</p>");
              
              client.println("<p><b>Layer 4 (Transport)</b><br>Protocol: TCP | Port: 80</p>");
              
              client.println("<hr style='border-color: #444; margin-top: 20px;'>");
              client.println("<a href='/api/stack' style='display:inline-block; margin-top:10px; padding:10px 15px; background:#4CAF50; color:#fff; text-decoration:none; border-radius:5px;'>Test JSON API Endpoint</a>");
              
              client.println("</div></body></html>");
              client.println();
              
              Serial.println("-> Served HTML Dashboard");
            }
            break; 
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("TCP Socket Closed.");
    delay(500); 
    matrix.renderBitmap(iconIdle, 8, 12); // Return to idle face
  }
}