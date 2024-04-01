#include <future>
#include <iostream>

int main()
{
    try {
        // Summary: This code demonstrates the basic usage of std::promise and std::future
        // It creates a std::promise, gets a std::future associated with it, sets a value on the promise,
        // and retrieves the value using the future.
        
        // Create a promise that will provide an integer value
        std::promise<int> p;
        
        // Get a future associated with the promise
        std::future<int> f1 = p.get_future();
      
        // Set the value of the promise to 1
        p.set_value(1);
        
        // Get the value from the future and print it
        std::cout << f1.get(); 
        
    } catch(const std::future_error& e)
    {  
        // If a future_error occurs during the process, catch it here and print its message
        std::cout << e.what()<<std::endl;
    }
}
