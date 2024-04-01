#include <iostream>

// Summary: This code snippet demonstrates the modification of a reference member in a struct.
// The reference member i is initialized with x and then modified to hold the value of y.
struct C {
    int& i; // Define a reference member i in struct C
};

int main() {
    int x = 1; // Declare and initialize integer variable x to 1
    int y = 2; // Declare and initialize integer variable y to 2

    C c{x}; // Declare an object c of type C and initialize its reference member i with x
    
    // Assign the value of y to the reference member i, effectively modifying x indirectly
    c.i = y;

    // Print the values of x, y, and the reference member i
    std::cout << "Output: " << x << y << c.i; // Output: 222
}
