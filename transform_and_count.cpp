#include <vector>
#include <functional>
#include <iostream>

// Function to transform and count elements greater than 10
int transform_and_count(const std::vector<int>& numbers, std::function<void(int&)> transform_func) {
    int count = 0;
    std::vector<int> transformed_numbers = numbers; // Create a copy to avoid modifying the original vector

    std::for_each(transformed_numbers.begin(), transformed_numbers.end(), [&](int& n) {
        transform_func(n); // Apply the transformation
        if (n > 10) {
            count++;
        }
    });

    return count;
}

int main() {
    // Define a transformation: add 2 to each element
    std::function<void(int&)> add_two = [](int& n) { n += 2; };

    std::vector<int> numbers{2, 3, 5, 8, 2, 1, 0, 9, 3, 8};

    // Call the function and print the result
    int result = transform_and_count(numbers, add_two);

    std::cout << "Count of elements greater than 10 after transformation: " << result << '\n';
    return 0;
}
