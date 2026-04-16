#include "src/point_analyzer.h"
#include "src/report_generator.h"
#include <iostream>
#include <unordered_map>

int main() {
    std::unordered_map<std::string, Point> named = {
        {"U1.1", {0, 0}}, {"U1.2", {0.1, 0.1}}, {"U1.3", {0.2, 0.2}},
        {"U2.A", {5, 5}}, {"U2.B", {3, 3}}, {"U3.CLK", {7, 7}}
    };

    PointAnalyzer analyzer(named);
    AnalysisResult result = analyzer.analyze(5);
    result.executionTimeMs = 42.5;

    ReportGenerator::generate(result, ReportFormat::Text, std::cout);
    return 0;
}
