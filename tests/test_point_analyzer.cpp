#include <gtest/gtest.h>
#include "point_analyzer.h"
#include "report_generator.h"
#include <sstream>
#include <string>

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

TEST(StatisticsTest, DefaultInitialization) {
    Statistics stats;
    EXPECT_DOUBLE_EQ(stats.minNearestDistance, 0.0);
    EXPECT_DOUBLE_EQ(stats.maxNearestDistance, 0.0);
}

TEST(AnalysisResultTest, DefaultInitialization) {
    AnalysisResult result;
    EXPECT_DOUBLE_EQ(result.mostIsolated.x, 0.0);
    EXPECT_DOUBLE_EQ(result.mostIsolated.y, 0.0);
    EXPECT_DOUBLE_EQ(result.minDistance, 0.0);
    EXPECT_TRUE(result.topK.empty());
    EXPECT_DOUBLE_EQ(result.executionTimeMs, 0.0);
}

TEST(KDTreeAdapterTest, PointCount) {
    std::vector<Point> points = {{0, 0}, {1, 1}, {2, 2}};
    KDTreeAdapter adapter(points);
    EXPECT_EQ(adapter.kdtree_get_point_count(), 3);
}

TEST(KDTreeAdapterTest, GetPoint) {
    std::vector<Point> points = {{1.5, 2.5}, {3.5, 4.5}};
    KDTreeAdapter adapter(points);
    EXPECT_DOUBLE_EQ(adapter.kdtree_get_pt(0, 0), 1.5);
    EXPECT_DOUBLE_EQ(adapter.kdtree_get_pt(0, 1), 2.5);
    EXPECT_DOUBLE_EQ(adapter.kdtree_get_pt(1, 0), 3.5);
    EXPECT_DOUBLE_EQ(adapter.kdtree_get_pt(1, 1), 4.5);
}

TEST(PointAnalyzerTest, ConstructorWithEmptyPoints) {
    std::vector<Point> points;
    EXPECT_NO_THROW({
        PointAnalyzer analyzer(points);
    });
}

TEST(PointAnalyzerTest, ConstructorWithPoints) {
    std::vector<Point> points = {{0, 0}, {1, 1}, {2, 2}};
    PointAnalyzer analyzer(points);
    SUCCEED();
}

TEST(PointAnalyzerTest, AnalyzeSinglePoint) {
    std::vector<Point> points = {{5, 5}};
    PointAnalyzer analyzer(points);
    auto result = analyzer.analyze();
    EXPECT_DOUBLE_EQ(result.mostIsolated.x, 5.0);
    EXPECT_DOUBLE_EQ(result.mostIsolated.y, 5.0);
}

TEST(PointAnalyzerTest, AnalyzeTwoPoints) {
    std::vector<Point> points = {{0, 0}, {10, 10}};
    PointAnalyzer analyzer(points);
    auto result = analyzer.analyze();
    EXPECT_TRUE(result.mostIsolated.x == 0.0 || result.mostIsolated.x == 10.0);
}

TEST(PointAnalyzerTest, AnalyzeCluster) {
    std::vector<Point> points = {
        {0, 0}, {0.1, 0.1}, {0.2, 0.2},
        {5, 5}
    };
    PointAnalyzer analyzer(points);
    auto result = analyzer.analyze();
    EXPECT_DOUBLE_EQ(result.mostIsolated.x, 5.0);
    EXPECT_DOUBLE_EQ(result.mostIsolated.y, 5.0);
}

TEST(PointAnalyzerTest, AnalyzeEmpty) {
    std::vector<Point> points;
    PointAnalyzer analyzer(points);
    auto result = analyzer.analyze();
    EXPECT_DOUBLE_EQ(result.mostIsolated.x, 0.0);
    EXPECT_DOUBLE_EQ(result.mostIsolated.y, 0.0);
}

TEST(PointAnalyzerTest, AnalyzeTopK) {
    std::vector<Point> points = {
        {0, 0}, {0.1, 0.1}, {0.2, 0.2},
        {5, 5},
        {3, 3}
    };
    PointAnalyzer analyzer(points);
    auto result = analyzer.analyze(3);
    EXPECT_EQ(result.topK.size(), 3);
    EXPECT_DOUBLE_EQ(result.topK[0].x, 5.0);
    EXPECT_DOUBLE_EQ(result.topK[1].x, 3.0);
}

TEST(PointAnalyzerTest, AnalyzeTopKGreaterThanPoints) {
    std::vector<Point> points = {{0, 0}, {1, 1}};
    PointAnalyzer analyzer(points);
    auto result = analyzer.analyze(5);
    EXPECT_EQ(result.topK.size(), 2);
}

TEST(PointAnalyzerTest, AnalyzeStatisticsEmpty) {
    std::vector<Point> points;
    PointAnalyzer analyzer(points);
    auto result = analyzer.analyze();
    EXPECT_DOUBLE_EQ(result.stats.minNearestDistance, 0.0);
}

TEST(PointAnalyzerTest, AnalyzeStatisticsSinglePoint) {
    std::vector<Point> points = {{5, 5}};
    PointAnalyzer analyzer(points);
    auto result = analyzer.analyze();
    EXPECT_EQ(result.stats.distribution.size(), 0);
}

TEST(PointAnalyzerTest, AnalyzeStatisticsBasic) {
    std::vector<Point> points = {
        {0, 0}, {1, 0}, {0, 1}, {1, 1}
    };
    PointAnalyzer analyzer(points);
    auto result = analyzer.analyze();
    EXPECT_GT(result.stats.meanNearestDistance, 0.0);
    EXPECT_GT(result.stats.minNearestDistance, 0.0);
    EXPECT_GT(result.stats.maxNearestDistance, 0.0);
    EXPECT_GE(result.stats.stdDeviation, 0.0);
}

TEST(ReportGeneratorTest, GenerateTextReport) {
    AnalysisResult result;
    result.mostIsolated = {5.0, 5.0};
    result.minDistance = 2.5;
    result.topK = {{5.0, 5.0}, {3.0, 3.0}};
    result.executionTimeMs = 123.45;

    std::ostringstream oss;
    ReportGenerator::generate(result, ReportFormat::Text, oss);

    std::string output = oss.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("5.00"), std::string::npos);
}
