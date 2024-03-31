// Summary:
// This program demonstrates the usage of a template class Container with a private data member.
// It declares a friend function printData inside the Container class to access the private member,
// and then defines and implements the printData function outside the class.

#include <iostream>
using namespace std;

// Definition of the template class Container
template<typename T>
class Container {
private:
    T data; // Private data member of type T

public:
    // Constructor initializing the private data
    Container(T value) : data(value) {}

    // Declaration of a friend function
    template<typename U>
    friend void printData(const Container<U>& container);
};

// Definition and implementation of the friend function printData
template<typename T>
void printData(const Container<T>& container) {
    cout << "Data inside the container: " << container.data << endl;
}

int main() {
    // Creating a Container object with an integer
    Container<int> intContainer(42);

    // Calling the friend function to print the private data of the object
    printData(intContainer);

    return 0;
}
