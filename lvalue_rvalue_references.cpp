// This C++ code demonstrates the usage of lvalue and rvalue references, as well as the concept of move and copy constructors.
// It defines a simple class MyClass with constructors and destructor, along with functions that take lvalue and rvalue references as arguments.
// The main function illustrates the creation of MyClass objects and their utilization in various scenarios to understand the behavior of lvalue and rvalue references.
#include <iostream>

// Definition of a simple class
class MyClass {
public:
    // Default constructor
    MyClass() { std::cout << "Default constructor called" << std::endl; }

    // Copy constructor
    MyClass(const MyClass&) { std::cout << "Copy constructor called" << std::endl; }

    // Move constructor
    MyClass(MyClass&&) noexcept { std::cout << "Move constructor called" << std::endl; }

    // Destructor
    ~MyClass() { std::cout << "Destructor called" << std::endl; }

    // Declaration of the createObject function
    static MyClass createObject() {
        return MyClass();
    }
};

// Function taking an lvalue reference
void lvalueReferenceToMyClass(const MyClass& obj) {
    std::cout << "Function taking lvalue reference called" << std::endl;
}

// Function taking an rvalue reference
void rvalueReferenceToMyClass(MyClass&& obj) {
    std::cout << "Function taking rvalue reference called" << std::endl;
}

int main() {
    // Creating a MyClass object
    MyClass obj;

    // Using the copy constructor with an lvalue reference
    MyClass copyObj(obj);

    // Using the move constructor with an rvalue reference (temporary)
    MyClass moveObj(std::move(obj));

    // Using the lvalue reference with an existing lvalue object
    lvalueReferenceToMyClass(obj);

    // Using the lvalue reference with a temporary rvalue object
    lvalueReferenceToMyClass(std::move(moveObj));

    // Using the rvalue reference with a temporary rvalue object
    rvalueReferenceToMyClass(MyClass());

    // Using the temporary object returned by the function
    rvalueReferenceToMyClass(MyClass::createObject());

    return 0;
}
