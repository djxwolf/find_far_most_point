#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include "point_analyzer.h"
#include <ostream>
#include <iostream>

enum class ReportFormat {
    Text,
    JSON,
    CSV
};

class ReportGenerator {
public:
    static void generate(const AnalysisResult& result,
                        ReportFormat format = ReportFormat::Text,
                        std::ostream& out = std::cout);
};

#endif
