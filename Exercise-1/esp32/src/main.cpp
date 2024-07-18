/*Make a program for ESP32 to get the current date and time from an NTP server and initialize the RTC of ESP32.

Then make a program for FRDM-K64F which has a menu system with 2 options:

S) Synchronize Date and Time – To get the current date and time from ESP32 via UART and set date and time of the RTC of FRDM-K64F

D) Display The Current Date and Time - To read the current date and time of the RTC of FRDM-K64F and display it in the terminal


Date and Time format shall be YYYY-MM-DD hh:mm:ss
*/

#include <time.h>    // Include the time library for working with date and time
#include <WiFi.h>    // Include the WiFi library for handling WiFi connection
#include <Arduino.h> // Include the Arduino library

// Define constants
#define LENGTH 19       // Length of the date and time string (format: YYYY-MM-DD hh:mm:ss)
#define UART Serial1    // Define UART as Serial1 (used for communication with the other device)
#define SYNCRHONIZE 'S' // Define the synchronization character as 'S'

// Define WiFi credentials
#define SSID "Telia-1B01AD.2.4GHZ" // WiFi network name (SSID)
#define PASSWORD "Elsie2022"       // Wifi password

// Define time offsets and NTP server
#define TIME_OFFSET (1 * 60 * 60)      // Offset from GMT in seconds. For Sweden, it is 1 hour (3600 seconds)
#define TIME_DAYLIGHT (1 * 60 * 60)    // Daylight saving time in Sweden (summer time)
#define NTP_SERVER "0.se.pool.ntp.org" // Address of the NTP server in Sweden

void setup()
{
  Serial.begin(115200);
  UART.begin(9600); // Initialize UART communication at 9600 baud rate

  // Connect to the WiFi network
  WiFi.begin(SSID, PASSWORD);
  while (WL_CONNECTED != WiFi.status())
  {
    Serial.print(".");
    delay(500); // Wait until WiFi is connected, cheing every 500 millisecond
  }

  // Initialize time variables
  struct tm timeinfo = {0};
  // Configure time with specified offset and daylight savings using the NTP server
  configTime(TIME_OFFSET, TIME_DAYLIGHT, NTP_SERVER);
  while (!getLocalTime(&timeinfo))
  {
    // Wait until the local time is obtained from the NTP server
  }

  // Disconnected from WiFi since we are done with time synchronization
  WiFi.disconnect();
}

void loop()
{

  char chr;
  // Check if any data is available in the UART buffer
  if (UART.available() > 0)
  {
    chr = UART.read(); // Read the character from UART
  }

  // If the character is the synchronization character
  if (chr == SYNCRHONIZE)
  {
    time_t now = 0;                  // Declare a time variable to hold the current time
    char datetime[LENGTH + 1] = {0}; // Array to hold the date and time

    time(&now);                            // Get the current time
    struct tm *timeinfo = localtime(&now); // Convert time to a local time structure

    // Format the date and time as a string (format: YYYY-MM-DD HH:MM:SS)
    strftime(datetime, LENGTH, "%Y-%m-%d %H:%M:%S", timeinfo);

    // Send the formatted date and time string through UART
    UART.write(datetime, LENGTH);
  }
}