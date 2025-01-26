#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>

std::mutex cout_mutex;

void synchronized_print(int id) {
    std::ostringstream buffer;
    buffer << "Thread " << id << " completed its task.\n";
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << buffer.str();
}

int main() {
    const int num_threads = 5;

    std::vector<std::jthread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(synchronized_print, i);
    }

    return 0; // Threads automatically joined
}
