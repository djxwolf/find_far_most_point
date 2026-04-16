#include "src/point_analyzer.h"
#include "src/report_generator.h"
#include <iostream>
#include <vector>

int main() {
    std::vector<Point> points = {
        {0, 0}, {0.1, 0.1}, {0.2, 0.2}, {0.3, 0.3},
        {5, 5}, {3, 3}, {7, 7}, {1, 1},
        {2, 2}, {4, 4}
    };

    PointAnalyzer analyzer(points);
    AnalysisResult result = analyzer.analyze(5);
    result.executionTimeMs = 42.5;

    ReportGenerator::generate(result, ReportFormat::Text, std::cout);
    return 0;
}
