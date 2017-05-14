CXX = g++
CFLAGS = -g -Wall
TARGET = shell
OBJECTS = main.o command.o pipe.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
		$(CXX) -o $@ $^
$(OBJECTS): %.o: %.cpp %.h
	$(CXX) -o $@ -c $< $(CFLAGS)

clean:
		rm -f *.o shell
