#include "point_analyzer.h"
#include "report_generator.h"
#include <iostream>

int main() {
    std::vector<Point> points = {
        {0, 0}, {1, 1}, {2, 2}, {5, 5}
    };

    PointAnalyzer analyzer(points);
    Point result = analyzer.findMostIsolated();

    AnalysisResult analysisResult;
    analysisResult.mostIsolated = result;
    analysisResult.minDistance = 2.5;
    analysisResult.executionTimeMs = 123.45;

    ReportGenerator::generate(analysisResult, ReportFormat::Text);

    return 0;
}
