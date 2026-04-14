#include "point_analyzer.h"
#include <array>
#include <cmath>

PointAnalyzer::PointAnalyzer(const std::vector<Point>& points)
    : points_(points)
    , adapter_(points_)
{
    // Build KD-Tree index
    if (!points_.empty()) {
        kdtree_ = std::make_unique<KDTree>(
            2 /* dimension */,
            adapter_,
            nanoflann::KDTreeSingleIndexAdaptorParams(10 /* max leaf */)
        );
        kdtree_->buildIndex();
    }
}

Point PointAnalyzer::findMostIsolated() {
    if (points_.empty()) {
        return {0.0, 0.0};
    }
    if (points_.size() == 1) {
        return points_[0];
    }

    double maxMinDistance = -1.0;
    size_t farthestIdx = 0;

    for (size_t i = 0; i < points_.size(); ++i) {
        const double query_pt[2] = {points_[i].x, points_[i].y};
        std::array<size_t, 2> indices;
        std::array<double, 2> distances_sqr;
        nanoflann::KNNResultSet<double> resultSet(2);
        resultSet.init(indices.data(), distances_sqr.data());
        kdtree_->findNeighbors(resultSet, query_pt, nanoflann::SearchParameters());

        double minDistance = std::sqrt((indices[0] == i) ? distances_sqr[1] : distances_sqr[0]);

        if (minDistance > maxMinDistance) {
            maxMinDistance = minDistance;
            farthestIdx = i;
        }
    }
    return points_[farthestIdx];
}
