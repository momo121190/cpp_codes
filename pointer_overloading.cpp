// Summary:
// This program demonstrates the behavior of function overloading when passing pointers to char.
// It defines two overloaded functions f, one taking an rvalue reference to char* and the other taking an lvalue reference to char*.
// Depending on whether the pointer is an rvalue or an lvalue, different overload of f will be selected.

#include <iostream>

// Function to print 1 if the parameter is an rvalue reference to char*
void f(char*&&) { std::cout << 1; }

// Function to print 2 if the parameter is an lvalue reference to char*
void f(char*&) { std::cout << 2; }

int main() {
    char c = 'a';
    
    // Passes the address of c, which is an rvalue, to function f
    // The overload f(char*&&) is selected, printing "1"
    f(&c);
    
    // Obtain the address of c, which is an lvalue
    char* ptr = &c;
    
    // Passes the pointer ptr, which is an lvalue, to function f
    // The overload f(char*&) is selected, printing "2"
    f(ptr);
    
    return 0;
}
