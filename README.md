# Live OSI Model Network Dashboard – Arduino UNO R4 WiFi

A hands-on educational project that maps real-time network status of the Arduino UNO R4 WiFi to the **OSI Model**.

The board connects to a Wi-Fi network and serves:
- A live HTML dashboard
- A simple JSON API endpoint (`/api/stack`)
- Visual Layer 1 feedback on the built-in 12×8 LED matrix

Built as a CCNA-aligned portfolio project.

## Features

- **Layer 1 (Physical)**: Real-time RSSI (dBm) + LED matrix status icons
- **Layer 2 (Data Link)**: MAC address display
- **Layer 3 (Network)**: IP address, subnet mask, default gateway (DHCP)
- **Layer 4 (Transport)**: TCP server on port 80
- **Layer 7 (Application)**: HTTP HTML dashboard + machine-readable JSON API

## CCNA Concept Mapping

**Layer 1 – Physical**  
CCNA Topic: Wireless signals & media  
Demonstrated by: RSSI measurement + LED matrix visual feedback

**Layer 2 – Data Link**  
CCNA Topic: MAC addressing  
Demonstrated by: Extraction and display of the board’s MAC address

**Layer 3 – Network**  
CCNA Topic: IPv4 addressing & DHCP  
Demonstrated by: DHCP lease, IP, subnet mask, and default gateway

**Layer 4 – Transport**  
CCNA Topic: TCP  
Demonstrated by: TCP server listening on port 80 and client socket handling

**Layer 7 – Application**  
CCNA Topic: HTTP & basic APIs  
Demonstrated by: HTML dashboard + `/api/stack` JSON endpoint

## Project Structure

```
osi-network-dashboard-arduino-r4/
├── src/OSI_Network_Dashboard/
│   ├── OSI_Network_Dashboard.ino
│   └── arduino_secrets_example.h
├── LICENSE
├── .gitignore
└── README.md
```

## Setup

1. Clone this repository.
2. Copy `arduino_secrets_example.h` → `arduino_secrets.h` and add your Wi-Fi credentials.  
   (The real secrets file is git-ignored.)
3. Open `OSI_Network_Dashboard.ino` in Arduino IDE.
4. Select board: **Arduino UNO R4 WiFi**.
5. Upload the sketch.
6. Open Serial Monitor at **115200 baud** to see the assigned IP address.
7. Open that IP in a browser on the same network.

## API Endpoint

**`GET /api/stack`**

Example response:

```json
{
  "board": "Arduino UNO R4 WiFi",
  "layer_1_rssi_dbm": -60,
  "layer_3_ip": "192.168.1.7"
}
```

## Troubleshooting

- **Cannot reach the dashboard**  
  The UNO R4 WiFi only supports 2.4 GHz. Make sure your phone or laptop is connected to the 2.4 GHz network (many dual-band routers keep 2.4 GHz and 5 GHz separate).

- **IP shows as 0.0.0.0**  
  The board has associated with the access point but has not yet received a DHCP lease. The code waits for a valid IP before starting the web server.

## Future Improvements

- Dynamic signal-strength bars on the LED matrix
- Toggle between DHCP and static IP
- More complete JSON telemetry
- Simple gateway reachability check

## License

MIT License