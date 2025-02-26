CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -pthread -I.

# 1) benchmark executable
BENCH_SOURCES = benchmark.cpp cache_simulator.cpp persistent_data_structure.cpp
BENCH_OBJS = $(BENCH_SOURCES:.cpp=.o)

# 2) multicore_simulation executable
MULTI_SOURCES = multi_core_simulation.cpp cache_simulator.cpp multi_level_cache.cpp persistent_data_structure.cpp
MULTI_OBJS = $(MULTI_SOURCES:.cpp=.o)

# 3) skipcache_advanced (extended_benchmark)
SKIP_SOURCES = extended_benchmark.cpp cache_simulator.cpp multi_level_cache.cpp persistent_data_structure.cpp
SKIP_OBJS = $(SKIP_SOURCES:.cpp=.o)

# 4) unified_sim executable (new unified CLI with extra "vectorized" mode)
UNIFIED_SOURCES = unified_main.cpp cache_simulator.cpp multi_level_cache.cpp persistent_data_structure.cpp vectorized_hash_table.cpp
UNIFIED_OBJS = $(UNIFIED_SOURCES:.cpp=.o)

all: benchmark multicore_simulation skipcache_advanced unified_sim

benchmark: $(BENCH_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(BENCH_OBJS)

multicore_simulation: $(MULTI_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(MULTI_OBJS)

skipcache_advanced: $(SKIP_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(SKIP_OBJS)

unified_sim: $(UNIFIED_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(UNIFIED_OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run_all: all
	@echo "\n=== Running benchmark ==="
	./benchmark
	@echo "\n=== Running multicore_simulation ==="
	./multicore_simulation
	@echo "\n=== Running skipcache_advanced ==="
	./skipcache_advanced
	@echo "\n=== Running unified_sim in all modes ==="
	@echo " - benchmark mode"
	./unified_sim benchmark
	@echo "\n - multi mode"
	./unified_sim multi
	@echo "\n - skipcache mode"
	./unified_sim skipcache
	@echo "\n - vectorized mode"
	./unified_sim vectorized

clean:
	rm -f benchmark multicore_simulation skipcache_advanced unified_sim \
	      $(BENCH_OBJS) $(MULTI_OBJS) $(SKIP_OBJS) $(UNIFIED_OBJS)
