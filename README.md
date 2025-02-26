# SkipCache Simulation Suite

This project simulates various cache control and flush optimizations inspired by the paper "Skip It: Take Control of Your Cache!" and further enhanced with insights from persistent memory and vectorized data structures research from "NVM: Is it Not Very Meaningful for Databases?" and "Analyzing Vectorized Hash Tables Across CPU Architectures". In addition to simulating cache behavior, the suite now includes a vectorized hash table module inspired by the Bucket-Based Comparison (BBC) design. The project provides multiple simulation modes that demonstrate:
- **Benchmark Simulation**: A simple cache flush and persistent counter benchmark.
- **Multi-Core Simulation**: A simulation where each core has its own L1 cache and shares an L2 cache.
- **SkipCache Advanced Simulation**: Extended benchmarks that simulate redundant flush avoidance and multi-level cache operations.
- **Unified CLI Simulation**: A single executable (`unified_sim`) that provides a command-line interface to select between different simulation modes, including a new vectorized mode for demonstrating batched, vectorized hash table operations.

## Features

- **Cache Simulation**: 
  - Implements a simple cache model with flush, clean, read, and eviction operations.
  - Tracks statistics such as flush count, clean operations, evictions, redundant flushes skipped, read hits, and read misses.
  - Supports configurable latencies for flush, clean, and read operations to mimic different hardware behaviors, including persistent memory modes.

- **Persistent Data Structures**: 
  - Provides a persistent counter that uses flush and memory fence operations to simulate persistence through the cache hierarchy.

- **Multi-Level & Multi-Core Simulation**:
  - Models an L1 cache (per core) and a shared L2 cache with slower flush latency.
  - Demonstrates cache evictions, data propagation between levels, and the impact of flush optimizations.

- **Vectorized Hash Table Module**:
  - Implements a vectorized hash table inspired by the BBC design.
  - Uses a batched (simulated vectorized) probing approach with fingerprints to accelerate key insertion and lookup.
  - Provides an optional vectorized mode in the unified CLI to showcase how hardware-aware data structures can boost performance.

- **Unified Command-Line Interface**:
  - A single executable (`unified_sim`) that accepts command-line options (`benchmark`, `multi`, `skipcache`, `vectorized`) to run different simulations.
  - The new "vectorized" mode demonstrates the functionality of the vectorized hash table.

- **Dynamic Build Targets**:
  - The Makefile builds separate executables for each simulation as well as the unified CLI.
  - A `run_all` target executes all simulations sequentially.
 
- **Enhanced Human-Readable Persistent Logging**:
  - Implements a new logging mechanism (ReadableFlexibleLogger) inspired by advanced persistent logging techniques (CSO-FVB).
  - Each log entry is written as a single, human-readable text line that includes:
    - A timestamp and event message.
    - Computed metadata detailing the "last difference" (formatted as offset=X, bit=Y) between a preinitialized pattern and the new entry.
    - A validity field that is initially set to "INVALID" and then updated in place to "0xBEEF" once the entry is persisted.
  - This feature not only provides persistence guarantees with a single flush round-trip but also makes it easy to review and debug logs directly from the text file.

## Project Structure
```text
.
├── cache_simulator.cpp            # Implementation of the CacheSimulator class
├── cache_simulator.hpp            # CacheSimulator declaration and supporting types
├── persistent_data_structure.cpp  # Implementation of the PersistentCounter class
├── persistent_data_structure.hpp  # PersistentCounter declaration
├── multi_level_cache.cpp          # Implementation of the L2Cache class
├── multi_level_cache.hpp          # L2Cache declaration
├── vectorized_hash_table.cpp      # Implementation of the vectorized hash table (BBC-inspired)
├── vectorized_hash_table.hpp      # Declaration of the vectorized hash table module
├── benchmark.cpp                  # Benchmark simulation main (standalone)
├── multi_core_simulation.cpp      # Multi-core simulation main (standalone)
├── extended_benchmark.cpp         # SkipCache advanced simulation main (standalone)
├── unified_main.cpp               # Unified CLI main (select simulation mode via command-line)
├── config.hpp                     # Configuration loader using nlohmann/json (header-only)
├── config.json                    # Sample configuration file for simulation parameters
├── logger.hpp                     # Wrapper for the enhanced human-readable logger
├── ReadableFlexibleLogger.hpp     # Enhanced human-readable persistent logger implementation
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
./unified_sim vectorized
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
This project is inspired by research on cache control and persistent memory, including the techniques presented in "Skip It: Take Control of Your Cache!", "Efficient Logging in Non-Volatile Memory by Exploiting Coherency Protocols", "NVM: Is it Not Very Meaningful for Databases?" and "Analyzing Vectorized Hash Tables Across CPU Architectures".
