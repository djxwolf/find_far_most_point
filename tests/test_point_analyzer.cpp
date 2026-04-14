#include <gtest/gtest.h>
#include "point_analyzer.h"

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
    EXPECT_DOUBLE_EQ(adapter.kdtree_get_pt(0, 0), 1.5);  // x of point 0
    EXPECT_DOUBLE_EQ(adapter.kdtree_get_pt(0, 1), 2.5);  // y of point 0
    EXPECT_DOUBLE_EQ(adapter.kdtree_get_pt(1, 0), 3.5);  // x of point 1
    EXPECT_DOUBLE_EQ(adapter.kdtree_get_pt(1, 1), 4.5);  // y of point 1
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
    // Should build KD-Tree without throwing
    SUCCEED();
}
