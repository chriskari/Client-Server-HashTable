#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <limits>

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
