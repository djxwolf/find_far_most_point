# Point Analyzer

High-performance 2D point isolation analysis — find the most isolated points using KD-Tree nearest neighbor search.

## Features

- **Fast**: O(n log n) using KD-Tree (nanoflann)
- **Multithreaded**: BS::thread_pool v5.1.0 for parallel KNN queries
- **Named points**: Pin/component names (e.g. U1.1, IC3.CLK, RST_12.DATA_BUS_7)
- **Auto-width report**: Box-drawing report dynamically adjusts to fit any name length
- **Scalable**: Handles millions of points efficiently

## Performance (Apple M-series, 8 cores)

| Points | Total Time | Query Phase |
|--------|-----------|-------------|
| 100K   | ~80ms     | ~5ms        |
| 1M     | ~750ms    | ~130ms      |
| 10M    | ~9s       | ~1.5s       |

*Total includes KD-Tree build. Query phase is parallelized across all cores.*

## Building

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Usage

### Generate random points with varied pin names
```bash
./bin/point_analyzer --count 1000000 --seed 42
```

### Read named pins from file
```bash
./bin/point_analyzer --input data/example_pins.txt
```

### Skip statistics (faster output)
```bash
./bin/point_analyzer --input data/example_pins.txt --no-stats
```

### All options
```
--count N         Generate N random points (default: 10000)
--seed N          Random seed (default: random)
--input FILE      Read points from file (name,x,y or x,y)
--topK N          Show top N isolated points (default: 10)
--no-stats        Skip statistics and histogram
--help            Show help message
```

## Input Format

Named (recommended): `name,x,y`
```
U1.1,0.0,0.0
U2.CLK,5.0,5.0
U3.RST,1.0,1.0
```

Legacy (unnamed): `x,y`
```
0.0,0.0
5.0,5.0
```

## API

```cpp
#include "point_analyzer.h"

// Named points
std::unordered_map<std::string, Point> pins = {
    {"U1.1", {0.0, 0.0}}, {"U2.A", {5.0, 5.0}}
};
PointAnalyzer analyzer(pins);
auto result = analyzer.analyze(10, true);

// result.mostIsolated.name  -> "U2.A"
// result.topK[0].name       -> "U2.A"
// result.topK[0].x, .y      -> coordinates
// result.topK[0].minDist    -> nearest-neighbor distance
```

## Sample Output

```
╔══════════════════════════════════════════════════════════════╗
║               POINT ANALYSIS REPORT                          ║
╠══════════════════════════════════════════════════════════════╣
║ MOST ISOLATED POINT                                          ║
╠══════════════════════════════════════════════════════════════╣
║ Name:                  J_160500.DATA_BUS_20                  ║
║ Coordinates:           (0.999351, 0.426181)                  ║
║ Min Neighbor Distance: 0.002162                              ║
╠══════════════════════════════════════════════════════════════╣
║ TOP 10 MOST ISOLATED POINTS                                  ║
╠══════════════════════════════════════════════════════════════╣
║  Rank  Name                  Coordinates      Dist    Pct    ║
║     1  J_160500.DATA_BUS_20  (0.9994, 0.4262) 0.0022 100.0% ║
║     2  J_510051.DATA_BUS_17  (0.4152, 0.5543) 0.0021  90.0% ║
║    ...                                                       ║
╚══════════════════════════════════════════════════════════════╝
```

## Project Structure

```
src/
  main.cpp              CLI entry point with argument parsing
  point_analyzer.h/cpp  Core analysis engine (KD-Tree + thread pool)
  report_generator.h/cpp Auto-width box-drawing report
include/
  nanoflann.hpp         KD-Tree library
  BS_thread_pool.hpp    Thread pool library
tests/
  test_point_analyzer.cpp  Google Test unit tests
data/
  example_pins.txt      Named pin example
  example_input.txt     Legacy x,y example
```

## Changelog

### v1.6
- Auto-adjust report width based on pin name length (no truncation)
- All columns align correctly regardless of name length

### v1.5
- Show coordinates in top-K table
- Generate varied-length realistic pin names for random mode

### v1.4
- Replace `std::thread` with BS::thread_pool v5.1.0

### v1.3
- Named points via `std::unordered_map<std::string, Point>`
- Auto-detect `name,x,y` vs legacy `x,y` file format

### v1.2
- `--no-stats` flag to skip statistics and histogram

### v1.1
- Merged traversal, deferred sqrt, multithreaded queries (~15x vs v1.0)

## License

MIT
