# SkipCache Simulation Suite

This project simulates various cache control and flush optimizations inspired by the paper "Skip It: Take Control of Your Cache!". It provides multiple simulation modes that demonstrate:
- **Benchmark Simulation**: A simple cache flush and persistent counter benchmark.
- **Multi-Core Simulation**: A simulation where each core has its own L1 cache and shares an L2 cache.
- **SkipCache Advanced Simulation**: Extended benchmarks that simulate redundant flush avoidance.
- **Unified CLI Simulation**: A single executable (`unified_sim`) that provides a command-line interface to select between different simulation modes.

## Features

- **Cache Simulation**: 
  - Implements a simple cache model with flush, clean, read, and eviction operations.
  - Tracks statistics such as flush count, clean operations, evictions, redundant flushes skipped, read hits, and read misses.
  - Supports configurable latencies for flush, clean, and read operations.

- **Persistent Data Structures**: 
  - Provides a simple persistent counter that writes through the cache, using flush and memory fence operations.

- **Multi-Level & Multi-Core Simulation**:
  - Models an L1 cache (per core) and a shared L2 cache with slower flush latency.
  - Demonstrates how cache evictions and inter-level data propagation work.

- **Unified Command-Line Interface**:
  - A single executable (`unified_sim`) that accepts command-line options (`benchmark`, `multi`, `skipcache`) to run different simulations.

- **Dynamic Build Targets**:
  - The Makefile builds separate executables for each simulation as well as the unified CLI.
  - A `run_all` target executes all simulations sequentially.

## Project Structure
```text
.
├── cache_simulator.cpp            # Implementation of the CacheSimulator class
├── cache_simulator.hpp            # CacheSimulator declaration and supporting types
├── persistent_data_structure.cpp  # Implementation of the PersistentCounter class
├── persistent_data_structure.hpp  # PersistentCounter declaration
├── multi_level_cache.cpp          # Implementation of the L2Cache class
├── multi_level_cache.hpp          # L2Cache declaration
├── benchmark.cpp                  # Benchmark simulation main (standalone)
├── multi_core_simulation.cpp      # Multi-core simulation main (standalone)
├── extended_benchmark.cpp         # SkipCache advanced simulation main (standalone)
├── unified_main.cpp               # Unified CLI main (select simulation mode via command-line)
├── config.hpp                     # Configuration loader using nlohmann/json (header-only)
├── logger.hpp                     # Simple logging facility (header-only)
├── json.hpp                       # Single-header JSON library (nlohmann/json)
└── Makefile                       # Build script for all targets and run_all
```

## Requirements

- A C++ compiler with C++14 support (e.g., `g++` or `clang++`).
- POSIX Threads (`-pthread`).
- [nlohmann/json](https://github.com/nlohmann/json) single-header version placed as `json.hpp` in the project directory.

## Building the Project

To build all simulation executables, run:

```bash
make
```

## Build Output

This will generate the following executables:

* `benchmark`
* `multicore_simulation` 
* `skipcache_advanced`
* `unified_sim`

## Running the Simulations

### Running Individually

You can run any simulation individually:

* Benchmark simulation:
```bash
./benchmark
```

* Multi-core simulation:
```bash
./multicore_simulation
```

* SkipCache advanced simulation:
```bash
./skipcache_advanced
```

* Unified CLI simulation:
```bash
./unified_sim benchmark
./unified_sim multi
./unified_sim skipcache
```

### Running All Simulations Sequentially

use the 'run_all' target:
```bash
make run_all
```

This target will build all executables and then run them sequentially, printing output for each simulation mode.

## Configuration
The simulation parameters (cache sizes, latencies, number of cores, etc.) can be configured via a JSON configuration file (e.g., config.json). See the sample below:

```
{
  "l1Size": 1024,
  "l2Size": 1024,
  "flushLatency": 100,
  "cleanLatency": 50,
  "readLatency": 10,
  "numThreads": 4,
  "numCores": 2,
  "simulationDuration": 200
}

```

The configuration is loaded at runtime (used in multi-core simulation, for example) via config.hpp.

## Acknowledgments
This project is inspired by research on cache control and persistent memory, including the techniques presented in "Skip It: Take Control of Your Cache!".
