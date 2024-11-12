#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <limits>
#include <map>

enum class Operation
{
    INSERT,
    READ,
    DELETE
};

struct Request
{
    Operation operation;
    int key;
    int value;
    bool result;
    bool processed;
};

#endif
