#ifndef POINT_ANALYZER_H
#define POINT_ANALYZER_H

#include <vector>
#include <memory>

struct Point {
    double x = 0.0;
    double y = 0.0;
};

struct HistogramBin {
    double lowerBound;
    double upperBound;
    size_t count;
    double percentage;
};

struct Statistics {
    double minNearestDistance = 0.0;
    double maxNearestDistance = 0.0;
    double meanNearestDistance = 0.0;
    double medianNearestDistance = 0.0;
    double stdDeviation = 0.0;
    std::vector<HistogramBin> distribution;
};

struct AnalysisResult {
    Point mostIsolated{0.0, 0.0};
    double minDistance = 0.0;
    std::vector<Point> topK;
    Statistics stats;
    double executionTimeMs = 0.0;
};

#endif // POINT_ANALYZER_H
