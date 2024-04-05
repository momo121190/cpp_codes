#include <iostream>
using namespace std;

// Template function adl
template<typename T>
void adl(T)
{
  cout << "T"; // Print "T" for any type T
}

// Structure S
struct S
{
};

// Function adl specialization for type S
void adl(S)
{
  cout << "S"; // Print "S" when called with an object of type S
}

// Function template call_adl
template<typename T>
void call_adl(T t)
{
  adl(S()); // First adl call: Non-dependent call, uses the generic adl(T) at compile time
  adl(t);   // Second adl call: Dependent call, resolves to specialized adl(S) at instantiation
}

// Main function
int main ()
{
  call_adl(S()); // Call the function call_adl with an object of type S
}

