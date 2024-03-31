// Summary: This code demonstrates the use of 'auto' without reference in C++.
// It declares a class A with a private member variable 'foo'. 
// The method getFoo() returns a reference to 'foo'. However, when 'auto' is used 
// to declare a variable 'bar' and initialize it with the result of getFoo(), 
// 'bar' becomes a copy of 'foo' instead of a reference to 'foo'. Thus, 
// modifying 'bar' does not affect the original 'foo'.

#include <iostream>

class A {
  int foo = 0;

public:
  int& getFoo() { return foo; }
  void printFoo() { std::cout << foo; }
};

int main() {
  A a;

  auto bar = a.getFoo(); // 'auto' infers the type as 'int', not 'int&'
  bar = bar + 3; // Modifying 'bar' does not affect the original 'foo'

  a.printFoo(); // Prints 0 since 'foo' remains unchanged
}
