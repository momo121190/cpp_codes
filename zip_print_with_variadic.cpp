#include <iostream>
#include <tuple>
#include <vector>
#include <list>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>
// Helper function: Print a tuple
template <typename Tuple, std::size_t... Indices>
void printTupleHelper(const Tuple& t, std::index_sequence<Indices...>) {
    ((std::cout << (Indices == 0 ? "" : ", ") << std::get<Indices>(t)), ...);
}

template <typename... Args>
void printTuple(const std::tuple<Args...>& t) {
    std::cout << "(";
    printTupleHelper(t, std::index_sequence_for<Args...>{});
    std::cout << ")";
}

// Variadic function: zipPrint
template <typename... Containers>
void zipPrint(const Containers&... containers) {
    // Ensure all containers have the same size
    const std::array<std::size_t, sizeof...(Containers)> sizes = {containers.size()...};
    if (!std::all_of(sizes.begin(), sizes.end(), [&](std::size_t s) { return s == sizes[0]; })) {
    std::cout<<"All containers must have the same size."<<std::endl ;
    }

    // Use iterators to traverse the containers
    auto begin_iters = std::make_tuple(containers.begin()...);
    auto end_iters = std::make_tuple(containers.end()...);

    // Traverse iterators simultaneously
    while (std::get<0>(begin_iters) != std::get<0>(end_iters)) {
        // Dereference iterators and create a tuple of current elements
        auto current = std::apply(
            [](auto&... iters) { return std::make_tuple((*iters)...); }, begin_iters);
        printTuple(current); // Print the tuple
        std::cout << std::endl;

        // Increment all iterators
        std::apply([](auto&... iters) { ((++iters), ...); }, begin_iters);
    }
}

int main() {
    // Example 1: Two containers
    std::vector<int> a{1, 2, 3};
    std::vector<double> b{ 5.5, 6.5,6};
    zipPrint(a, b);

    // Example 2: Three containers
    std::vector<int> c{1, 2};
    std::list<char> d{'a', 'b'};
    std::vector<bool> e{true, false};
    zipPrint(c, d, e);

    return 0;
}
