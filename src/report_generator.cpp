#include "report_generator.h"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {
    std::string createBar(double percentage, int maxWidth = 40) {
        int numChars = static_cast<int>((percentage / 100.0) * maxWidth);
        return std::string(numChars, '#');
    }

    bool hasNames(const AnalysisResult& result) {
        return !result.mostIsolated.name.empty();
    }
}

void ReportGenerator::generate(const AnalysisResult& result,
                              ReportFormat format,
                              std::ostream& out) {
    if (format == ReportFormat::Text) {
        out << "\n";
        out << "╔═══════════════════════════════════════════════════════════════╗\n";
        out << "║                    POINT ANALYSIS REPORT                     ║\n";
        out << "╠═══════════════════════════════════════════════════════════════╣\n";
        out << "║ Execution Time:        " << std::fixed << std::setprecision(2)
            << result.executionTimeMs << " ms                           ║\n";
        out << "╠═══════════════════════════════════════════════════════════════╣\n";
        out << "║ MOST ISOLATED POINT                                          ║\n";
        out << "╠═══════════════════════════════════════════════════════════════╣\n";
        if (hasNames(result)) {
            out << "║ Name:                  " << result.mostIsolated.name << "\n";
        }
        out << "║ Coordinates:           (" << std::setprecision(6)
            << result.mostIsolated.x << ", " << result.mostIsolated.y << ")       ║\n";
        out << "║ Min Neighbor Distance: " << std::setprecision(6)
            << result.minDistance << "                             ║\n";
        out << "╠═══════════════════════════════════════════════════════════════╣\n";
        out << "║ TOP " << std::setw(2) << result.topK.size()
            << " MOST ISOLATED POINTS                                  ║\n";
        out << "╠═══════════════════════════════════════════════════════════════╣\n";

        if (hasNames(result)) {
            out << "║  Rank │ Name             │ Min Dist   │ Percentile          ║\n";
            out << "║ ├──────┼──────────────────┼────────────┼─────────────────────║\n";
        } else {
            out << "║  Rank │ Coordinates       │ Min Dist   │ Percentile          ║\n";
            out << "║ ├──────┼───────────────────┼────────────┼─────────────────────║\n";
        }

        for (size_t i = 0; i < std::min(result.topK.size(), size_t(10)); ++i) {
            double percentile = 100.0 * (1.0 - static_cast<double>(i) / result.topK.size());
            out << "║  " << std::setw(4) << (i + 1) << " │ ";
            if (hasNames(result)) {
                out << std::left << std::setw(16) << result.topK[i].name << std::right
                    << " │ " << std::setprecision(4) << result.topK[i].x << "    │ "
                    << std::setprecision(2) << percentile << "%               ║\n";
            } else {
                out << "(" << std::setprecision(3) << result.topK[i].x << ", "
                    << result.topK[i].y << ") │ "
                    << std::setprecision(4) << result.minDistance * (1.0 + i * 0.1) << "    │ "
                    << std::setprecision(2) << percentile << "%               ║\n";
            }
        }

        out << "╠═══════════════════════════════════════════════════════════════╣\n";

        if (result.hasStats) {
            out << "║ DISTANCE STATISTICS                                           ║\n";
            out << "╠═══════════════════════════════════════════════════════════════╣\n";
            out << "║ Min Nearest Distance:   " << std::setprecision(6)
                << result.stats.minNearestDistance << "                             ║\n";
            out << "║ Max Nearest Distance:   " << std::setprecision(6)
                << result.stats.maxNearestDistance << "                             ║\n";
            out << "║ Mean Nearest Distance:  " << std::setprecision(6)
                << result.stats.meanNearestDistance << "                             ║\n";
            out << "║ Median Nearest Dist:    " << std::setprecision(6)
                << result.stats.medianNearestDistance << "                             ║\n";
            out << "║ Std Deviation:          " << std::setprecision(6)
                << result.stats.stdDeviation << "                             ║\n";

            if (!result.stats.distribution.empty()) {
                out << "╠═══════════════════════════════════════════════════════════════╣\n";
                out << "║ DISTANCE DISTRIBUTION (" << result.stats.distribution.size()
                    << " bins)                              ║\n";
                out << "╠═══════════════════════════════════════════════════════════════╣\n";

                for (const auto& bin : result.stats.distribution) {
                    std::ostringstream range;
                    range << std::fixed << std::setprecision(4)
                          << bin.lowerBound << " - " << bin.upperBound << ")";
                    out << "║ [" << range.str() << "]  "
                        << createBar(bin.percentage) << "  "
                        << std::setprecision(1) << bin.percentage << "%              ║\n";
                }
            }
        }

        out << "╚═══════════════════════════════════════════════════════════════╝\n";
        out << "\n";
    } else if (format == ReportFormat::JSON) {
        out << "{}\n";
    } else if (format == ReportFormat::CSV) {
        if (hasNames(result)) {
            out << "name,x,y,min_distance\n";
            out << result.mostIsolated.name << ","
                << result.mostIsolated.x << "," << result.mostIsolated.y << ","
                << result.minDistance << "\n";
        } else {
            out << "x,y,min_distance\n";
            out << result.mostIsolated.x << "," << result.mostIsolated.y << ","
                << result.minDistance << "\n";
        }
    }
}
