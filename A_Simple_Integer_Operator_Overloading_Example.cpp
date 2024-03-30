// This program demonstrates operator overloading in C++. 
// It defines a class called Integer that represents integer values. 
// The class overloads the + and += operators to perform addition operations.
// The main function demonstrates the usage of these operators.

#include <iostream>
using namespace std;

class Integer {
  int i;
public:
  Integer(int ii) : i(ii) {}  // Constructor to initialize the Integer object with a value
  
  // Overloading + operator
  Integer operator+(const Integer& rv) const {
    cout << "operator+" << endl;  // Output message indicating the + operator is being used
    int sum = i + rv.i;  // Calculating the sum of the current object's value and the value of the passed object
    return Integer(sum);  // Returning a new Integer object with the sum
  }
  
  // Overloading += operator
  Integer& operator+=(const Integer& rv) {
    cout << "operator+=" << endl;  // Output message indicating the += operator is being used
    i += rv.i;  // Adding the value of the passed object to the current object's value
    return *this;  // Returning a reference to the current object
  }
  
  // Getter function to access the value of i
  int getValue() const {
    return i;  // Returning the value of i
  }
};

int main() {
  Integer ii(15), jj(2), kk(3);  // Creating Integer objects ii, jj, and kk with initial values
  
  // Using the += operator to add ii to kk
  kk += ii;
  cout << "kk after kk += ii: " << kk.getValue() << endl;  // Outputting the value of kk after addition
  
  // Using the + operator to add ii and kk and storing the result in result
  Integer result = ii + kk;
  cout << "Result of ii + kk: " << result.getValue() << endl;  // Outputting the result of addition
  
  return 0;  // Indicating successful termination of the program
}
