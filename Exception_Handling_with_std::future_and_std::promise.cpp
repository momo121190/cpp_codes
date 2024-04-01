#include <future>
#include <iostream>

int main()
{
    try {
        // Summary: This code demonstrates a scenario where an exception is thrown due to misuse of std::promise and std::future
        // It attempts to create two futures from the same promise, which is not allowed. 
        
        // Create a promise that will provide an integer value
        std::promise<int> p;
        
        // Get the first future associated with the promise
        std::future<int> f1 = p.get_future();
        
        // Attempt to get a second future associated with the same promise
        std::future<int> f2 = p.get_future();
        
        // This line would throw a future_error since attempting to get a future from the same promise twice is invalid
        // But since it's caught, the program continues to run
        
        // Set the value of the promise to 1
        p.set_value(1);
        
        // Get the value from the first future and print it
        std::cout << f1.get(); 
        
    } catch(const std::future_error& e)
    {  
        // If a future_error occurs during the process, catch it here and print its message
        std::cout << e.what()<<std::endl;
    }
}
