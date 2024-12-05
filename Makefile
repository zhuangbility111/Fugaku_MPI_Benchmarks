# Compiler and flags
CXX = mpiFCC
CXXFLAGS = -Nclang -O0 -Wall -std=c++11

# Target executable
TARGET = main

# Source files
SRCS = benchmark.cpp topology.cpp main.cpp

# Object files (derived from source files)
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Rule to build the target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to build object files
%.o: %.cpp benchmark.h topology.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean

