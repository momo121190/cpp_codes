// This program demonstrates the usage of a temperature monitor class that
// registers a callback function to detect high temperature events and
// simulates periodic temperature monitoring.

#include <iostream> // For standard input/output operations
#include <functional> // For std::function
#include <chrono> // For time-related functions
#include <thread> // For std::this_thread::sleep_for

class TemperatureMonitor {
public:
    // Type of callback function to detect high temperature events
    using HighTemperatureCallback = std::function<void(double)>;

    // Method to register a callback function to detect high temperatures
    void registerHighTemperatureCallback(HighTemperatureCallback callback) {
        highTemperatureCallback = callback;
    }

    // Method to simulate periodic temperature monitoring
    void startMonitoring() {
        while (true) {
            // Simulating temperature reading
            double temperature = readTemperature();
            // Checking if the temperature exceeds the specified threshold (30 degrees Celsius)
            if (temperature > 30.0) {
                // Calling the callback function for high temperature
                highTemperatureCallback(temperature);
            }
            // Waiting before the next temperature check
            std::this_thread::sleep_for(std::chrono::seconds(5)); // Simulating a 5-second interval
        }
    }

private:
    // Callback function for high temperatures
    HighTemperatureCallback highTemperatureCallback;

    // Simulation method to read the temperature
    double readTemperature() {
        // Simulation: generating a random temperature between 20 and 35 degrees Celsius
        return 20.0 + (rand() % 150) / 5.0;
    }
};

int main() {
    // Creating an instance of TemperatureMonitor
    TemperatureMonitor temperatureMonitor;

    // Registering a lambda function as the callback function for high temperature
    temperatureMonitor.registerHighTemperatureCallback(
        [](double temperature) {
            std::cout << "Warning: High temperature detected! Temperature: " << temperature << " °C" << std::endl;
        }
    );

    // Starting temperature monitoring
    temperatureMonitor.startMonitoring();

    return 0;
}
