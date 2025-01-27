#include <iostream>
#include <fstream>

class Matrix {
private:
    int** data;
    const int rows = 3;
    const int cols = 3;

public:
    // Constructor to initialize the matrix with given values
    Matrix(int initData[3][3]) {
        data = new int*[rows];
        for (int i = 0; i < rows; ++i) {
            data[i] = new int[cols];
            for (int j = 0; j < cols; ++j) {
                data[i][j] = initData[i][j];
            }
        }
    }

    // Destructor to free dynamically allocated memory
    ~Matrix() {
        for (int i = 0; i < rows; ++i) {
            delete[] data[i];
        }
        delete[] data;
    }

    // Overloading the << operator to print the matrix
    friend std::ostream& operator<<(std::ostream& os, const Matrix& mat);
};

// Implementing the << operator overload
std::ostream& operator<<(std::ostream& os, const Matrix& mat) {
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            os << mat.data[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}

int main() {
    int data[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    // Create a Matrix object with the above data
    Matrix mat(data);

    // Print the matrix using the overloaded << operator
    std::cout << mat;

    return 0;
}
