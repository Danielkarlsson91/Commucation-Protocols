/*Bluetooth - UART over BLE

Make a program for ESP32 as a BLE Server to receive a string from a client and convert it to uppercase

and then return it to the client. Use nRF Toolbox or Bluetooth for Arduino on your phone as the client.
*/

#include <Arduino.h>   // Include the Arduino core library
#include <BLEDevice.h> // Include the BLE device library
#include <BLEUtils.h>  // Include the BLE utilities library
#include <BLEServer.h> // Include the BLE server library
#include <BLE2902.h>   // Include the BLE2902 library (for descriptors)

// UUIDs for the BLE service and characteristics
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"           // UUID for the service
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" // UUID for the RX characteristic
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" // UUID for the TX characteristic

// Create a BLE server
BLEServer *pServer = nullptr;                   // Pointer to the BLE server object
BLECharacteristic *pTxCharacteristic = nullptr; // Pointer to the TX characteristic
bool deviceConnected = false;                   // Flag to check if a device is connected

// Function to convert a string to uppercase
std::string toUpperCase(const std::string &str)
{
  std::string upperStr = str;               // Create a copy of the input string
  for (size_t i = 0; i < str.length(); i++) // Iterate over each character in the string
  {
    upperStr[i] = toupper(str[i]); // Convert the character to uppercase
  }
  return upperStr; // Return the uppercase string
}

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

// Callback class for BLE characteristics events
class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string rxValue = pCharacteristic->getValue(); // Get the value written to the characteristic

    if (rxValue.length() > 0) // If there is data
    {
      Serial.print("Received Value: "); // Print "Received Value: " to the Serial Monitor
      Serial.println(rxValue.c_str());  // Print the received value to the Serial Monitor

      // Convert the received string to uppercase
      std::string upperStr = toUpperCase(rxValue);

      // Send the uppercase string back to the client
      pTxCharacteristic->setValue(upperStr); // Set the TX characteristic value to the uppercase string
      pTxCharacteristic->notify();           // Notify the client that the TX characteristic value has changed
      Serial.print("Sent Value: ");          // Print "Sent Value: " to the Serial Monitor
      Serial.println(upperStr.c_str());      // Print the sent value to the Serial Monitor
    }
  }
};

void setup()
{
  Serial.begin(115200); // Initialize serial communication at 115200 baud rate

  // Initialize BLE
  BLEDevice::init("ESP32_BLE_SERVER");            // Initialize the BLE device with the name "ESP32_BLE_Server"
  pServer = BLEDevice::createServer();            // Create the BLE server
  pServer->setCallbacks(new MyServerCallbacks()); // Set the server callbacks

  // Create the BLE service
  BLEService *pService = pServer->createService(SERVICE_UUID); // Create the service with the specified UUID

  // Create the TX characteristic
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY); // Set the TX characteristic as notify property

  pTxCharacteristic->addDescriptor(new BLE2902()); // Add a descriptor to the TX characteristic

  // Create the RX characteristic
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE); // Set the RX characteristic as write property

  pRxCharacteristic->setCallbacks(new MyCallbacks()); // Set the RX characteristic callbacks

  // Start the service
  pService->start(); // Start the BLE service

  pServer->getAdvertising()->start();                   // Start BLE advertising
  Serial.println("Waiting for a client connection..."); // Print a message to the Serial Monitor
}

void loop()
{
  delay(1000); // Sleep for 1000 milliseconds(1 second)
}
