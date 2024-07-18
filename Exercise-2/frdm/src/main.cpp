#include <mbed.h>
#include <time.h>

// Constants
constexpr int LENGTH{19};        // Length of the expected date and time string
constexpr char SYNCHRONIZE{'S'}; // Command to synchronize date and time
constexpr char DISPLAY{'D'};     // Command to display date and time

// I2C pins
constexpr PinName SDA_PIN = PTC11; // I2C SDA pin (change if needed)
constexpr PinName SCL_PIN = PTC10; // I2C SCL pin (change if needed)

// I2C object
I2C i2c(SDA_PIN, SCL_PIN); // Create an I2C object for communication with ESP32

// BufferedSerial object for serial communication
BufferedSerial serial(USBTX, USBRX, 115200); // Serial communication at 115200  baud rate

// Function to synchronize date and time
void synchronize_datetime()
{
    // Create an array to store the date and time string received from ESP32
    char datetime[LENGTH + 1] = {0};

    // Send the 'S' command to ESP32 via I2C
    char command = SYNCHRONIZE;
    i2c.write(0x10, &command, 1); // 0x10 is the I2C address of ESP32(change if necessary)

    // Read date and time string from ESP32 via I2C
    i2c.read(0x10, datetime, LENGTH);

    // Parse the date and time string
    int yr, mt, dy, hr, mn, sc;
    if (sscanf(datetime, "%04d-%02d-%02d %02d:%02d:%02d", &yr, &mt, &dy, &hr, &mn, &sc) == 6)
    {
        // Set the system date and time
        struct tm new_time = {0};
        new_time.tm_year = yr - 1900;
        new_time.tm_mon = mt - 1;
        new_time.tm_mday = dy;
        new_time.tm_hour = hr;
        new_time.tm_min = mn;
        new_time.tm_sec = sc;

        // Convert struct tm to time_t
        time_t epoch_time = mktime(&new_time);
        set_time(epoch_time);

        // Confirm successful synchronization to the user
        serial.write("Synchronized successfully!\n", 25);
    }
    else
    {
        // Indicate synchronization failure to the user
        serial.write("Synchronization failed!\n", 23);
    }
}

// Function to display the current date and time
void display_datetime()
{
    // Get the current time as a time_t
    time_t current_time = time(NULL);

    // Convert the time_t to a struct tm
    struct tm *local_time = localtime(&current_time);

    // Format the date and time and send it to the serial port
    char buffer[20]; // Buffer to hold formatted date and time string
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d\n", local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_min);
    serial.write(buffer, strlen(buffer)); // Send formatted date and time
}

int main()
{
    while (true)
    {
        // Prompt the user for input
        serial.write("Enter a command (S: Synchronize, D: Display): ", 44);

        // Read user input
        char command;
        serial.read(&command, sizeof(command));

        // Handle the user's input
        switch (command)
        {
        case SYNCHRONIZE:
            synchronize_datetime(); // Synchronize date and time
            break;
        case DISPLAY:
            display_datetime(); // Display date and time
            break;
        default:
            // Invalid command entered by user
            serial.write("Invalid command!\n", 18);
            break;
        }
    }
}