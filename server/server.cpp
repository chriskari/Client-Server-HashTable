#include "../common/shared.h"
#include "hash_table.h"

void processRequest(HashTable &hashTable, Request &request);
int getTableSize();

int main()
{
    int tableSize = getTableSize();
    HashTable hashTable(tableSize);

    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, shm_SIZE);

    SharedQueue *queue = static_cast<SharedQueue *>(mmap(0, shm_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (queue == MAP_FAILED)
    {
        std::cerr << "mmap failed" << std::endl;
        close(shm_fd);
        shm_unlink(shm_name);
        return 1;
    }
    new (queue) SharedQueue();

    std::cout << "Shared memory created and initialized" << std::endl;
    std::cout << "*********** HashTable server started ***********" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    while (true)
    {
        if (!queue->isEmpty())
        {
            int currentTail = queue->tail.load();
            Request &request = queue->requests[currentTail];

            if (!request.processed)
            {
                processRequest(hashTable, request);
                request.processed = true;
                queue->tail.store((currentTail + 1) % SharedQueue::QUEUE_SIZE);
            }
        }
        usleep(100);
    }

    munmap(queue, shm_SIZE);
    close(shm_fd);
    shm_unlink(shm_name);

    return 0;
}

void processRequest(HashTable &hashTable, Request &request)
{
    switch (request.operation)
    {
    case Operation::INSERT:
        hashTable.insert(request.key, request.value);
        request.result = true;
        std::cout << "Processed request: Insert entry <" << request.key << "," << request.value << "> - " << "Success!" << std::endl;
        break;
    case Operation::READ:
        request.result = hashTable.read(request.key, request.value);
        std::cout << "Processed request: Read key <" << request.key << "> - " << (request.result ? "Success!" : "Failure!") << std::endl;
        break;
    case Operation::DELETE:
        request.result = hashTable.remove(request.key);
        std::cout << "Processed request: Delete key <" << request.key << "> - " << (request.result ? "Success!" : "Failure!") << std::endl;
        break;
    default:
        request.result = false; // Unsupported operation
    }

    request.processed = true;
}

int getTableSize()
{
    int tableSize;
    std::cout << "Select a size for the hash table: ";
    while (!(std::cin >> tableSize) || tableSize <= 0)
    {
        std::cout << "Invalid input. Please enter a positive integer for the table size." << std::endl
                  << "Value: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return tableSize;
};
