#include "point_analyzer.h"
#include <array>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace {
    double computeMedian(std::vector<double>& v) {
        if (v.empty()) return 0.0;
        std::sort(v.begin(), v.end());
        size_t n = v.size();
        if (n % 2 == 0) {
            return (v[n/2 - 1] + v[n/2]) / 2.0;
        } else {
            return v[n/2];
        }
    }

    double computeStdDev(const std::vector<double>& v, double mean) {
        if (v.size() <= 1) return 0.0;
        double sum_sq_diff = 0.0;
        for (double val : v) {
            double diff = val - mean;
            sum_sq_diff += diff * diff;
        }
        return std::sqrt(sum_sq_diff / static_cast<double>(v.size()));
    }
}

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

std::vector<Point> PointAnalyzer::findTopKIsolated(size_t k) {
    if (points_.empty() || k == 0) {
        return {};
    }

    struct PointWithDistance {
        Point point;
        double minDistance;
        size_t index;
    };

    std::vector<PointWithDistance> pointsWithDist;
    pointsWithDist.reserve(points_.size());

    for (size_t i = 0; i < points_.size(); ++i) {
        const double query_pt[2] = {points_[i].x, points_[i].y};
        std::array<size_t, 2> indices;
        std::array<double, 2> distances_sqr;
        nanoflann::KNNResultSet<double> resultSet(2);
        resultSet.init(indices.data(), distances_sqr.data());
        kdtree_->findNeighbors(resultSet, query_pt, nanoflann::SearchParameters());

        double minDistance;
        if (points_.size() == 1) {
            minDistance = 0.0;
        } else {
            minDistance = std::sqrt((indices[0] == i) ? distances_sqr[1] : distances_sqr[0]);
        }
        pointsWithDist.push_back({points_[i], minDistance, i});
    }

    std::sort(pointsWithDist.begin(), pointsWithDist.end(),
        [](const PointWithDistance& a, const PointWithDistance& b) {
            return a.minDistance > b.minDistance;
        });

    size_t resultSize = std::min(k, pointsWithDist.size());
    std::vector<Point> result;
    result.reserve(resultSize);

    for (size_t i = 0; i < resultSize; ++i) {
        result.push_back(pointsWithDist[i].point);
    }

    return result;
}

Statistics PointAnalyzer::computeStatistics() {
    Statistics stats;
    if (points_.empty() || points_.size() == 1) {
        return stats;
    }

    std::vector<double> nearestDistances;
    nearestDistances.reserve(points_.size());

    for (size_t i = 0; i < points_.size(); ++i) {
        const double query_pt[2] = {points_[i].x, points_[i].y};
        std::array<size_t, 2> indices;
        std::array<double, 2> distances_sqr;
        nanoflann::KNNResultSet<double> resultSet(2);
        resultSet.init(indices.data(), distances_sqr.data());
        kdtree_->findNeighbors(resultSet, query_pt, nanoflann::SearchParameters());

        double minDistance = std::sqrt((indices[0] == i) ? distances_sqr[1] : distances_sqr[0]);
        nearestDistances.push_back(minDistance);
    }

    stats.minNearestDistance = *std::min_element(nearestDistances.begin(), nearestDistances.end());
    stats.maxNearestDistance = *std::max_element(nearestDistances.begin(), nearestDistances.end());

    double sum = std::accumulate(nearestDistances.begin(), nearestDistances.end(), 0.0);
    stats.meanNearestDistance = sum / static_cast<double>(nearestDistances.size());

    stats.medianNearestDistance = computeMedian(nearestDistances);
    stats.stdDeviation = computeStdDev(nearestDistances, stats.meanNearestDistance);

    // Create histogram (10 bins)
    const size_t numBins = 10;
    double binWidth = (stats.maxNearestDistance - stats.minNearestDistance) / numBins;

    if (binWidth > 0) {
        std::vector<size_t> binCounts(numBins, 0);
        for (double dist : nearestDistances) {
            size_t binIndex = static_cast<size_t>((dist - stats.minNearestDistance) / binWidth);
            if (binIndex >= numBins) binIndex = numBins - 1;
            binCounts[binIndex]++;
        }
        for (size_t i = 0; i < numBins; ++i) {
            HistogramBin bin;
            bin.lowerBound = stats.minNearestDistance + i * binWidth;
            bin.upperBound = bin.lowerBound + binWidth;
            bin.count = binCounts[i];
            bin.percentage = (binCounts[i] * 100.0) / nearestDistances.size();
            stats.distribution.push_back(bin);
        }
    }

    return stats;
}
