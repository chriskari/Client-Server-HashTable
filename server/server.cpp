#include "server.h"
#include "hash_table.h"

const char *shm_name = "/hash_table_shm";
const int shm_SIZE = sizeof(Request);

void processRequest(HashTable &hashTable, Request &request);
int getTableSize();

int main()
{
    int tableSize = getTableSize();
    HashTable hashTable(tableSize);

    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, shm_SIZE);

    // Map the shared memory object to the address space
    Request *request = static_cast<Request *>(mmap(0, shm_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (request == MAP_FAILED)
    {
        std::cerr << "Server: mmap failed\n";
        close(shm_fd);
        shm_unlink(shm_name);
        return 1;
    }

    std::cout << "Server: Shared memory created and initialized\n";

    while (true)
    {
        // Wait for a new request from the client
        if (!request->processed)
        {
            processRequest(hashTable, *request);
        }
        std::cerr << "Waiting for incoming requests...\n";
        usleep(3000000); // Wait for 3000ms before trying again
    }

    munmap(request, shm_SIZE);
    close(shm_fd);
    shm_unlink(shm_name);

    return 0;
}

void processRequest(HashTable &hashTable, Request &request)
{
    std::cout << "Server: Processing request:\n";
    std::cout << "Operation: " << static_cast<int>(request.operation) << "\n";
    std::cout << "Key: " << request.key << "\n";
    std::cout << "Value: " << request.value << "\n";

    switch (request.operation)
    {
    case Operation::INSERT:
        hashTable.insert(request.key, request.value);
        request.result = true;
        break;
    case Operation::READ:
        request.result = hashTable.read(request.key, request.value);
        break;
    case Operation::DELETE:
        request.result = hashTable.remove(request.key);
        break;
    default:
        request.result = false; // Unsupported operation
    }

    request.processed = true;
}

int getTableSize()
{
    int tableSize;
    std::cout << "Select a size for the hashmap: ";
    while (!(std::cin >> tableSize))
    {
        std::cout << "Invalid input. Please enter an integer for the value.\nValue: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return tableSize;
};
