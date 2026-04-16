# Point Analyzer

High-performance 2D point set analysis using KD-Tree for O(n log n) nearest neighbor search.

## Features

- **Fast**: O(n log n) algorithm using KD-Tree (nanoflann)
- **Multithreaded**: BS::thread_pool v5.1.0 for parallel KNN queries
- **Named points**: Support for pin/component names (e.g. U1.1, U2.CLK)
- **Scalable**: Handles millions of points efficiently

## Performance (Apple M-series, 8 cores)

| Points | Query Time | Speedup vs v1.0 |
|--------|-----------|-----------------|
| 100K   | ~5ms      | 15x             |
| 500K   | ~39ms     | 14x             |
| 1M     | ~131ms    | 15x             |
| 5M     | ~883ms    | 16x             |

*Query phase only (excludes KD-Tree build). Full run includes tree construction overhead.*

## Changelog

### v1.4
- Replace `std::thread` with BS::thread_pool v5.1.0 (thread reuse, cleaner API)

### v1.3
- Support named points via `std::unordered_map<std::string, Point>` input
- File format: `name,x,y` (auto-detects legacy `x,y`)

### v1.2
- Add `--no-stats` flag to skip statistics and histogram

### v1.1
- Merged traversal, deferred sqrt, multithreaded queries (~15x vs v1.0)

## Building

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Usage

### Generate random points
```bash
./bin/point_analyzer --count 1000000 --seed 42
```

### Read named pins from file
```bash
./bin/point_analyzer --input data/pins.txt
```

### Skip statistics (faster output)
```bash
./bin/point_analyzer --input data/pins.txt --no-stats
```

### Options
- `--count N`: Generate N random points (default: 10000)
- `--seed N`: Random seed (default: random)
- `--input FILE`: Read points from file
- `--topK N`: Show top N isolated points (default: 10)
- `--no-stats`: Skip statistics and histogram
- `--help`: Show help message

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
```

## License

MIT
