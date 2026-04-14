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
