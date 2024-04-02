#include <iostream> // Include the iostream library for input/output operations

// Summary:
// This code explores the impact of access specifiers in C++ classes and their interaction with inheritance. 
// We discuss the difference between public and private access specifiers, where public members can be accessed 
// from anywhere in the program, while private members are restricted to member functions within the class. 
// We also examine inheritance, noting that when a derived class inherits publicly from a base class, the public 
// members of the base class become public members of the derived class, whereas with private inheritance, they 
// become private members. Additionally, we highlight the default access specifiers for inheritance, where classes 
// inherit privately by default, while structs inherit publicly.

// Explanation:
// The code defines two classes, C and D, inheriting from classes A and B. B contains a public member variable x. 
// However, C inherits publicly from B, while D inherits privately. Consequently, attempting to access x from 
// objects c and d leads to compilation errors, as x is private in both C and D. This example underscores the 
// importance of understanding access specifiers and inheritance in C++ to avoid unexpected behavior and compilation errors.

class A {}; // Define an empty class A

class B { // Define class B
public:
    int x = 0; // Declare and initialize a public member variable x with a default value of 0
};

class C : public A, B {}; // Define class C inheriting publicly from class A and class B

struct D : private A, B {}; // Define struct D inheriting privately from class A and class B

int main() // Main function
{
    C c; // Declare an object c of class C

    // Attempt to access member x of object c, which results in a compilation error since x is private in class C
    c.x = 3; 

    D d; // Declare an object d of struct D

    // Attempt to access member x of object d, which results in a compilation error since x is private in struct D
    d.x = 3;

    // Print the values of x for objects c and d (This line is not executed due to compilation errors)
    std::cout << c.x << d.x;
}
