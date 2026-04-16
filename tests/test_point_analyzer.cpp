#include <gtest/gtest.h>
#include "point_analyzer.h"
#include "report_generator.h"
#include <sstream>
#include <string>
#include <unordered_map>

TEST(PointTest, DefaultConstruction) {
    Point p;
    EXPECT_DOUBLE_EQ(p.x, 0.0);
    EXPECT_DOUBLE_EQ(p.y, 0.0);
}

TEST(PointTest, ParameterizedConstruction) {
    Point p{1.5, 2.5};
    EXPECT_DOUBLE_EQ(p.x, 1.5);
    EXPECT_DOUBLE_EQ(p.y, 2.5);
}

TEST(NamedPointTest, DefaultConstruction) {
    NamedPoint np;
    EXPECT_TRUE(np.name.empty());
    EXPECT_DOUBLE_EQ(np.x, 0.0);
    EXPECT_DOUBLE_EQ(np.y, 0.0);
}

TEST(KDTreeAdapterTest, PointCount) {
    std::vector<Point> points = {{0, 0}, {1, 1}, {2, 2}};
    KDTreeAdapter adapter(points);
    EXPECT_EQ(adapter.kdtree_get_point_count(), 3);
}

TEST(PointAnalyzerTest, NamedConstructor) {
    std::unordered_map<std::string, Point> named = {
        {"U1.1", {0, 0}}, {"U1.2", {1, 1}}, {"U2.A", {5, 5}}
    };
    EXPECT_NO_THROW({ PointAnalyzer analyzer(named); });
}

TEST(PointAnalyzerTest, VectorConstructor) {
    std::vector<Point> points = {{0, 0}, {1, 1}, {2, 2}};
    PointAnalyzer analyzer(points);
    SUCCEED();
}

TEST(PointAnalyzerTest, AnalyzeNamedCluster) {
    std::unordered_map<std::string, Point> named = {
        {"U1.1", {0, 0}}, {"U1.2", {0.1, 0.1}}, {"U1.3", {0.2, 0.2}},
        {"U2.A", {5, 5}}
    };
    PointAnalyzer analyzer(named);
    auto result = analyzer.analyze();
    EXPECT_EQ(result.mostIsolated.name, "U2.A");
    EXPECT_DOUBLE_EQ(result.mostIsolated.x, 5.0);
    EXPECT_DOUBLE_EQ(result.mostIsolated.y, 5.0);
}

TEST(PointAnalyzerTest, AnalyzeTopKNamed) {
    std::unordered_map<std::string, Point> named = {
        {"U1.1", {0, 0}}, {"U1.2", {0.1, 0.1}}, {"U1.3", {0.2, 0.2}},
        {"U2.A", {5, 5}}, {"U2.B", {3, 3}}
    };
    PointAnalyzer analyzer(named);
    auto result = analyzer.analyze(3);
    EXPECT_EQ(result.topK.size(), 3);
    EXPECT_EQ(result.topK[0].name, "U2.A");
    EXPECT_EQ(result.topK[1].name, "U2.B");
}

TEST(PointAnalyzerTest, AnalyzeEmpty) {
    std::vector<Point> points;
    PointAnalyzer analyzer(points);
    auto result = analyzer.analyze();
    EXPECT_TRUE(result.mostIsolated.name.empty());
}

TEST(PointAnalyzerTest, AnalyzeStatisticsBasic) {
    std::vector<Point> points = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    PointAnalyzer analyzer(points);
    auto result = analyzer.analyze(10, true);
    EXPECT_TRUE(result.hasStats);
    EXPECT_GT(result.stats.meanNearestDistance, 0.0);
}

TEST(PointAnalyzerTest, AnalyzeNoStats) {
    std::vector<Point> points = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    PointAnalyzer analyzer(points);
    auto result = analyzer.analyze(10, false);
    EXPECT_FALSE(result.hasStats);
}

TEST(ReportGeneratorTest, GenerateTextReport) {
    AnalysisResult result;
    result.mostIsolated = {"U1.1", 5.0, 5.0};
    result.minDistance = 2.5;
    result.topK = {{"U1.1", 5.0, 5.0}, {"U2.A", 3.0, 3.0}};
    result.executionTimeMs = 123.45;

    std::ostringstream oss;
    ReportGenerator::generate(result, ReportFormat::Text, oss);

    std::string output = oss.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("U1.1"), std::string::npos);
}
