#include "UserInterface.h"

namespace UserInterface
{
    void printMenu()
    {
        std::cout << "*********** HashTable client started ***********" << std::endl;
        std::cout << "You can choose between the following actions:" << std::endl
                  << "------------------------------------------------" << std::endl
                  << "Use 'I' to insert an entry into the hash table" << std::endl
                  << "Use 'R' to read an entry in the hash table" << std::endl
                  << "Use 'D' to delete an entry from the hash table" << std::endl
                  << "Use 'X' to exit the program" << std::endl
                  << "------------------------------------------------" << std::endl;
    }

    std::string operationToString(Operation op)
    {
        switch (op)
        {
        case Operation::INSERT:
            return "INSERT";
        case Operation::READ:
            return "READ";
        case Operation::DELETE:
            return "DELETE";
        }
        return "UNKNOWN";
    }

    int getKey()
    {
        int key;
        std::cout << "Key: ";
        while (!(std::cin >> key))
        {
            std::cout << "Invalid input. Please enter an integer for the key." << std::endl
                      << "Key: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        return key;
    }

    int getValue()
    {
        int value;
        std::cout << "Value: ";
        while (!(std::cin >> value))
        {
            std::cout << "Invalid input. Please enter an integer for the value." << std::endl
                      << "Value: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        return value;
    }
}
