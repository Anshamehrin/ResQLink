/*
=========================================================
ResQLink - ESP32 SOS Badge
Beginner Friendly Version

What it does:
1. Shows "SAFE" on OLED.
2. Waits for button press.
3. Reads GPS location.
4. Sends location through Bluetooth.
5. Blinks LED and buzzer.
6. Displays "SOS SENT" on OLED.
=========================================================
*/

// LIBRARIES

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <TinyGPSPlus.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// PIN DEFINITIONS

const int BUTTON_PIN = 18;
const int LED_PIN = 2;
const int BUZZER_PIN = 5;

const int OLED_WIDTH = 128;
const int OLED_HEIGHT = 64;


// Bluetooth UUIDs

const char* SERVICE_ID =
"4fafc201-1fb5-459e-8fcc-c5c9c331914b";

const char* CHARACTERISTIC_ID =
"beb5483e-36e1-4688-b7f5-ea07361b26a8";



// OBJECTS
// OLED

Adafruit_SSD1306 oledScreen(
OLED_WIDTH,
OLED_HEIGHT,
&Wire,
-1);

// GPS

TinyGPSPlus gpsReader;
HardwareSerial gpsSerial(2);

// Bluetooth

BLECharacteristic *bleMessage;


// VARIABLES


bool buttonWasPressed = false;

unsigned long lastPressTime = 0;


// FUNCTION PROTOTYPES

void setupButtonAndAlarm();

bool readButtonPress();

void setupScreen();

void updateScreen(String bigLine, String smallLine);

void setupGPS();

void keepReadingGPS();

String getLocationText();

void setupBluetooth();

void sendSOS();

// SETUP

void setup()
{
    Serial.begin(115200);

    setupButtonAndAlarm();

    setupScreen();

    setupGPS();

    setupBluetooth();

    updateScreen("SAFE", "Waiting...");

    Serial.println("Badge Ready");
}


// LOOP

void loop()
{
    // Keep GPS updated

    keepReadingGPS();

    // Check button

    if (readButtonPress())
    {
        sendSOS();
    }
}

// BUTTON, LED AND BUZZER FUNCTIONS

// Initialize button, LED and buzzer
void setupButtonAndAlarm()
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    pinMode(LED_PIN, OUTPUT);

    pinMode(BUZZER_PIN, OUTPUT);

    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
}


// Check whether button is pressed
// Returns true only once for each press
bool readButtonPress()
{
    bool isPressedNow = (digitalRead(BUTTON_PIN) == LOW);

    unsigned long currentTime = millis();

    // Simple software debounce
    if (isPressedNow &&
        !buttonWasPressed &&
        (currentTime - lastPressTime > 250))
    {
        buttonWasPressed = true;
        lastPressTime = currentTime;

        return true;
    }

    // Reset when button is released
    if (!isPressedNow)
    {
        buttonWasPressed = false;
    }

    return false;
}

// OLED DISPLAY FUNCTIONS

// Initialize OLED display
void setupScreen()
{
    if (!oledScreen.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println("OLED not found!");

        while (true)
        {
            // Stop here if OLED is not detected
        }
    }

    oledScreen.clearDisplay();
    oledScreen.display();
}



// Display two lines of text
void updateScreen(String bigLine, String smallLine)
{
    oledScreen.clearDisplay();

    // Large heading
    oledScreen.setTextSize(2);
    oledScreen.setTextColor(SSD1306_WHITE);
    oledScreen.setCursor(0, 0);
    oledScreen.println(bigLine);

    // Small message
    oledScreen.setTextSize(1);
    oledScreen.setCursor(0, 35);
    oledScreen.println(smallLine);

    oledScreen.display();
}
// GPS FUNCTIONS

// Initialize GPS module
void setupGPS()
{
    // GPS TX -> ESP32 GPIO16
    // GPS RX -> ESP32 GPIO17

    gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
}


// Continuously read GPS data
void keepReadingGPS()
{
    while (gpsSerial.available() > 0)
    {
        gpsReader.encode(gpsSerial.read());
    }
}


// Get current GPS location
String getLocationText()
{
    if (gpsReader.location.isValid())
    {
        String latitude = String(gpsReader.location.lat(), 6);
        String longitude = String(gpsReader.location.lng(), 6);

        return "Lat:" + latitude + ", Lng:" + longitude;
    }

    return "Location not found";
}

// BLUETOOTH FUNCTIONS

// Initialize Bluetooth Low Energy (BLE)
void setupBluetooth()
{
    BLEDevice::init("ResQLink-Badge");

    BLEServer *bleServer = BLEDevice::createServer();

    BLEService *bleService =
        bleServer->createService(SERVICE_ID);

    bleMessage = bleService->createCharacteristic(
        CHARACTERISTIC_ID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    bleMessage->addDescriptor(new BLE2902());

    bleMessage->setValue("READY");

    bleService->start();

    BLEAdvertising *bleAdvertising =
        BLEDevice::getAdvertising();

    bleAdvertising->addServiceUUID(SERVICE_ID);

    bleAdvertising->setScanResponse(true);

    BLEDevice::startAdvertising();

    Serial.println("Bluetooth Ready");
}

// SOS FUNCTION

// This function is called whenever the SOS button is pressed.
void sendSOS()
{
    Serial.println("SOS TRIGGERED!");

    // Get the latest GPS location
    String location = getLocationText();

    // Show message on OLED
    updateScreen("SOS!", "Sending Help...");

    // Send GPS location through Bluetooth
    bleMessage->setValue(location.c_str());
    bleMessage->notify();

    // Blink LED and beep buzzer 5 times
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);

        delay(200);

        digitalWrite(LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);

        delay(200);
    }

    // Display confirmation
    updateScreen("SOS SENT", location);

    // Send again after a short delay
    delay(500);

    bleMessage->setValue(location.c_str());
    bleMessage->notify();

    Serial.println("SOS SENT SUCCESSFULLY");
}