# Point Analyzer

High-performance 2D point set analysis using KD-Tree for O(n log n) nearest neighbor search.

## Features

- **Fast**: O(n log n) algorithm using KD-Tree (nanoflann)
- **Scalable**: Handles millions of points efficiently
- **Comprehensive**: Full analysis report with statistics and visualization

## Building

```bash
mkdir build && cd build
cmake ..
make
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

## Performance

| Points | Time | Memory |
|--------|------|--------|
| 100K   | ~30ms| ~2.4MB |
| 1M     | ~350ms| ~24MB |
| 10M    | ~4s  | ~240MB |

## License

MIT
