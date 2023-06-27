CC = g++
CFLAGS = -Wall -std=c++11
LDFLAGS = -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lXinerama -lXcursor
EXECUTABLE = myapp

SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) glad.o
	$(CC) $(CFLAGS) $(OBJECTS) glad.o libglfw3.a -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

glad.o: glad.c 
	$(CC) $(CFLAGS) -c glad.c -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) glad.o

