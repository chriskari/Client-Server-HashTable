#include "hash_table.h"

HashTable::HashTable(int size) : size(size), table(size) {}

int HashTable::hashFunction(int key) const
{
    return key % size;
}

void HashTable::insert(int key, int value)
{
    int index = hashFunction(key);
    std::unique_lock lock(table[index].mutex);
    for (auto &item : table[index].items)
    {
        if (item.key == key)
        {
            item.value = value;
            return;
        }
    }
    table[index].items.push_back({key, value});
}

bool HashTable::read(int key, int &value) const
{
    int index = hashFunction(key);
    std::shared_lock lock(table[index].mutex);
    for (const auto &item : table[index].items)
    {
        if (item.key == key)
        {
            value = item.value;
            return true;
        }
    }
    return false;
}

bool HashTable::remove(int key)
{
    int index = hashFunction(key);
    std::unique_lock lock(table[index].mutex);
    for (auto it = table[index].items.begin(); it != table[index].items.end(); ++it)
    {
        if (it->key == key)
        {
            table[index].items.erase(it);
            return true;
        }
    }
    return false;
}
