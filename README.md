# Point Analyzer

High-performance 2D point set analysis using KD-Tree for O(n log n) nearest neighbor search.

## Features

- **Fast**: O(n log n) algorithm using KD-Tree (nanoflann)
- **Multithreaded**: Parallel KNN queries across all CPU cores
- **Scalable**: Handles millions of points efficiently
- **Comprehensive**: Full analysis report with statistics and visualization

## Performance (Apple M-series, 8 cores)

| Points | Query Time | Speedup vs v1.0 |
|--------|-----------|-----------------|
| 100K   | ~5ms      | 15x             |
| 500K   | ~39ms     | 14x             |
| 1M     | ~131ms    | 15x             |
| 5M     | ~883ms    | 16x             |

*Query phase only (excludes KD-Tree build). Full run includes tree construction overhead.*

### What changed in v1.1

- **Merged traversal**: Three separate KNN passes combined into one (~3x)
- **Deferred sqrt**: Squared-distance comparisons, sqrt only at final output
- **Multithreaded queries**: `std::thread` parallelizes KNN lookups across all cores (~5x)

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

### Read from file
```bash
./bin/point_analyzer --input data/points.txt
```

### Options
- `--count N`: Generate N random points (default: 10000)
- `--seed N`: Random seed (default: random)
- `--input FILE`: Read points from file
- `--topK N`: Show top N isolated points (default: 10)
- `--help`: Show help message

## Input Format

One point per line: `x,y`
```
0.0,0.0
1.5,2.3
0.998,0.002
```

## License

MIT
