#include <iostream>

// Template metaprogramming to compute GCD at compile-time
template <std::size_t A, std::size_t B>
struct gcd {
    static constexpr std::size_t value = gcd<B, A % B>::value;
};

// Base case: When B == 0, the GCD is A
template <std::size_t A>
struct gcd<A, 0> {
    static constexpr std::size_t value = A;
};

int main() {
    // Calculate GCD of 48 and 18 at compile-time
    std::cout << "GCD of 48 and 18: " << gcd<48, 18>::value << std::endl;  // Output: 6
    return 0;
}
