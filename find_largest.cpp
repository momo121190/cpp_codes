#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>

template <typename Container>
auto find_largest(const Container& container) {
    if (container.empty()) {
        return typename Container::value_type{};  // Return a default value if empty
    }
    return *std::max_element(container.begin(), container.end());
}

template <typename First, typename... Rest>
auto find_largest(const First& first, const Rest&... rest) {
    auto largest_in_first = find_largest(first);
    
    // To avoid issues comparing different types, we use a common type (e.g., double)
    using CommonType = std::common_type_t<decltype(largest_in_first), decltype(find_largest(rest...))>;
    
    // Convert the largest elements to the common type
    auto largest_in_first_converted = static_cast<CommonType>(largest_in_first);
    
    if constexpr (sizeof...(rest) == 0) {
        return largest_in_first_converted;  // Base case: no more containers to check
    } else {
        auto largest_in_rest = find_largest(rest...);
        auto largest_in_rest_converted = static_cast<CommonType>(largest_in_rest);
        return std::max(largest_in_first_converted, largest_in_rest_converted);  // Compare after conversion
    }
}

int main() {
    std::vector<int> vec = {1, 3, 5, 2};
    std::list<float> lst = {2.5f, 3.1f, 1.8f};
    std::vector<int> vec2 = {8, 10, 4};

    // Test the function with multiple containers
    auto largest = find_largest(vec, lst, vec2);
    std::cout << "Largest element: " << largest << std::endl;

    return 0;
}
