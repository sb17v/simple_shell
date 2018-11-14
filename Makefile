ROOT_DIR= $(shell pwd)

SOURCE= src/*.cpp
MYLS_SOURCE = src/myls/*.cpp

HEADERS= src/*.h
MYLS_HEADERS= src/myls/*.h

OBJS= build/myshell.o build/utility.o build/commands.o
MYLS_OBJS= myls_build/myls.o

TARGET= bin/myshell
MYLS_TARGET= bin/myls

CXX= g++
CXXFLAGS= -Wall -ansi -pedantic

all: createdir $(TARGET) $(MYLS_TARGET)

createdir:
	@if ! [ -d "./bin" ]; then mkdir bin; fi
	@if ! [ -d "./build" ]; then mkdir build; fi
	@if ! [ -d "./myls_build" ]; then mkdir myls_build; fi

$(TARGET): $(OBJS) $(HEADERS) $(SOURCE)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(MYLS_TARGET): $(MYLS_OBJS) $(MYLS_HEADERS) $(MYLS_SOURCE)
	$(CXX) $(CXXFLAGS) $(MYLS_OBJS) -o $(MYLS_TARGET)

build/%.o: src/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

myls_build/%.o: src/myls/%.cpp $(MYLS_HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean: 
	rm -f $(TARGET) $(OBJS)
	rm -r $(MYLS_TARGET) $(MYLS_OBJS)
