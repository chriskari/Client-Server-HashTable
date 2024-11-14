#include "../common/shared.h"
#include "hash_table.h"
#include <thread>
#include <vector>

const int MAX_THREADS = 4;
void processRequest(HashTable &hashTable, Request &request);
int getTableSize();

void serverThread(HashTable &hashTable, SharedQueue *queue)
{
    while (true)
    {
        if (!queue->isEmpty())
        {
            int currentTail = queue->tail.load();
            int currentHead = queue->head.load();

            // For e.g. 3 running threads the first 3 elements in the queue need to be checked for new requests at most
            const int MAX_ITERATIONS = MAX_THREADS;
            int iterations = 0;
            int index = currentTail;

            while (index != currentHead && iterations < MAX_ITERATIONS)
            {
                Request &request = queue->requests[index];

                if (!request.processed)
                {
                    // Try locking the request for processing
                    if (request.request_mutex.try_lock())
                    {
                        // Mutex successfully locked, process the request object
                        processRequest(hashTable, request);
                        request.request_mutex.unlock();

                        // Re-check the tail in case another thread updated it
                        int latestTail = queue->tail.load();
                        if (index == latestTail)
                        {
                            // Advance the tail to the next unprocessed request
                            int newTail = latestTail;
                            do
                            {
                                newTail = (newTail + 1) % SharedQueue::QUEUE_SIZE;
                            } while (newTail != currentHead && queue->requests[newTail].processed);

                            // Update the tail: if no unprocessed request was found, set tail to head
                            queue->tail.store(newTail == currentHead ? currentHead : newTail);
                        }
                        break;
                    }
                }

                index = (index + 1) % SharedQueue::QUEUE_SIZE;
                iterations++;
            }
        }
        usleep(100);
    }
}

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

    std::vector<std::thread> threads;
    for (int i = 0; i < MAX_THREADS; i++)
    {
        threads.emplace_back(serverThread, std::ref(hashTable), queue);
    }

    for (auto &thread : threads)
    {
        thread.join();
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
        request.result = false;
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
}