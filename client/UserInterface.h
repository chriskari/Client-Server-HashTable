#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "../common/Operation.h"
#include <iostream>
#include <limits>

namespace UserInterface
{
    void printMenu();
    std::string operationToString(Operation op);
    int getKey();
    int getValue();
}

#endif