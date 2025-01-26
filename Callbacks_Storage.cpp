#include <iostream>
#include <functional>
#include <array>

// Example callbacks
void hello() { std::cout << "Hello!\n"; }
void world() { std::cout << "World!\n"; }

// Maximum number of callbacks
constexpr int MAX_CALLBACKS = 5;

// Storage for callbacks
std::array<std::function<void()>, MAX_CALLBACKS> callbacks = {};
int callbackCount = 0;

// Function to register a callback
void registerCallback(const std::function<void()>& callback) {
    if (callbackCount < MAX_CALLBACKS) {
        // Add the callback to the array
        callbacks[callbackCount++] = callback;
        std::cout << "Callback registered.\n";
    } else {
        std::cout << "Callback storage is full!\n";
    }
}

// Function to trigger all registered callbacks
void triggerCallbacks() {
    if (callbackCount == 0) {
        std::cout << "No callbacks registered.\n";
        return;
    }

    for (int i = 0; i < callbackCount; ++i) {
        callbacks[i]();  // Execute the callback
    }
}

// Function to unregister a callback
void unregisterCallback(const std::function<void()>& callback) {
    bool found = false;

    for (int i = 0; i < callbackCount; ++i) {
        if (callbacks[i].target<void()>() == callback.target<void()>()) {
            // Shift remaining elements to the left
            for (int j = i; j < callbackCount - 1; ++j) {
                callbacks[j] = callbacks[j + 1];
            }
            callbackCount--;
            found = true;
            std::cout << "Callback unregistered.\n";
            break;
        }
    }

    if (!found) {
        std::cout << "Callback not found.\n";
    }
}

int main() {
    // Register callbacks
    registerCallback(hello);
    registerCallback(world);

    // Trigger callbacks
    triggerCallbacks();

    // Unregister a callback
    unregisterCallback(hello);

    // Trigger again
    triggerCallbacks();

    return 0;
}
