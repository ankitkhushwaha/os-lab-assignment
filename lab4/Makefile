CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS =

SOURCES = main.cpp PageTable.cpp FrameManager.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = vmm_simulator

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
