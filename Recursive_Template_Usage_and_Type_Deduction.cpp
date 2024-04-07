// Summary: This code demonstrates the use of template recursion and automatic type deduction in C++ to calculate the sum of a variadic number of arguments of different types.
#include <iostream>

// Template function sum: Base case
template<typename T>
T sum(T arg) {
    return arg;   
}

// Template function sum: Recursive case
template<typename T, typename ...Args>
T sum(T arg, Args... args) {    
    return arg + sum<T>(args...);
}

int main() {
    auto n1 = sum(0.5, 1, 0.5, 1);  // Sum of floating-point numbers
    auto n2 = sum(1, 0.5, 1, 0.5);   // Sum of integers
    std::cout << "Sum of floating-point numbers: " << n1 << std::endl;
    std::cout << "Sum of integers: " << n2 << std::endl;
}
