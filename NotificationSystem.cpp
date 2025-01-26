#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

// A map to associate topics with callbacks
std::map<std::string, std::vector<std::function<void(const std::string&)>>> topicCallbacks;

// Function to register a callback for a specific topic
void registerNotification(const std::string& topic, const std::function<void(const std::string&)>& callback) {
    topicCallbacks[topic].push_back(callback); // Add the callback to the topic's vector
}

// Function to send a notification to all callbacks registered for a topic
void sendNotification(const std::string& topic, const std::string& message) {
    auto it = topicCallbacks.find(topic);
    if (it != topicCallbacks.end()) {
        // Execute all callbacks for this topic
        for (const auto& callback : it->second) {
            callback(message);
        }
    } else {
        std::cout << "No callbacks registered for topic: " << topic << "\n";
    }
}

// Function to unregister a specific callback from a topic
void unregisterNotification(const std::string& topic, const std::function<void(const std::string&)>& callback) {
    auto it = topicCallbacks.find(topic);
    if (it != topicCallbacks.end()) {
        auto& callbacks = it->second;
        // Find and remove the callback
        callbacks.erase(
            std::remove_if(callbacks.begin(), callbacks.end(),
                           [&callback](const std::function<void(const std::string&)>& cb) {
                               return cb.target<void(const std::string&)>() == callback.target<void(const std::string&)>();
                           }),
            callbacks.end());
    }
}

void sportsNews(const std::string& message) {
    std::cout << "Sports Notification: " << message << "\n";
}

void weatherUpdate(const std::string& message) {
    std::cout << "Weather Notification: " << message << "\n";
}

void generalNews(const std::string& message) {
    std::cout << "General News: " << message << "\n";
}

int main() {
    // Register callbacks for topics
    registerNotification("Sports", sportsNews);
    registerNotification("Weather", weatherUpdate);
    registerNotification("News", generalNews);

    // Send notifications
    std::cout << "Sending notifications:\n";
    sendNotification("Sports", "Your favorite team won!");
    sendNotification("Weather", "It's sunny today!");
    sendNotification("News", "Breaking: Important update!");

    // Unregister a callback
    unregisterNotification("Sports", sportsNews);

    // Send notifications again (Sports should not trigger)
    std::cout << "\nAfter unregistering Sports callback:\n";
    sendNotification("Sports", "Another sports update!");
    sendNotification("Weather", "Rain expected tomorrow!");
    sendNotification("News", "General news update.");

    return 0;
}
