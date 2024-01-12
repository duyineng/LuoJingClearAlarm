CC = g++

CFLAGS = -Iinc -std=c++11 -ljsoncpp

TARGET = clit

SRCDIR = src

OBJDIR = obj

SOURCES = $(wildcard $(SRCDIR)/*.cpp)

OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES)) $(OBJDIR)/main.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(CFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) 

$(OBJDIR)/main.o: main.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf $(OBJDIR)/* $(TARGET)

.PHONY:clean all
