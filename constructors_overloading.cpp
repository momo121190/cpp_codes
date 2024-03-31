// This code defines a structure C with a default constructor, a copy constructor,
// and an assignment operator overload. When these constructors/operators are called,
// they print numbers to the console indicating their invocation.

#include <iostream>

struct C {
  // Default constructor
  C() { std::cout << "1"; }

  // Copy constructor
  C(const C& other) { std::cout << "2"; }

  // Assignment operator overload
  C& operator=(const C& other) { std::cout << "3"; return *this;}
};

int main() {
  // Creating object c1, invoking the default constructor, prints "1"
  C c1;

  // Creating object c2 and initializing it with c1, invoking the copy constructor, prints "2"
  C c2 = c1;

  // Creating object c3
  C c3;

  // Assigning c2 to c3, invoking the assignment operator overload, prints "3"
  c3 = c2;
}
