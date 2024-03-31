// Summary:
// This program demonstrates the usage of a template class List to create a list of elements.
// It overloads the << operator to print the elements of the list.
// The main function creates a List object with integer elements, adds elements to the list,
// and then prints the list using the overloaded << operator.

#include <iostream>
#include <vector>

using std::ostream;
using std::vector;
using std::cout;

// Definition of the template class List
template<class T>
class List {
private:
    std::vector<T> vec; // Vector to store elements of type T

public:
    // Method to add element to the list
    void push_back(T t) {
        vec.push_back(t);
    };

    // Friend function declaration to overload the << operator
    template<class U>
    friend ostream& operator<<(ostream& os, const List<U>& L);
};

// Overloading the << operator to print the List
template<class T>
ostream& operator<<(ostream& os, const List<T>& L) {
    for (T it : L.vec) { // Iterating through the vector elements
        os << it << " .. "; // Printing each element followed by " .. "
    }
    return os; // Returning the output stream
}

int main() {
    // Creating a List object with integer elements
    List<int> L;
    // Adding elements to the list
    L.push_back(1);
    L.push_back(2);
    L.push_back(3);
    // Printing the list using the overloaded << operator
    std::cout << L;

    return 0;
}
