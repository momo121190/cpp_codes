// This program calculates the roots of a quadratic equation of the form ax^2 + bx + c = 0.
// It defines a function findRoots that takes three coefficients (a, b, c) as input and returns the roots of the equation as a pair.
// The program checks for the discriminant (b^2 - 4ac) to determine if real roots exist.
// If the discriminant is negative, indicating no real roots, it throws a domain_error exception.
// Otherwise, it calculates the roots using the quadratic formula (-b ± sqrt(b^2 - 4ac)) / (2a).
// The main function calls findRoots with coefficients 9, -10, 8, and prints the roots if they exist.
// If an exception is caught (indicating no real roots), it prints an error message.

#include <tuple>        // Inclusion of the <tuple> header for using tuples
#include <stdexcept>    // Inclusion of the <stdexcept> header for exceptions
#include <iostream>     // Inclusion of the <iostream> header for input/output
#include <cmath>        // Inclusion of the <cmath> header for math functions (e.g., sqrt)
#include <utility>      // Inclusion of the <utility> header for the pair structure (std::pair)
#include <string>       // Inclusion of the <string> header for string manipulation

// Function to find the roots of a quadratic equation
std::pair<double, double> findRoots(double a, double b, double c)
{   
    // Calculate the discriminant
    double discriminant = b * b - 4 * a * c;

    // Check if the discriminant is negative (no real roots)
    if (discriminant < 0) {     
        // Throw a domain_error exception
        throw std::domain_error("No real roots");    
    }
    
    // Calculate the square root of the discriminant
    double sqrt_discriminant = sqrt(discriminant);   
    
    // Calculate the roots using the quadratic formula
    double root1 = (-b + sqrt_discriminant) / (2 * a);
    double root2 = (-b - sqrt_discriminant) / (2 * a);
    
    // Return the roots as a pair
    return std::make_pair(root1, root2);
}

// Main function
#ifndef RunTests
int main()
{
    try {
        // Call the findRoots function with coefficients 9, -10, 8
        std::pair<double,double> roots = findRoots( 9, -10, 8);
        
        // Print the roots
        std::cout << "Roots: " << roots.first << ", " << roots.second << std::endl;
    } catch(const std::domain_error& e) {    // Catch the domain_error exception
        // Print an error message
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;   // Return 0 to indicate successful completion of the program
}
#endif
