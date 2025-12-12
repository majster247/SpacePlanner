# Nazwa programu
TARGET = perlin

# Kompilator
CXX = g++
CXXFLAGS=-std=c++17 -O2 -Wall -Wextra

# Pliki źródłowe
SRCS = perlin.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)

# Biblioteka nlohmann/json - jeśli jest w systemie lub w lokalnym include
INCLUDES = -I.

# Domyślny cel
all: $(TARGET)

# Kompilacja programu
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Kompilacja plików .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Sprzątanie
clean:
	rm -f $(OBJS) $(TARGET) *.ppm *.obj *.json

# Skrót do rebuild
rebuild: clean all

.PHONY: all clean rebuild
