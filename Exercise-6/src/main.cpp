/*Using WebServer library make a web server to control a LED connected to pin 21 on ESP32. The web server shall serve 3 URLs

    http://<ip_address_of_your_esp32>/              => Turns the LED off
    http://<ip_address_of_your_esp32>/led/on    => Turns the LED on
    http://<ip_address_of_your_esp32>/led/off    => Turns the LED off

If you are familiar with html and CSS, make a simple html page which has a text to show the current state of the LED and a link to click to change the state of the LED.*/

#include <WiFi.h>      // Include the WiFi library for WiFi functions
#include <WebServer.h> // Include the WebServer library for creating a web server

#define PORT (80U)                 // Define the port number for the web server
#define SSID "Telia-1B01AD.2.4GHZ" // Enter the WiFi SSID here
#define PASSWORD "Elsie2022"       // Enter the WiFi password here

// Function prototypes for handling different HTTP requests
static void handle_index(void);
static void handle_ledon(void);
static void handle_ledoff(void);
static void handle_not_found(void);
static String response(uint8_t state); // Function prototype for generating HTML response

static WebServer server(PORT);  // Create a web server object on the specified port
static uint8_t led_state = LOW; // Variable to keep track of the LED state (LOW=OFF, HIGH=ON)

void setup()
{
  pinMode(GPIO_NUM_21, OUTPUT);   // Set GPIO pin 21 as an output pin
  digitalWrite(GPIO_NUM_21, LOW); // Set GPIO pin 21 to LOW (turn off LED)

  Serial.begin(9600); // Start serial communication at 9600 baud rate
  delay(3000);        // Wait for 3 seconds

  WiFi.begin(SSID, PASSWORD);           // Start connecting to the WiFi
  while (WL_CONNECTED != WiFi.status()) // Wait until the WiFi is connected
  {
    Serial.print("."); // Print a dot every second while waiting for the connection
    delay(1000);       // Wait for 1 second
  }

  server.on("/", handle_index);         // Define the function to handle requests to the root URL "/"
  server.on("/led/on", handle_ledon);   // Define the function to handle requests to "/led/on"
  server.on("/led/off", handle_ledoff); // Define the function to handle requests to "/led/off"
  server.onNotFound(handle_not_found);  // Define the function to handle requests to unknown URLs

  server.begin(PORT); // Start the web server on the specified port

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP().toString()); // Print the IP address of the ESP32

  Serial.println("HTTP server started"); // Indicate that the HTTP server has started
}

void loop()
{
  server.handleClient(); // Handle incoming client requests

  if (HIGH == led_state) // Check if the LED state is HIGH
  {
    digitalWrite(GPIO_NUM_21, HIGH); // Turn on the LED
  }
  else // If the LED state is LOW
  {
    digitalWrite(GPIO_NUM_21, LOW); // Turn off the LED
  }
}

static void handle_index(void)
{
  led_state = LOW;                                    // Set the LED state to LOW (turn off LED)
  Serial.println("LED State: OFF");                   // Print the LED state to the serial monitor
  server.send(200, "text/html", response(led_state)); // Send an HTTP response with the LED state
}

static void handle_ledon(void)
{
  led_state = HIGH;                                   // Set the LED state to HIGH (turn on LED)
  Serial.println("LED State: ON");                    // Print the LED state to the serial monitor
  server.send(200, "text/html", response(led_state)); // Send an HTTP response with the LED state
}
static void handle_ledoff(void)
{
  led_state = LOW;                                    // Set the LED state to LOW (turn off LED)
  Serial.println("LED State: OFF");                   // Print the LED state to the serial monitor
  server.send(200, "text/html", response(led_state)); // Send an HTTP response with the LED state
}
static void handle_not_found(void)
{
  server.send(404, "text/plain", "Not found"); // Send a 404 Not Found response
}

static String response(uint8_t state)
{
  String ptr = "<DOCTYPE html><html\n";                                                                                                                                                                                   // Start the HTML document
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";                                                                                                          // Set the viewport
  ptr += "<title>LED Control</title>\n";                                                                                                                                                                                  // Set the title of the webpage
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";                                                                                                         // Style the HTML
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";                                                                                                // Style the body
  ptr += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n"; // Style the button
  ptr += ".button-on {background-color: #3498db;}\n";                                                                                                                                                                     // Style the ON button
  ptr += ".button-on:active {background-color: #2980b9;}\n";                                                                                                                                                              // Style the active ON button
  ptr += ".button-off {background-color: #34495e;}\n";                                                                                                                                                                    // Style the OFF button
  ptr += ".button-off:active {background-color: #2c3e50;}\n";                                                                                                                                                             // Style the active OFF button
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";                                                                                                                                                        // Style the paragraph text
  ptr += "</style>\n";                                                                                                                                                                                                    // Close the style tag
  ptr += "</head>\n";                                                                                                                                                                                                     // Close the head tag
  ptr += "<body>\n";                                                                                                                                                                                                      // Start the body tag
  ptr += "<h1>ESP32 Web Server</h1>\n";                                                                                                                                                                                   // Add a heading to the webpage

  if (state == HIGH) // If the LED is ON
  {
    ptr += "<p>LED State: ON</p><a class=\"button button-off\" href=\"/led/off\">OFF</a>\n"; // Add a paragraph and an OFF button
  }
  else // If the LED is OFF
  {
    ptr += "<p>LED State: OFF</p><a class=\"button button-on\" href=\"/led/on\">ON</a>\n"; // Add a paragraph and an ON button
  }

  ptr += "</body>\n"; // Close the body tag
  ptr += "</html>\n"; // Close the HTML document

  return ptr; // Return the HTML string
}