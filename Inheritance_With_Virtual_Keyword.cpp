/* 
Summary:
The code demonstrates inheritance in C++ using the 'virtual' keyword for function f() in class A.
With 'virtual', function resolution occurs at runtime based on the actual type of the object, enabling polymorphic behavior.

Importance of 'virtual':
With 'virtual':
- Enables dynamic dispatch or runtime polymorphism.
- The correct overridden function is called based on the actual type of the object at runtime.
- Ensures the expected behavior when dealing with polymorphic objects.
- Facilitates proper use of inheritance in designing object-oriented systems.
*/
#include <iostream>

class A {
public:
  virtual void f() { std::cout << "A"; } // Base class function declared as virtual
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



