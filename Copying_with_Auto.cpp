// Summary: This code demonstrates the use of 'auto' with reference in C++.
// It declares a class A with a private member variable 'foo'. 
// The method getFoo() returns a reference to 'foo'. By explicitly specifying 
// 'auto&' instead of 'auto' when declaring the variable 'bar', 'bar' becomes 
// a reference to 'foo'. Thus, modifying 'bar' directly affects the original 'foo'.

#include <iostream>

class A {
  int foo = 0;

public:
  int& getFoo() { return foo; }
  void printFoo() { std::cout << foo; }
};

int main() {
  A a;

  auto& bar = a.getFoo(); // 'auto&' ensures 'bar' is a reference to 'foo'
  bar = bar + 3; // Modifying 'bar' affects the original 'foo'

  a.printFoo(); // Prints 3 since 'foo' has been modified
}
