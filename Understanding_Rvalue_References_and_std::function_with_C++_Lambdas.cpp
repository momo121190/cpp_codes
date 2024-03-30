/*
The function main is called.

Inside main, the function takeQfunc is called with a lambda function as an argument.

In takeQfunc:
    - An object of type Q is created locally. Its default constructor is called, which prints "D".
    - Then, the func method of the object Q is called, printing the current value of v, which is initially 0 (prints "0").
    - The lambda function is called with an rvalue reference to the local object Q as an argument.
        - When executing the lambda function, a temporary object of type Q is created as a copy of the local object Q (prints "C").
        - The change method of the temporary object Q is called, incrementing the value of v of the temporary object (which is now 1).
    - The func method of the local object Q is called again, printing the current value of v, which is still 0 (prints "0").

So, the complete execution order is: D0C0.
*/

#include<iostream>
#include<functional>

// Class representing an object Q
class Q {
    int v = 0; // Initialize v to 0
public:
    // Constructor with move semantics
    Q(Q&&) {
        std::cout << "M"; // Print M when object is moved
    }
    
    // Copy constructor
    Q(const Q&) {
        std::cout << "C"; // Print C when object is copied
    }
    
    // Default constructor
    Q() {
        std::cout << "D"; // Print D when object is default constructed
    }
    
    // Method to increment v
    void change() {
        ++v; // Increment v
    }
    
    // Method to print v
    void func() {
        std::cout << v; // Print the value of v
    }
};

// Function that takes a std::function with Q parameter
void takeQfunc(std::function<void(Q)> qfunc) {
    Q q; // Create an object of type Q
    q.func(); // Print the initial value of v
    
    // Call the provided function with the created object
    qfunc(q); 
    
    q.func(); // Print the value of v again
}

int main() {
    // Call takeQfunc with a lambda function
    takeQfunc([](Q&& q) {
        q.change(); // Call the change method of the object
    });
    
    return 0;
}
