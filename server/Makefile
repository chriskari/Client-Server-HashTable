# Compiler and flags
CC = g++
CFLAGS = -Wall -g -std=c++17 -pthread

# Target executable name
TARGET = server_program

# Source and object files
SRC = server.cpp hash_table.cpp
OBJ = $(SRC:.cpp=.o)

# Default target to build the executable
all: $(TARGET)

# Rule to link object files into the final executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(CFLAGS)

# Rule to compile .cpp files into .o files
%.o: %.cpp server.h hash_table.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build files
clean:
	rm -f $(OBJ) $(TARGET)
