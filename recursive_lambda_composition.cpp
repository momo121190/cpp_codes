#include <iostream>
#include <utility>

// Base case: If there are no more functions left, return the value
template <typename T>
T apply_all(T value) {
    return value;
}

// Recursive case: Apply the first function and pass the result to the rest
template <typename T, typename F, typename... Fs>
auto apply_all(T value, F func, Fs... funcs) {
    return apply_all(func(value), funcs...);
}

int main() {
    auto add = [](int x) { return x + 4; };
    auto square = [](int x) { return x * x; };
    auto half = [](int x) { return x / 2; };

    int result = apply_all(4, add, square, half);
    std::cout << result << std::endl;  // Expected output: 20

    return 0;
}
