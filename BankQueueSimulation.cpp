#include <iostream>
#include <queue>
#include <chrono>
#include <ctime>
#include <string>

class Bank_Queue_Simulation {
private:
    // Queue to store customers and their service times (in minutes).
    std::queue<std::pair<std::string, int>> q;  // {customer name, service time in minutes}
    int totalServiceTime = 0;  // Keeps track of the total service time of all customers
    int customerCount = 0;     // Number of customers added to the queue

public:
    Bank_Queue_Simulation() {}

    // Add a customer to the queue with their service time (in minutes)
    void addCustomer(const std::string& name, int service_time) {
        q.push(std::make_pair(name, service_time));
        totalServiceTime += service_time;
        customerCount++;
    }

    // Serve the next customer in the queue
    void serveCustomer() {
        if (!q.empty()) {
            auto customer = q.front();
            std::cout << "Serving customer: " << customer.first 
                      << " (Service time: " << customer.second << " minutes)\n";
            q.pop();
        } else {
            std::cout << "No customers to serve.\n";
        }
    }

    // Peek at the next customer without removing them
    void peekNextCustomer() {
        if (!q.empty()) {
            auto customer = q.front();
            std::cout << "Next customer to be served: " << customer.first 
                      << " (Service time: " << customer.second << " minutes)\n";
        } else {
            std::cout << "No customers in the queue.\n";
        }
    }

    // Check if the queue is empty
    bool isQueueEmpty() {
        return q.empty();
    }

    // Get the average service time of customers
    double getAverageServiceTime() {
        if (customerCount == 0) {
            return 0;  // Avoid division by zero if no customers are added
        }
        return static_cast<double>(totalServiceTime) / customerCount;
    }

    // Get the number of customers in the queue
    int getQueueSize() {
        return q.size();
    }
};

int main() {
    Bank_Queue_Simulation bankQueue;

    // Add some customers to the queue
    bankQueue.addCustomer("John Doe", 5);  // 5 minutes
    bankQueue.addCustomer("Jane Smith", 10);  // 10 minutes
    bankQueue.addCustomer("Alex Johnson", 3);  // 3 minutes

    // Peek at the next customer
    bankQueue.peekNextCustomer();

    // Serve customers
    bankQueue.serveCustomer();
    bankQueue.serveCustomer();

    // Check if the queue is empty
    std::cout << "Is the queue empty? " << (bankQueue.isQueueEmpty() ? "Yes" : "No") << "\n";

    // Serve the remaining customers
    bankQueue.serveCustomer();

    // Print average service time
    std::cout << "Average service time: " << bankQueue.getAverageServiceTime() << " minutes\n";

    return 0;
}
