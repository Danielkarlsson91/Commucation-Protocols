/* Predict nationality based on a name
Using HTTPClient library make a program for ESP32 to read names from the terminal and make requests to
https://api.nationalize.io/?name=XXXXXX The server will send a respose to a request. The program shall display the response in the terminal.

The web server gets the name and based on it predict the nationality.

For example: https://api.nationalize.io/?name=stefan will return

{"name":"stefan","country":[{"country_id":"AT","probability":0.1154051730300023},
{"country_id":"RS","probability":0.10833768504783693},{"country_id":"DE","probability":0.09098869138807936}]}*/

#include <WiFi.h>       // Include the WiFi library for WiFi functions
#include <HTTPClient.h> // Include the HTTPClient library for HTTP functions

#define MAX_LEN (32U)                                // Define a maximum length for the name input
#define SSID "Telia-1B01AD.2.4GHZ"                   // Enter the WiFi SSID here
#define PASSWORD "Elsie2022"                         // Enter the WiFi password here
#define URL_BASE "https://api.nationalize.io/?name=" // Base URL for the nationality prediction API

static HTTPClient http; // Create an HTTPClient object

// Function to clear the serial buffer
static void serial_clear(void)
{
  while (Serial.available()) // While there is data in the serial buffer
  {
    (void)Serial.read(); // Read and discard the data
  }
}

void setup()
{
  Serial.begin(9600); // Start the serial communication at 9600 baud rate

  WiFi.begin(SSID, PASSWORD);           // Start connecting to the WiFi network
  while (WL_CONNECTED != WiFi.status()) // Wait until the WiFi is connected
  {
    Serial.print("."); // Print a dot every second while waiting for the connection
    delay(1000);       // Wait for 1 second
  }
}

void loop()
{
  uint8_t i = sizeof(URL_BASE) - 1;                // Initialize the index to the length of the base URL
  char url[sizeof(URL_BASE) + MAX_LEN] = URL_BASE; // Create a URL buffer and initialize it with the base URL

  Serial.print("\nEnter a name: "); // Prompt the user to enter a name

  serial_clear();               // Clear any existing data in the serial buffer
  while (i < (sizeof(url) - 1)) // While there is space in the URL buffer
  {
    if (Serial.available()) // If there is data available in the serial buffer
    {
      char ch = Serial.read(); // Read a character from the serial buffer

      Serial.print(ch); // Echo the character back to the serial monitor

      if (ch == '\n') // If the character is a newline (end of input)
      {
        break; // Exit the loop
      }

      url[i++] = ch; // Add the character to the URL buffer and increment the index
    }
  }
  Serial.flush(); // Ensure all serial data is sent

  if (sizeof(URL_BASE) <= strlen(url)) // If the URL buffer contains more than just the base URL
  {
    if (http.begin(String(url))) // Start the HTTP connection to the URL
    {
      int status = http.GET(); // Perform the HTTP GET request

      if (status == HTTP_CODE_OK) // If the request was successful
      {
        Serial.println(http.getString()); // Print the response from the server
      }
      else // If the request failed
      {
        Serial.printf("[HTTP] GET failed, error code: %d", status); // Print the error code
      }

      http.end(); // End the HTTP connection
    }
    else // If unable to connect to the server
    {
      Serial.printf("[HTTP] Unable to connect to the server..."); // Print an error message
    }
  }
}
