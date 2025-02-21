CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -pthread
SOURCES = cache_simulator.cpp persistent_data_structure.cpp multi_level_cache.cpp extended_benchmark.cpp  
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = skipcache_advanced 

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)
