#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "point_analyzer.h"
#include "report_generator.h"

struct CommandLineArgs {
    size_t count = 10000;
    unsigned int seed = std::random_device{}();
    std::string inputFile;
    size_t topK = 10;
    bool benchmark = false;
};

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]\n\n"
              << "Options:\n"
              << "  --count N         Generate N random points (default: 10000)\n"
              << "  --seed N          Random seed (default: random)\n"
              << "  --input FILE      Read points from file (one per line: x,y)\n"
              << "  --topK N          Show top N most isolated points (default: 10)\n"
              << "  --benchmark       Run benchmark mode\n"
              << "  --help            Show this help message\n\n"
              << "Examples:\n"
              << "  " << programName << " --count 1000000 --seed 42\n"
              << "  " << programName << " --input data/points.txt\n";
}

CommandLineArgs parseArguments(int argc, char* argv[]) {
    CommandLineArgs args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            std::exit(0);
        } else if (arg == "--count" && i + 1 < argc) {
            args.count = std::stoull(argv[++i]);
        } else if (arg == "--seed" && i + 1 < argc) {
            args.seed = std::stoul(argv[++i]);
        } else if (arg == "--input" && i + 1 < argc) {
            args.inputFile = argv[++i];
        } else if (arg == "--topK" && i + 1 < argc) {
            args.topK = std::stoull(argv[++i]);
        } else if (arg == "--benchmark") {
            args.benchmark = true;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            std::exit(1);
        }
    }
    return args;
}

std::vector<Point> generateRandomPoints(size_t count, unsigned int seed) {
    std::vector<Point> points;
    points.reserve(count);
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    for (size_t i = 0; i < count; ++i) {
        points.push_back({dis(gen), dis(gen)});
    }
    return points;
}

std::vector<Point> readPointsFromFile(const std::string& filename) {
    std::vector<Point> points;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file: " << filename << "\n";
        return points;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        char comma;
        Point p;
        if (iss >> p.x >> comma >> p.y && comma == ',') {
            points.push_back(p);
        }
    }
    return points;
}

int main(int argc, char* argv[]) {
    CommandLineArgs args = parseArguments(argc, argv);

    std::vector<Point> points;
    if (!args.inputFile.empty()) {
        points = readPointsFromFile(args.inputFile);
        if (points.empty()) {
            std::cerr << "Error: No points loaded from file\n";
            return 1;
        }
    } else {
        points = generateRandomPoints(args.count, args.seed);
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    PointAnalyzer analyzer(points);
    Point mostIsolated = analyzer.findMostIsolated();
    auto topK = analyzer.findTopKIsolated(args.topK);
    Statistics stats = analyzer.computeStatistics();

    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();

    AnalysisResult result;
    result.mostIsolated = mostIsolated;
    result.minDistance = stats.minNearestDistance;
    result.topK = topK;
    result.stats = stats;
    result.executionTimeMs = elapsedMs;

    ReportGenerator::generate(result, ReportFormat::Text);

    return 0;
}
