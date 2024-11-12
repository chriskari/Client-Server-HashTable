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
        std::cerr << "Server: mmap failed" << std::endl;
        close(shm_fd);
        shm_unlink(shm_name);
        return 1;
    }

    std::cout << "Shared memory created and initialized" << std::endl;
    std::cout << "*********** HashTable server started ***********" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    while (true)
    {
        // Wait for a new request from the client
        if (!request->processed)
        {
            processRequest(hashTable, *request);
        }
        usleep(100);
    }

    munmap(request, shm_SIZE);
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
        std::cout << "Processed request: Insert <" << request.key << "," << request.value << "> - " << "Success!" << std::endl;
        break;
    case Operation::READ:
        request.result = hashTable.read(request.key, request.value);
        std::cout << "Processed request: Read <" << request.key << ",?> - " << (request.result ? "Success!" : "Failure!") << std::endl;
        break;
    case Operation::DELETE:
        request.result = hashTable.remove(request.key);
        std::cout << "Processed request: Delete <" << request.key << ",?> - " << (request.result ? "Success!" : "Failure!") << std::endl;
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
    while (!(std::cin >> tableSize))
    {
        std::cout << "Invalid input. Please enter an integer for the value." << std::endl
                  << "Value: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return tableSize;
};
