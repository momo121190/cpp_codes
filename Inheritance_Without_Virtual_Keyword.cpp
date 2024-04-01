/* 
Summary:
The code demonstrates inheritance in C++ without using the 'virtual' keyword. 
The function 'f()' in class B does not override the function 'f()' in class A.
When an instance of B is passed to a function expecting an A reference, 
the function from the base class A is called, leading to static resolution.

Importance of 'virtual':
Without 'virtual':
- Function resolution is static, based on the type of the reference or pointer.
- Even if the object is of type B, the function 'f()' of the base class A is invoked.
- This can lead to unexpected results when dealing with polymorphic behavior.
*/

#include <iostream>

class A {
public:
  void f() { std::cout << "A"; } // Base class function
};

class B : public A {
public:
  void f() { std::cout << "B"; } // Derived class overrides function f()
};

void g(A &a) { a.f(); } // Function g() takes a reference to A

int main() {
  B b;
  g(b); // Calls g() with an instance of B
} 



