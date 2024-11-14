#ifndef SHARED_H
#define SHARED_H

#include "Operation.h"
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <limits>
#include <atomic>
#include <mutex>

struct Request
{
    Operation operation;
    int key;
    int value;
    bool result;
    bool processed;
    std::mutex request_mutex;

    Request() : result(false), processed(false) {}
    Request(Operation op, int k, int v)
        : operation(op), key(k), value(v), result(false), processed(false) {}
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