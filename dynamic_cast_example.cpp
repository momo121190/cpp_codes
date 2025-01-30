#include <iostream>

class Animal {
public:
    virtual void speak() { std::cout << "Some generic animal sound\n"; }
    virtual ~Animal() {}  // Virtual destructor for proper cleanup
};

class Dog : public Animal {
public:
    void speak() override { std::cout << "Woof! Woof!\n"; }
    void fetch() { std::cout << "Dog is fetching the ball!\n"; }
};

class Cat : public Animal {
public:
    void speak() override { std::cout << "Meow! Meow!\n"; }
    void scratch() { std::cout << "Cat is scratching!\n"; }
};

// Function to identify the type of Animal using dynamic_cast
void identifyAnimal(Animal* animal) {
    if (Dog* dog = dynamic_cast<Dog*>(animal)) {
        std::cout << "This is a Dog.\n";
        dog->fetch();
    } 
    else if (Cat* cat = dynamic_cast<Cat*>(animal)) {
        std::cout << "This is a Cat.\n";
        cat->scratch();
    } 
    else {
        std::cout << "Unknown animal.\n";
    }
}

int main() {
    Dog myDog;
    Cat myCat;
    Animal* unknown = new Animal(); // Not an actual dog or cat

    identifyAnimal(&myDog);  // Should recognize as Dog
    identifyAnimal(&myCat);  // Should recognize as Cat
    identifyAnimal(unknown); // Should print "Unknown animal."

    delete unknown;  // Clean up
    return 0;
}
