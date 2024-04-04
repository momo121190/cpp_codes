// This C++ code demonstrates the usage of variadic templates to create a flexible class that can handle a variable number of arguments of different types.
// It defines a class template Foo with variadic template parameters.
// The class includes a constructor that prints the count of parameters passed and their values.
// Additionally, it contains private member functions for printing the parameters recursively.

// Summary: This code showcases the power of variadic templates in C++ for handling a flexible number of arguments of different types within a class template.

#include <iostream>

// Class Template Foo
template<typename... Args>
class Foo {
public:
    // Variadic Constructor
    Foo(Args&&... args) {
        std::cout << "Number of parameters: " << sizeof...(Args) << std::endl;
        printArgs(std::forward<Args>(args)...); // Calling printArgs function with variadic parameters
    }

private:
    // Function to print parameters
    template<typename T>
    void printArgs(T&& arg) {
        std::cout << arg << std::endl;
    }

    // Recursive function to print parameters
    template<typename T, typename... Rest>
    void printArgs(T&& arg, Rest&&... rest) {
        std::cout << arg << ", "; // Prints current parameter
        printArgs(std::forward<Rest>(rest)...); // Recursive call with remaining parameters
    }
};

int main() {
    // Instantiating Foo with different parameter types
    Foo<int, double, char> foo1(10, 3.14, 'a');
    Foo<std::string, int> foo2("Hello", 42);

    return 0;
}
