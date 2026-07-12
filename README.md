ResQLink – Smart Emergency Response Badge

Team ZEPHYR

Startlet Hackathon 2026

Problem Statement

During emergencies, victims may be unable to communicate their location or medical information due to panic, injury, or language barriers. This can delay rescue and medical assistance.

Solution

ResQLink is a smart emergency response platform built using an ESP32. When the SOS button is pressed, the device reads the user's GPS location, sends an emergency alert through Bluetooth, activates an LED and buzzer, and displays the alert status on an OLED screen. The platform can also be integrated with a website to display emergency information and assist first responders.

Features

- One-button SOS activation
- GPS location tracking
- Bluetooth Low Energy (BLE) communication
- OLED status display
- LED and buzzer emergency alert
- Fast and simple operation

Hardware Used

- ESP32 DevKit
- GPS Module (NEO-6M)
- OLED Display (SSD1306)
- Push Button
- LED
- Active Buzzer
- Breadboard and Jumper Wires

Software Used

- Arduino IDE
- ESP32 Board Package
- TinyGPSPlus Library
- Adafruit SSD1306 Library
- Adafruit GFX Library
- ESP32 BLE Library

Working

1. The ESP32 initializes all connected components.
2. The OLED displays SAFE while the system is idle.
3. The device continuously reads GPS data.
4. When the SOS button is pressed:
   - The current GPS location is obtained.
   - An emergency notification is sent over Bluetooth.
   - The LED blinks and the buzzer sounds.
   - The OLED displays SOS SENT with the location.

Future Improvements

- Live location sharing over Wi-Fi
- Medical profile integration
- Cloud database support
- Mobile application
- Multilingual voice assistance

License

This project was developed for the Startlet Hackathon 2026 for educational and demonstration purposes.
