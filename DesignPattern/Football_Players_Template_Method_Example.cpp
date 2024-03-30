#include <iostream>
#include <string>
using namespace std;

// Abstract class Player
class Player {
public:
    // Generic method display()
    void display() {
        // Call to the pure virtual method displayRole() to display the player's role
        displayRole();
        // Displaying player-specific information
        cout << "-- Name: " << name << endl;
        cout << "-- Jersey Number: " << jerseyNumber << endl;
        // Call to the protected virtual method displayOtherInfo() to display additional information
        displayOtherInfo();
    }
    // Pure virtual method to display the player's role
    virtual void displayRole() = 0;

protected:
    // Protected virtual method to display additional information (default implementation)
    virtual void displayOtherInfo() {}

    // Player's name
    string name;
    // Player's jersey number
    int jerseyNumber;
};

// Defender class, derived from Player
class Defender : public Player {
public:
    // Constructor initializing the name, jersey number, and number of interceptions
    Defender(const string& name, int jerseyNumber, int interceptions) {
        this->name = name;
        this->jerseyNumber = jerseyNumber;
        this->interceptions = interceptions;
    }
    // Implementation of the virtual method displayRole() to display the defender's role
    virtual void displayRole() override { cout << "I am a defender" << endl; }
    // Redefinition of the virtual method displayOtherInfo() to display the number of interceptions
    virtual void displayOtherInfo() override {
        cout << "-- Interceptions: " << interceptions << endl;
    }

private:
    // Number of interceptions by the defender
    int interceptions;
};

// Forward class, derived from Player
class Forward : public Player {
public:
    // Constructor initializing the name, jersey number, and number of goals scored
    Forward(const string& name, int jerseyNumber, int goalsScored) {
        this->name = name;
        this->jerseyNumber = jerseyNumber;
        this->goalsScored = goalsScored;
    }
    // Implementation of the virtual method displayRole() to display the forward's role
    virtual void displayRole() override { cout << "I am a forward" << endl; }
    // Redefinition of the virtual method displayOtherInfo() to display the number of goals scored
    virtual void displayOtherInfo() override {
        cout << "-- Goals Scored: " << goalsScored << endl;
    }

private:
    // Number of goals scored by the forward
    int goalsScored;
};

// Main function
int main() {
    // Creating a defender player with the name "Ramos", jersey number 4, and 20 interceptions
    Defender *ramos = new Defender("Ramos", 4, 20);
    // Calling the display() method to display the defender player's information
    ramos->display();

    // Creating a forward player with the name "Ronaldo", jersey number 7, and 30 goals scored
    Forward *ronaldo = new Forward("Ronaldo", 7, 30);
    // Calling the display() method to display the forward player's information
    ronaldo->display();

    return 0;
}
