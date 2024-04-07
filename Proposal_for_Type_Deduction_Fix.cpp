include <iostream>

template<typename T>
T sum(T arg) {
    return arg;   
}

template<typename T, typename ...Args>
T sum(T arg, Args... args) {    
    return arg + sum<T>(args...);
}

int main() {
    auto n1 = sum<double>(0.5, 1, 0.5, 1); // Spécifier explicitement le type double
    auto n2 = sum<double>(1, 0.5, 1, 0.5);    // Spécifier explicitement le type int
    std::cout << n1 << " " << n2;
}


