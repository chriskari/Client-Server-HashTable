# Compiler and flags
CC = g++
CFLAGS = -Wall -g -std=c++17 -Icommon

# Target executable name
TARGET = client_program

# Source and object files
SRC = client.cpp
OBJ = $(SRC:.cpp=.o)

# Default target to build the executable
all: $(TARGET)

# Rule to link object files into the final executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Rule to compile .cpp files into .o files
%.o: %.cpp common/shared.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build files
clean:
	rm -f $(OBJ) $(TARGET)
