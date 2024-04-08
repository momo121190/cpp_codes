#include <iostream>
#include <openssl/rand.h>
#include <iomanip>

int main() {
    // Initialize the OpenSSL PRNG
    RAND_poll();

    // Generate a random number
    unsigned char rand_num[16];
    if (RAND_bytes(rand_num, sizeof(rand_num)) != 1) {
        std::cerr << "Error generating random number" << std::endl;
        return 1;
    }

    // Print the random number
    std::cout << "Random number generated:" << std::endl;
    std::cout << std::hex << std::setfill('0');
    for (int i = 0; i < sizeof(rand_num); i++) {
        std::cout << std::setw(2) << static_cast<unsigned int>(rand_num[i]);
    }
    std::cout << std::dec << std::endl;

    return 0;
}

