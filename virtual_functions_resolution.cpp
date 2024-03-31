// Summary: Illustrating default arguments in virtual functions and their resolution
//          based on static type of pointer/reference in C++.

#include <iostream>

// Definition of base class A
struct A {
    // Virtual function foo with a default argument of 1
    virtual void foo(int a = 1) {
        std::cout << "A" << a;
    }
};

// Definition of derived class B, inheriting from A
struct B : A {
    // Redefinition of virtual function foo with a default argument of 2
    virtual void foo(int a = 2) {
        std::cout << "B" << a;
    }
};

// Main function
int main() {
    // Creating an object of type B and assigning its address to a pointer of type A
    A* b = new B;

    // Calling the virtual function foo through the pointer
    // Expected output: "B1"
    b->foo();
}
