#include "client.h"

const char *shm_name = "/hash_table_shm";
const int shm_SIZE = sizeof(Request);

void sendRequest(Request &request);
void printMenu();
int getKey();
int getValue();
void handleInsert(int key, int value);
void handleRead(int key);
void handleDelete(int key);

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
            std::cout << "************************* HashMap client stopped *************************\n";
            return 0;
        default:
            std::cout << "Invalid command. Please use one of the following: I, R, D, X\n";
            break;
        }
    }
}

void sendRequest(Request &request)
{
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        std::cerr << "Client: Failed to open shared memory\n";
        return;
    }

    Request *sharedRequest = static_cast<Request *>(mmap(0, shm_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (sharedRequest == MAP_FAILED)
    {
        std::cerr << "Client: mmap failed\n";
        munmap(sharedRequest, shm_SIZE);
        close(shm_fd);
        shm_unlink(shm_name);
        return;
    }
    *sharedRequest = request;

    std::cout << "Client: Sent request - Operation: " << static_cast<int>(request.operation)
              << ", Key: " << request.key << ", Value: " << request.value << std::endl;

    // Wait for the server to process the request
    while (!sharedRequest->processed)
    {
        usleep(100); // Sleep briefly to avoid busy-waiting
    }
    std::cout << "Client: Server processed request. Result = "
              << (sharedRequest->result ? "Success" : "Failure") << std::endl;

    munmap(sharedRequest, shm_SIZE);
    close(shm_fd);
    shm_unlink(shm_name);
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
        std::cout << "Invalid input. Please enter an integer for the key.\nKey: ";
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
        std::cout << "Invalid input. Please enter an integer for the value.\nValue: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return value;
}

void handleInsert(int key, int value)
{
    Request request{Operation::INSERT, key, value};
    sendRequest(request);

    std::cout << "Requested to insert (" << key << "," << value << ") into the hashmap\n";
    if (request.result)
    {
        std::cout << "Insert successful!\n";
    }
}

void handleRead(int key)
{
    Request request{Operation::READ, key, 0};
    sendRequest(request);

    std::cout << "Requested to read value for key: " << key << "\n";
    if (request.result)
    {
        std::cout << "Read value: " << request.value << "\n";
    }
    else
    {
        std::cout << "Key not found\n";
    }
}

void handleDelete(int key)
{
    Request request{Operation::DELETE, key, 0};
    sendRequest(request);

    std::cout << "Requested to delete entry for key: " << key << "\n";
    if (request.result)
    {
        std::cout << "Delete success!\n";
    }
    else
    {
        std::cout << "Key not found\n";
    }
}