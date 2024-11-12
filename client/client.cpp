#include "client.h"

const char *shm_name = "/hash_table_shm";
const int shm_SIZE = sizeof(Request);

void handleRequest(Request &request);
void printMenu();
int getKey();
int getValue();
void handleInsert(int key, int value);
void handleRead(int key);
void handleDelete(int key);

std::map<Operation, std::string> operation_map = {
    {Operation::INSERT, "INSERT"},
    {Operation::READ, "READ"},
    {Operation::DELETE, "DELETE"}};

int main()
{
    std::cout << "************************* HashMap client started *************************\n";

    char userInput;
    while (true)
    {
        printMenu();
        std::cin >> userInput;
        userInput = std::toupper(userInput);

        int key, value;
        switch (userInput)
        {
        case 'I':
            key = getKey();
            value = getValue();
            handleInsert(key, value);
            break;
        case 'R':
            key = getKey();
            handleRead(key);
            break;
        case 'D':
            key = getKey();
            handleDelete(key);
            break;
        case 'X':
            shm_unlink(shm_name);
            std::cout << "************************* HashMap client stopped *************************\n";
            return 0;
        default:
            std::cout << "Invalid command. Please use one of the following: I, R, D, X\n";
            break;
        }
    }
}

void handleRequest(Request &request)
{
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        std::cerr << "Failed to open shared memory\n";
        return;
    }

    Request *sharedRequest = static_cast<Request *>(mmap(0, shm_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (sharedRequest == MAP_FAILED)
    {
        std::cerr << "mmap failed\n";
        munmap(sharedRequest, shm_SIZE);
        close(shm_fd);
        shm_unlink(shm_name);
        return;
    }

    *sharedRequest = request;
    std::cout << "Sent request - " << operation_map[request.operation]
              << " <" << request.key << ", " << request.value << ">" << std::endl;

    // Wait for the server to process the request
    while (!sharedRequest->processed)
    {
        usleep(100);
    }
    request = *sharedRequest;

    munmap(sharedRequest, shm_SIZE);
    close(shm_fd);
}

void printMenu()
{
    std::cout << "What action would you like to perform?\n"
              << "------------------------------------------------\n"
              << "Use 'I' to insert an entry into the HashMap\n"
              << "Use 'R' to read an entry in the HashMap\n"
              << "Use 'D' to delete an entry from the HashMap\n"
              << "Use 'X' to exit the program\n"
              << "------------------------------------------------\n";
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

void handleInsert(int key, int value)
{
    Request request{Operation::INSERT, key, value};
    handleRequest(request);

    if (request.result)
    {
        std::cout << "Insert " << " <" << request.key << ", " << request.value << "> successful!" << std::endl;
    }
}

void handleRead(int key)
{
    Request request{Operation::READ, key, 0};
    handleRequest(request);

    if (request.result)
    {
        std::cout << "Reading value successful: <" << request.key << ", " << request.value << ">" << std::endl;
    }
    else
    {
        std::cout << "Reading failed. Key: " << request.key << " not found." << std::endl;
    }
}

void handleDelete(int key)
{
    Request request{Operation::DELETE, key, 0};
    handleRequest(request);

    if (request.result)
    {
        std::cout << "Deletion of key: " << request.key << "> successful!" << std::endl;
    }
    else
    {
        std::cout << "Deletion failed. Key: " << request.key << " not found." << std::endl;
    }
}