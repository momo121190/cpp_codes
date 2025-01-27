#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <algorithm>

// Enum for event types
enum class EventType {
    START,
    STOP,
    PAUSE,
    RESUME
};

// Example callback functions
void onStart() { std::cout << "Start Event Triggered!\n"; }
void onStop() { std::cout << "Stop Event Triggered!\n"; }
void onPause() { std::cout << "Pause Event Triggered!\n"; }

// Event dispatcher to store and manage callbacks
std::map<EventType, std::vector<std::function<void()>>> Map_Callback;

// Function to register a callback for a specific event
void registerCallback(EventType event, const std::function<void()>& callback) {
    Map_Callback[event].push_back(callback); // Add callback to the vector for the event
}

// Function to trigger all callbacks registered for a specific event
void triggerCallbacks(EventType event) {
    auto it = Map_Callback.find(event);
    if (it != Map_Callback.end()) {
        for (const auto& callback : it->second) {
            callback(); // Execute each registered callback
        }
    } else {
        std::cout << "No callbacks registered for this event.\n";
    }
}

// Function to unregister a specific callback for an event
void unregisterCallback(EventType event, const std::function<void()>& callback) {
    auto it = Map_Callback.find(event);
    if (it != Map_Callback.end()) {
        auto& callbacks = it->second;
        callbacks.erase(
            std::remove_if(callbacks.begin(), callbacks.end(),
                           [&callback](const std::function<void()>& cb) {
                               // Compare the target functions
                               return cb.target<void()>() == callback.target<void()>();
                           }),
            callbacks.end());
    }
}

int main() {
    // Create an event dispatcher
    // Register callbacks for events
    registerCallback(EventType::START, onStart);
    registerCallback(EventType::STOP, onStop);
    registerCallback(EventType::PAUSE, onPause);

    // Trigger events
    std::cout << "Triggering START event:\n";
    triggerCallbacks(EventType::START);

    std::cout << "Triggering STOP event:\n";
    triggerCallbacks(EventType::STOP);

    std::cout << "Triggering PAUSE event:\n";
    triggerCallbacks(EventType::PAUSE);

    // Unregister a callback
    unregisterCallback(EventType::START, onStart);

    // Trigger START event again to check if the callback was unregistered
    std::cout << "Triggering START event after unregistering onStart:\n";
    triggerCallbacks(EventType::START);

    return 0;
}
