#include <iostream>    // For std::ostream and std::cout
#include <vector>      // For std::vector container
#include <list>        // For std::list container
#include <deque>       // For std::deque container
#include <type_traits> // For type-checking utilities (not used here but could be useful)
#include <array>       // For std::array container

int main() {
    // Define a lambda function to print the contents of a container
    // This lambda is generic and works with any container that supports range-based for loops.
    auto print_container = [](std::ostream& os, const auto& container) {
        // Iterate over each element in the container
        for (const auto& element : container) {
            os << element << " "; // Print the element followed by a space
        }
        os << "\n"; // Add a newline for better readability after printing all elements
    };

    // Create a std::vector of integers and initialize it with some values
    std::vector<int> vec = {1, 2, 3, 4, 5};

    // Create a std::list of strings and initialize it with some values
    std::list<std::string> lst = {"Hello", "world", "from", "C++"};

    // Create a std::deque of doubles and initialize it with some values
    std::deque<double> deq = {1.1, 2.2, 3.3, 4.4};

    // Create a std::array of integers with a fixed size and initialize it
    std::array<int, 6> a = {1, 2, 3, 4, 5, 6};

    // Use the lambda function to print the contents of each container
    // Print the contents of the vector
    print_container(std::cout, vec);

    // Print the contents of the list
    print_container(std::cout, lst);

    // Print the contents of the deque
    print_container(std::cout, deq);

    // Print the contents of the array
    print_container(std::cout, a);

    return 0; // Indicate successful program termination
}
