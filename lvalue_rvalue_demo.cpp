// The purpose of this code is to demonstrate the usage of lvalue and rvalue references in C++.
// It shows how lvalue references and rvalue references work differently in modifying and moving values.

#include <iostream> // This header file is included to enable input and output operations using std::cout.

int main() {
    int x = 5; // x is an lvalue

    int& lvalueRef = x; // lvalue reference to an lvalue
    lvalueRef = 10;     // Modifying the value of x
    std::cout << x << std::endl; // Output the value of x (which is now 10)

    int&& rvalueRef = 7; // rvalue reference to an rvalue
    int y = std::move(rvalueRef); // Moving the value from the temporary object to y
    std::cout << y << std::endl; // Output the value of y (which is now 7)
}
