#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "point_analyzer.h"
#include "report_generator.h"

struct CommandLineArgs {
    size_t count = 10000;
    unsigned int seed = std::random_device{}();
    std::string inputFile;
    size_t topK = 10;
    bool noStats = false;
};

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]\n\n"
              << "Options:\n"
              << "  --count N         Generate N random points (default: 10000)\n"
              << "  --seed N          Random seed (default: random)\n"
              << "  --input FILE      Read points from file (one per line: name,x,y or x,y)\n"
              << "  --topK N          Show top N most isolated points (default: 10)\n"
              << "  --no-stats        Skip statistics and histogram\n"
              << "  --help            Show this help message\n\n"
              << "Examples:\n"
              << "  " << programName << " --count 1000000 --seed 42\n"
              << "  " << programName << " --input data/pins.txt\n";
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
        } else if (arg == "--no-stats") {
            args.noStats = true;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            std::exit(1);
        }
    }
    return args;
}

std::unordered_map<std::string, Point> generateRandomPoints(size_t count, unsigned int seed) {
    std::unordered_map<std::string, Point> namedPoints;
    namedPoints.reserve(count);
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> coord(0.0, 1.0);
    std::uniform_int_distribution<int> nameType(0, 3);

    const char* prefixes[] = {"U", "IC", "R", "C", "J", "Q", "D", "L", "SW", "BTN"};
    const char* suffixes[] = {".CLK", ".RST", ".VCC", ".GND", ".EN", ".CS", ".SDA", ".SCL", ""};

    for (size_t i = 0; i < count; ++i) {
        std::string name;
        switch (nameType(gen)) {
            case 0: // short: U1
                name = std::string(prefixes[gen() % 10]) + std::to_string(i + 1);
                break;
            case 1: // medium: IC3.CLK
                name = std::string(prefixes[gen() % 10]) + std::to_string(i + 1) + suffixes[gen() % 9];
                break;
            case 2: // long: U5.PIN_A14
                name = std::string(prefixes[gen() % 10]) + std::to_string(i + 1) + ".PIN_" +
                       std::to_string(gen() % 100);
                break;
            default: // extra long: PROC_12.DATA_BUS_7
                name = std::string(prefixes[gen() % 10]) + "_" + std::to_string(i + 1) +
                       ".DATA_BUS_" + std::to_string(gen() % 64);
                break;
        }
        namedPoints.emplace(std::move(name), Point{coord(gen), coord(gen)});
    }
    return namedPoints;
}

// Parse file: auto-detect "name,x,y" (3 fields) vs "x,y" (2 fields)
std::unordered_map<std::string, Point> readPointsFromFile(const std::string& filename) {
    std::unordered_map<std::string, Point> namedPoints;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file: " << filename << "\n";
        return namedPoints;
    }

    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        ++lineNum;

        std::istringstream iss(line);
        std::string f1, f2, f3;
        if (std::getline(iss, f1, ',') && std::getline(iss, f2, ',') && std::getline(iss, f3)) {
            // 3 fields: name,x,y
            try {
                namedPoints[f1] = {std::stod(f2), std::stod(f3)};
            } catch (...) {
                std::cerr << "Warning: skipping malformed line " << lineNum << "\n";
            }
        } else {
            // 2 fields: x,y (auto-generate name)
            iss.clear();
            iss.str(line);
            char comma;
            Point p;
            if (iss >> p.x >> comma >> p.y && comma == ',') {
                namedPoints["P" + std::to_string(lineNum)] = p;
            }
        }
    }
    return namedPoints;
}

int main(int argc, char* argv[]) {
    CommandLineArgs args = parseArguments(argc, argv);

    auto startTime = std::chrono::high_resolution_clock::now();

    AnalysisResult result;
    if (!args.inputFile.empty()) {
        auto namedPoints = readPointsFromFile(args.inputFile);
        if (namedPoints.empty()) {
            std::cerr << "Error: No points loaded from file\n";
            return 1;
        }
        PointAnalyzer analyzer(namedPoints);
        result = analyzer.analyze(args.topK, !args.noStats);
    } else {
        auto namedPoints = generateRandomPoints(args.count, args.seed);
        PointAnalyzer analyzer(namedPoints);
        result = analyzer.analyze(args.topK, !args.noStats);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();

    ReportGenerator::generate(result, ReportFormat::Text);

    return 0;
}
