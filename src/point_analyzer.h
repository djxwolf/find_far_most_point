#ifndef POINT_ANALYZER_H
#define POINT_ANALYZER_H

#include <vector>
#include <memory>

struct Point {
    double x = 0.0;
    double y = 0.0;
};

struct HistogramBin {
    double lowerBound;
    double upperBound;
    size_t count;
    double percentage;
};

struct Statistics {
    double minNearestDistance = 0.0;
    double maxNearestDistance = 0.0;
    double meanNearestDistance = 0.0;
    double medianNearestDistance = 0.0;
    double stdDeviation = 0.0;
    std::vector<HistogramBin> distribution;
};

struct AnalysisResult {
    Point mostIsolated{0.0, 0.0};
    double minDistance = 0.0;
    std::vector<Point> topK;
    Statistics stats;
    bool hasStats = false;
    double executionTimeMs = 0.0;
};

// KD-Tree adapter for nanoflann
class KDTreeAdapter {
public:
    explicit KDTreeAdapter(const std::vector<Point>& points) : pts_(points) {}

    inline size_t kdtree_get_point_count() const { return pts_.size(); }

    inline double kdtree_get_pt(const size_t idx, const size_t dim) const {
        return dim == 0 ? pts_[idx].x : pts_[idx].y;
    }

    template <class BBOX>
    bool kdtree_get_bbox(BBOX&) const { return false; }

private:
    const std::vector<Point>& pts_;
};

#include <nanoflann.hpp>

using KDTree = nanoflann::KDTreeSingleIndexAdaptor<
    nanoflann::L2_Simple_Adaptor<double, KDTreeAdapter>,
    KDTreeAdapter, 2
>;

class PointAnalyzer {
public:
    explicit PointAnalyzer(const std::vector<Point>& points);
    ~PointAnalyzer() = default;

    PointAnalyzer(const PointAnalyzer&) = delete;
    PointAnalyzer& operator=(const PointAnalyzer&) = delete;

    AnalysisResult analyze(size_t topK = 10, bool computeStats = true);

private:
    std::vector<Point> points_;
    std::unique_ptr<KDTree> kdtree_;
    KDTreeAdapter adapter_;
};

#endif // POINT_ANALYZER_H
