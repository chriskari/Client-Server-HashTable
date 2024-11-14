#include "../common/shared.h"
#include <map>

void handleRequest(Request &request);
int getAndIncrementHead(SharedQueue *queue);
void printMenu();
int getKey();
int getValue();
void handleInsert();
void handleRead();
void handleDelete();

std::map<Operation, std::string> operation_map = {
    {Operation::INSERT, "INSERT"},
    {Operation::READ, "READ"},
    {Operation::DELETE, "DELETE"}};

int main()
{
    std::cout << "*********** HashTable client started ***********" << std::endl;

    printMenu();
    char userInput;
    while (true)
    {
        std::cin >> userInput;
        userInput = std::toupper(userInput);
        switch (userInput)
        {
        case 'I':
            handleInsert();
            break;
        case 'R':
            handleRead();
            break;
        case 'D':
            handleDelete();
            break;
        case 'X':
            std::cout << "*********** HashTable client stopped ***********" << std::endl;
            return 0;
        default:
            std::cout << "Invalid command. Please use one of the following: I, R, D, X" << std::endl;
            break;
        }
    }
}

void handleRequest(Request &request)
{
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        std::cerr << "Failed to open shared memory" << std::endl;
        return;
    }
    SharedQueue *queue = static_cast<SharedQueue *>(mmap(0, shm_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (queue == MAP_FAILED)
    {
        std::cerr << "mmap failed" << std::endl;
        close(shm_fd);
        return;
    }

    while (queue->isFull())
    {
        usleep(100);
    }

    int currentHead = getAndIncrementHead(queue);
    Request &sharedRequest = queue->requests[currentHead];
    sharedRequest.operation = request.operation;
    sharedRequest.key = request.key;
    sharedRequest.value = request.value;
    sharedRequest.processed = false;

    std::cout << "------------------------------------------------" << std::endl;
    if (request.operation == Operation::INSERT)
    {
        std::cout << "Sent request - " << operation_map[request.operation]
                  << " entry <" << request.key << "," << request.value << ">" << std::endl;
    }
    else
    {
        std::cout << "Sent request - " << operation_map[request.operation]
                  << " key <" << request.key << ">" << std::endl;
    }

    while (!sharedRequest.processed)
    {
        usleep(100);
    }
    request.result = sharedRequest.result;
    request.operation = sharedRequest.operation;
    request.key = sharedRequest.key;
    request.value = sharedRequest.value;
    request.processed = sharedRequest.processed;

    munmap(queue, shm_SIZE);
    close(shm_fd);
}

int getAndIncrementHead(SharedQueue *queue)
{
    int expected, newValue;
    do
    {
        expected = queue->head.load();
        newValue = (expected + 1) % SharedQueue::QUEUE_SIZE;
    } while (!queue->head.compare_exchange_weak(expected, newValue));
    return expected;
}

void printMenu()
{
    std::cout << "You can choose between the following actions:" << std::endl
              << "------------------------------------------------" << std::endl
              << "Use 'I' to insert an entry into the hash table" << std::endl
              << "Use 'R' to read an entry in the hash table" << std::endl
              << "Use 'D' to delete an entry from the hash table" << std::endl
              << "Use 'X' to exit the program" << std::endl
              << "------------------------------------------------" << std::endl;
}

int getKey()
{
    int key;
    std::cout << "Key: ";
    while (!(std::cin >> key))
    {
        std::cout << "Invalid input. Please enter an integer for the key." << std::endl
                  << "Key: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return key;
}

int getValue()
{
    int value;
    std::cout << "Value: ";
    while (!(std::cin >> value))
    {
        std::cout << "Invalid input. Please enter an integer for the value." << std::endl
                  << "Value: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return value;
}

void handleInsert()
{
    int key = getKey();
    int value = getValue();
    Request request{Operation::INSERT, key, value};
    handleRequest(request);

    if (request.result)
    {
        std::cout << "INSERT entry " << "<" << request.key << "," << request.value << "> successful!" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
}

void handleRead()
{
    int key = getKey();
    Request request{Operation::READ, key, 0};
    handleRequest(request);

    if (request.result)
    {
        std::cout << "READ successful! Value: <" << request.value << ">" << std::endl;
    }
    else
    {
        std::cout << "READ failed! Key <" << request.key << "> not found." << std::endl;
    }
    std::cout << "------------------------------------------------" << std::endl;
}

void handleDelete()
{
    int key = getKey();
    Request request{Operation::DELETE, key, 0};
    handleRequest(request);

    if (request.result)
    {
        std::cout << "DELETE key " << "<" << request.key << "> successful!" << std::endl;
    }
    else
    {
        std::cout << "DELETE failed! Key <" << request.key << "> not found." << std::endl;
    }
    std::cout << "------------------------------------------------" << std::endl;
}