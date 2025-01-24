#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

// Function to convert a comma-separated string into a vector of integers
std::vector<int> parseCoefficients(const std::string& str) {
    std::stringstream ss(str); 
    std::vector<int> coefficients;
    std::string token;

    while (getline(ss, token, ',')) {
        coefficients.push_back(std::stoi(token)); // Convert and add to vector
    }
    return coefficients;
}

// Function to compute quadratic results from a vector of coefficient strings
std::vector<int> computeQuadraticResults(const std::vector<std::string>& expressions, int x) {
    std::vector<int> results(expressions.size()); // Initialize results vector

    // Transform each string into a quadratic result
    std::transform(expressions.begin(), expressions.end(), results.begin(), [x](const std::string& expression) {
        auto coefficients = parseCoefficients(expression); // Parse coefficients
        int a = coefficients[0];
        int b = coefficients[1];
        int c = coefficients[2];
        return a * x * x + b * x + c; // Compute quadratic value
    });

    return results;
}

int main() {
    std::vector<std::string> coefficientExpressions = {"1,2,3", "4,2,3", "2,6,4"};
    int x = 2;

    auto results = computeQuadraticResults(coefficientExpressions, x);

    // Print the results
    for (const auto& result : results) {
        std::cout << result << " ";
    }

    return 0;
}
