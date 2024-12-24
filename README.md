# Energy Monitoring System

## Overview
The Energy Monitoring System is an IoT-based solution that allows real-time monitoring and visualization of power consumption. The system integrates hardware sensors, Firebase, and a web dashboard to provide comprehensive energy data, enabling efficient energy management.

## Features
- **Real-time Monitoring:** Track voltage, current, power, energy, frequency, and power factor in real-time.
- **Data Visualization:** Visualize energy consumption trends using an interactive chart.
- **Google Sheets Integration:** Export energy data to Google Sheets for detailed analysis.
- **Firebase Integration:** Store and retrieve sensor data using Firebase Realtime Database.
- **WiFi Management:** Seamless WiFi configuration using WiFiManager.
- **Energy Reset:** Reset energy readings via hardware button or web interface.

## Components Used
### Hardware:
- ESP32 microcontroller
- PZEM004T v3.0 Energy Meter
- SH1106 OLED Display
- Push Buttons for Reset
- Supporting components (wires, connectors, etc.)

### Software:
- Arduino IDE
- Firebase Realtime Database
- Google Apps Script (for Google Sheets integration)
- CanvasJS (for charts)
- Firebase JavaScript SDK

## Installation and Setup

### Prerequisites
- ESP32 board with Arduino IDE configured.
- Firebase project with Realtime Database enabled.
- Google Apps Script for Sheets integration set up.

### Steps
1. **Clone the Repository:**
   ```bash
   git clone https://github.com/your-username/energy-monitoring-system.git
   ```

2. **Upload Code to ESP32:**
   - Open the `energy_monitor.ino` file in Arduino IDE.
   - Update Firebase credentials and WiFi details.
   - Upload the code to the ESP32.

3. **Set Up Firebase Realtime Database:**
   - Create a Firebase project at [Firebase Console](https://console.firebase.google.com/).
   - Add a Realtime Database and copy the database URL.
   - Update the `FIREBASE_HOST` and `FIREBASE_AUTH` in the code.

4. **Configure Google Apps Script:**
   - Create a new Apps Script project and copy the provided script.
   - Deploy as a web app and copy the deployment URL.
   - Update the `sheet_url` in the code with the URL.

5. **Run the Web Dashboard:**
   - Open `index.html` in a browser to view the real-time dashboard.

## Usage
### Monitoring Data
- The OLED display shows real-time sensor readings.
- The web dashboard updates automatically with Firebase data.

### Resetting Energy
- Press the hardware reset button connected to GPIO 12 to reset energy readings.
- Alternatively, use the web dashboard toggle button.

### Download Data
- Click the "Download Google Sheets" button on the dashboard to access historical data.
