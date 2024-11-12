#ifndef SHARED_H
#define SHARED_H

#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <limits>
#include <atomic>

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

struct SharedQueue
{
    static const int QUEUE_SIZE = 10;
    Request requests[QUEUE_SIZE];
    std::atomic<int> head;
    std::atomic<int> tail;

    SharedQueue() : head(0), tail(0) {}

    bool isFull() const
    {
        return (head + 1) % QUEUE_SIZE == tail;
    }

    bool isEmpty() const
    {
        return head == tail;
    }
};

const char *shm_name = "/shm_queue";
const int shm_SIZE = sizeof(SharedQueue);

#endif