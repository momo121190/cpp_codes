#include "server.hpp"
#include <iostream>

int main() {
    try {
        EpollServer server("9000");
        server.start();
        
        std::cout << "Server running. Press Enter to stop..." << std::endl;
        std::cin.get();
        
        server.stop();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
