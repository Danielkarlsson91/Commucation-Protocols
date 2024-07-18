/*Make a program using UART over BLE to control (on/off) a LED connected to pin 21 on ESP32.

Use nRF Toolbox or Bluetooth for Arduino app on your phone as the client.
*/

#include <BLEDevice.h> // Include the BLEDevice library
#include <BLEServer.h> // Include the BLEServer library
#include <BLEUtils.h>  // Include the BLEUtils library
#include <BLE2902.h>   // Include the BLE2902 library
#include <Arduino.h>   // Include Arduino library

#define LED_PIN 21 // Define the pin number for the LED

BLEServer *pServer = NULL;          // Create a pointer to the BLEServer object
BLECharacteristic *pCharacteristic; // Create a pointer to the BLECharacteristic object

bool deviceConnected = false; // Flag to check if a device is connected
uint8_t led_state = 0;        // Variable to store the LED state (0 = OFF, 1 = ON)

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"           // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" // RX characteristic UUID
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" // TX characteristic UUID

// Callback class for BLE server events
class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true; // Set the flag when a device is connected
  }

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false; // Clear the flag when a device is disconnected
  }
};

// Callback class for BLE characteristic events
class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string rxValue = pCharacteristic->getValue(); // Get the value written to the characteristic

    if (rxValue.length() > 0)
    {
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++)
      {
        Serial.print(rxValue[i]);
      }
      Serial.println();

      if (rxValue == "ON")
      {
        digitalWrite(LED_PIN, HIGH); // Turn the LED on
        led_state = 1;
      }
      else if (rxValue == "OFF")
      {
        digitalWrite(LED_PIN, LOW); // Turn the LED off
        led_state = 0;
      }
    }
  }
};

void setup()
{
  Serial.begin(115200);       // Start serial communication at 115200 baud rate
  pinMode(LED_PIN, OUTPUT);   // Set the LED pin as an output
  digitalWrite(LED_PIN, LOW); // Initialize the LED to be off

  BLEDevice::init("ESP32_LED"); // Initialize the BLE device with the name "ESP32_LED"

  pServer = BLEDevice::createServer();            // Create the BLE server
  pServer->setCallbacks(new MyServerCallbacks()); // Set the server callbacks

  BLEService *pService = pServer->createService(SERVICE_UUID); // Create the UART service

  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE); // Create the RX characteristic

  pCharacteristic->setCallbacks(new MyCallbacks()); // Set the chracteristic callbacks

  pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY // Create the TX characteristic
                                 )
      ->addDescriptor(new BLE2902()); // Add a descriptor to the TX characteristic

  pService->start(); // Start the service

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising(); // Start advertising
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // Set advertising parameters
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising(); // Start advertising the BLE service

  Serial.println("Waiting for a client connection...");
}

void loop()
{
  // You can add code here if you needed to perform other tasks while the BLE server is running
}
