/*Make a program for ESP32 to get the current date and time from an NTP server and initialize the RTC of ESP32.

Then make a program for FRDM-K64F which has a menu system with 2 options:

S) Synchronize Date and Time – To get the current date and time from ESP32 via I2C and set date and time of the RTC of FRDM-K64F

D) Display The Current Date and Time - To read the current date and time of the RTC of FRDM-K64F and display it in the terminal

Date and Time format shall be YYYY-MM-DD hh:mm:ss
*/

#include <WiFi.h> // Include the WiFi library for WiFi connection
#include <time.h> // Include the time library for working with date and time

// Define constants for WiFi credentials
#define SSID "Telia-1B01AD.2.4GHZ" // The WiFi network SSID
#define PASSWORD "Elsie2022"       // The WiFi password

// Define the time zone offset for your location (e.g., UTC+1 hour)
#define TIMEZONE_OFFSET 1 * 3600 // 1 hour in seconds

// Define NTP server
#define NTP_SERVER "pool.ntp.org" // NTP server address

void setup()
{
  Serial.begin(115200); // Initialize serial communication at 115200 baud rate

  // Connect to WiFi network
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("."); // Print a dot to show connection is in progress
    delay(500);        // Wait 500ms and check WiFi connection again
  }
  Serial.println("\nConnected to WiFi"); // Indicate successful connection

  // Configure time with the specified NTP server and time zone offset
  configTime(TIMEZONE_OFFSET, 0, NTP_SERVER);
  struct tm timeinfo; // Declare a struct tm to hold date and time information

  // Wait until time synchronization is complete
  while (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time. Retrying...");
    delay(1000); // Retry every second if time synchronization fails
  }
  Serial.println("Time synchronized!"); // Indicate successful time synchronization

  // Print the current date and time
  char datetime[20]; // Array to hold formatted date and time string
  strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", &timeinfo);
  Serial.println("Current Date and Time: " + String(datetime));
}

void loop()
{
}