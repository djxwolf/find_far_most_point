#include "report_generator.h"
#include <iomanip>
#include <iostream>

void ReportGenerator::generate(const AnalysisResult& result,
                              ReportFormat format,
                              std::ostream& out) {
    if (format == ReportFormat::Text) {
        out << "╔═══════════════════════════════════════════════════════════════╗\n";
        out << "║                    POINT ANALYSIS REPORT                     ║\n";
        out << "╠═══════════════════════════════════════════════════════════════╣\n";
        out << "║ Most Isolated Point:  (" << std::fixed << std::setprecision(6)
            << result.mostIsolated.x << ", " << result.mostIsolated.y << ")       ║\n";
        out << "║ Min Neighbor Distance: " << std::setprecision(6)
            << result.minDistance << "                                    ║\n";
        out << "║ Execution Time:        " << std::setprecision(2)
            << result.executionTimeMs << " ms                           ║\n";
        out << "╚═══════════════════════════════════════════════════════════════╝\n";
    } else if (format == ReportFormat::JSON) {
        out << "{}\n";
    } else if (format == ReportFormat::CSV) {
        out << "x,y,min_distance\n";
        out << result.mostIsolated.x << "," << result.mostIsolated.y << ","
            << result.minDistance << "\n";
    }
}
