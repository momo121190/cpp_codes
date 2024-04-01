#include <iostream>

// Summary: This code snippet demonstrates the modification of a member variable in a struct.
// The member variable i is initialized with x and then modified to hold the value of y.
struct C {
    int i; // Define a member variable i in struct C
};

int main() {
    int x = 1; // Declare and initialize integer variable x to 1
    int y = 2; // Declare and initialize integer variable y to 2

    C c{x}; // Declare an object c of type C and initialize its member variable i with the value of x
    
    // Assign the value of y to the member variable i
    c.i = y;

    // Print the values of x, y, and the member variable i
    std::cout << "Output: " << x << y << c.i; // Output: 122
}
