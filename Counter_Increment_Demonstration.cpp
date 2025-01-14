#include <iostream>

class Counter {
private:
    int value;

public:
    // Constructor to initialize the counter
    Counter(int val = 0) : value(val) {}

    // Pre-increment operator (++Counter)
    Counter& operator++() {
        ++value;
        return *this;
    }

    // Post-increment operator (Counter++)
    Counter operator++(int) {
        Counter temp = *this; // Save current state
        ++value;              // Increment the value
        return temp;          // Return the old state
    }

    // Function to display the current value
    void display() const {
        std::cout << "Counter value: " << value << std::endl;
    }
};

int main() {
    Counter c(5);

    std::cout << "Initial value: ";
    c.display();

    std::cout << "After pre-increment (++c): ";
    ++c; // Pre-increment
    c.display();

    std::cout << "After post-increment (c++): ";
    c++; // Post-increment
    c.display();

    return 0;
}
