#include "point_analyzer.h"

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
