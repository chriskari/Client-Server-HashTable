#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <list>
#include <vector>
#include <shared_mutex>
#include <mutex>

struct HashItem
{
    int key;
    int value;
};

struct HashBucket
{
    std::list<HashItem> items;
    mutable std::shared_mutex mutex;
};

class HashTable
{
    int size;
    std::vector<HashBucket> table;

    int hashFunction(int key) const;

public:
    HashTable(int size);
    void insert(int key, int value);
    bool read(int key, int &value) const;
    bool remove(int key);
};

#endif