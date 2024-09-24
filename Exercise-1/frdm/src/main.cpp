/*Make a program for ESP32 to get the current date and time from an NTP server and initialize the RTC of ESP32.

Then make a program for FRDM-K64F which has a menu system with 2 options:

S) Synchronize Date and Time – To get the current date and time from ESP32 via UART and set date and time of the RTC of FRDM-K64F

D) Display The Current Date and Time - To read the current date and time of the RTC of FRDM-K64F and display it in the terminal


Date and Time format shall be YYYY-MM-DD hh:mm:ss
*/

#include <mbed.h>

int main()
{
    // Constants
    constexpr int LENGTH{19};        // Length of the expected date-time string from UART
    constexpr char DISPLAY{'D'};     // Character command for displaying date-time
    constexpr char SYNCHRONIZE{'S'}; // Character command for synchronization date-time

    // Buffer for storing data
    char buffer[64]{0};       // Initialize a buffer with zeroes
    bool show_message = true; // Flag for whether to show the user message

    // BufferedSerial objects for serial communication
    BufferedSerial UART{PTC17, PTC16, 9600};   // UART communication with a baud rate of 9600
    BufferedSerial serial{USBTX, USBRX, 9600}; // USB serial communication with a baud rate of 9600

    while (1)
    {
        // Display message to user if flag is set
        if (show_message)
        {
            show_message = false; // Reset flag
            // Display command options to user
            std::strcpy(buffer, "Enter a command [D: Display, S: Synchronize]: ");
            serial.write(buffer, strlen(buffer));
        }

        // Check if serial data is available to read
        if (serial.readable())
        {
            char chr{0};                    // Variable to hold the received character
            serial.read(&chr, sizeof(chr)); // Read a character from serial
            chr = toupper(chr);             // Convert character to uppercase

            // Check if command is DISPLAY
            if (chr == DISPLAY)
            {
                // Get current time in seconds
                time_t seconds = time(nullptr);
                // Format the current date and time
                strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S\n", localtime(&seconds));
                // Send formatted date and time to serial
                serial.write(buffer, strlen(buffer));
            }

            // Check if command is SYNCHRONIZE
            else if (chr == SYNCHRONIZE)
            {
                // Send the synchronize command character to UART
                UART.write(&chr, sizeof(chr));

                // Reset buffer and read data from UART
                memset(buffer, 0, sizeof(buffer));
                int i = 0;
                while (i < LENGTH)
                {
                    if (UART.readable())
                    {
                        // Read characters from UART and store in buffer
                        UART.read(&buffer[i], sizeof(buffer[i]));
                        i++;
                    }
                }

                // Parse the reveived date-time string
                int yr = 0, mt = 0, dy = 0, hr = 0, mn = 0, sc = 0;
                if (6 == sscanf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", &yr, &mt, &dy, &hr, &mn, &sc))
                {
                    // Populate a struct tm with parsed date-time information
                    struct tm timeinfo;
                    timeinfo.tm_sec = sc;
                    timeinfo.tm_min = mn;
                    timeinfo.tm_hour = hr;
                    timeinfo.tm_mday = dy;
                    timeinfo.tm_mon = mt - 1;
                    timeinfo.tm_year = yr - 1900;

                    // Convert struct tm to time_t and set the system time
                    time_t seconds{mktime(&timeinfo)};
                    set_time(seconds);

                    // Send confirmation message to serial
                    std::strcpy(buffer, "Synchronized!\n");
                    serial.write(buffer, strlen(buffer));
                }
                else
                {
                    // Send error message if date-time format is invalid
                    std::strcpy(buffer, "Invalid datetime format!\n");
                    serial.write(buffer, strlen(buffer));
                }
            }
            else
            {
                // If newline character is reveived, send it back to serial
                if (chr == '\n')
                {
                    serial.write(&chr, sizeof(chr));
                }
            }

            // Set flag to show message again if a valid command was processed
            show_message = (chr == DISPLAY) || (chr == SYNCHRONIZE) || (chr == '\n');
        }
    }
    // Return statement for main function (should not be reached in an embedded system)
    return 0;
}