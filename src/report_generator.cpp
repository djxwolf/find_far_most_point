#include "report_generator.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {
    std::string createBar(double percentage, int maxWidth = 40) {
        int numChars = static_cast<int>((percentage / 100.0) * maxWidth);
        return std::string(numChars, '#');
    }

    bool hasNames(const AnalysisResult& result) {
        return !result.mostIsolated.name.empty();
    }

    std::string fmt(double val, int prec) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(prec) << val;
        return oss.str();
    }
}

void ReportGenerator::generate(const AnalysisResult& result,
                              ReportFormat format,
                              std::ostream& out) {
    if (format == ReportFormat::Text) {
        // --- Calculate dynamic box width from actual content ---
        int maxNameLen = 0;
        if (!result.mostIsolated.name.empty())
            maxNameLen = std::max(maxNameLen, static_cast<int>(result.mostIsolated.name.size()));
        const size_t maxRows = std::min(result.topK.size(), size_t(10));
        for (size_t i = 0; i < maxRows; ++i)
            maxNameLen = std::max(maxNameLen, static_cast<int>(result.topK[i].name.size()));

        // top-K row = 42 + nameLen;  distribution bar line <= 68
        const int W = std::max(68, 42 + maxNameLen);

        // --- Build border helpers ---
        const std::string DH = "\xE2\x95\x90";  // ═ (U+2550)
        auto repeatDH = [&DH](int n) {
            std::string s;
            s.reserve(n * DH.size());
            for (int i = 0; i < n; ++i) s += DH;
            return s;
        };
        auto makeBorder = [&](const char* left, const char* right) {
            return std::string(left) + repeatDH(W) + right + "\n";
        };

        const std::string topBorder    = makeBorder("\xE2\x95\x94", "\xE2\x95\x97");
        const std::string midBorder    = makeBorder("\xE2\x95\xA0", "\xE2\x95\xA3");
        const std::string bottomBorder = makeBorder("\xE2\x95\x9A", "\xE2\x95\x9D");

        auto line = [&](const std::string& content) -> std::string {
            int pad = W - 2 - static_cast<int>(content.size());
            if (pad < 0) pad = 0;
            return "\u2551 " + content + std::string(pad, ' ') + " \u2551\n";
        };

        out << "\n" << topBorder;
        out << line("              POINT ANALYSIS REPORT");
        out << midBorder;
        out << line("Execution Time:        " + fmt(result.executionTimeMs, 2) + " ms");
        out << midBorder;
        out << line("MOST ISOLATED POINT");
        out << midBorder;
        if (hasNames(result)) {
            out << line("Name:                  " + result.mostIsolated.name);
        }
        out << line("Coordinates:           (" + fmt(result.mostIsolated.x, 6)
            + ", " + fmt(result.mostIsolated.y, 6) + ")");
        out << line("Min Neighbor Distance: " + fmt(result.minDistance, 6));
        out << midBorder;

        // Top-K header
        std::ostringstream header;
        header << "TOP " << std::setw(2) << std::min(result.topK.size(), size_t(99))
               << " MOST ISOLATED POINTS";
        out << line(header.str());
        out << midBorder;

        // Name column width = actual max name length (no truncation)
        const int nameColW = maxNameLen;

        // Table header
        {
            std::ostringstream hdr;
            hdr << " Rank  " << std::left
                << std::setw(nameColW) << "Name" << std::right
                << "  Coordinates        Dist    Pct";
            out << line(hdr.str());
        }

        // Table rows
        for (size_t i = 0; i < maxRows; ++i) {
            double pct = 100.0 * (1.0 - static_cast<double>(i) / result.topK.size());

            std::ostringstream row;
            row << " " << std::setw(4) << (i + 1) << "  ";
            row << std::left
                << std::setw(nameColW) << result.topK[i].name << std::right;
            row << "  (" << fmt(result.topK[i].x, 4) << ", "
                << fmt(result.topK[i].y, 4) << ")  ";
            row << std::setw(7) << fmt(result.topK[i].minDist, 4)
                << "  " << std::setw(5) << fmt(pct, 1) << "%";
            out << line(row.str());
        }

        out << midBorder;

        // Statistics
        if (result.hasStats) {
            out << line("DISTANCE STATISTICS");
            out << midBorder;
            out << line("Min Nearest Distance:   " + fmt(result.stats.minNearestDistance, 6));
            out << line("Max Nearest Distance:   " + fmt(result.stats.maxNearestDistance, 6));
            out << line("Mean Nearest Distance:  " + fmt(result.stats.meanNearestDistance, 6));
            out << line("Median Nearest Dist:    " + fmt(result.stats.medianNearestDistance, 6));
            out << line("Std Deviation:          " + fmt(result.stats.stdDeviation, 6));

            if (!result.stats.distribution.empty()) {
                out << midBorder;
                std::ostringstream distTitle;
                distTitle << "DISTANCE DISTRIBUTION (" << result.stats.distribution.size() << " bins)";
                out << line(distTitle.str());
                out << midBorder;

                for (const auto& bin : result.stats.distribution) {
                    std::ostringstream entry;
                    entry << std::fixed << std::setprecision(4)
                          << "[" << bin.lowerBound << " - " << bin.upperBound << ")  "
                          << createBar(bin.percentage) << "  "
                          << std::setprecision(1) << bin.percentage << "%";
                    out << line(entry.str());
                }
            }
        }

        out << bottomBorder << "\n";

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
