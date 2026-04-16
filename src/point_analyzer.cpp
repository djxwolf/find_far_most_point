#include "point_analyzer.h"
#include <array>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <thread>

namespace {
    double computeMedian(std::vector<double>& v) {
        if (v.empty()) return 0.0;
        std::sort(v.begin(), v.end());
        size_t n = v.size();
        return (n % 2 == 0) ? (v[n/2 - 1] + v[n/2]) / 2.0 : v[n/2];
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

void PointAnalyzer::buildIndex() {
    if (!points_.empty()) {
        kdtree_ = std::make_unique<KDTree>(
            2, adapter_, nanoflann::KDTreeSingleIndexAdaptorParams(10)
        );
        kdtree_->buildIndex();
    }
}

PointAnalyzer::PointAnalyzer(const std::unordered_map<std::string, Point>& namedPoints)
    : adapter_(points_)
{
    points_.reserve(namedPoints.size());
    names_.reserve(namedPoints.size());
    for (const auto& [name, pt] : namedPoints) {
        names_.push_back(name);
        points_.push_back(pt);
    }
    buildIndex();
}

PointAnalyzer::PointAnalyzer(const std::vector<Point>& points)
    : points_(points)
    , names_(points.size())
    , adapter_(points_)
{
    buildIndex();
}

AnalysisResult PointAnalyzer::analyze(size_t topK, bool computeStats) {
    AnalysisResult result;
    const size_t n = points_.size();

    if (n == 0) return result;
    if (n == 1) {
        result.mostIsolated = {names_[0], points_[0].x, points_[0].y};
        result.minDistance = 0.0;
        result.topK.push_back({names_[0], points_[0].x, points_[0].y});
        return result;
    }

    // Phase 1: compute squared nearest-neighbor distances in parallel
    std::vector<double> sqDists(n);

    const unsigned hwThreads = std::thread::hardware_concurrency();
    const unsigned numThreads = std::max(1u, hwThreads ? hwThreads : 1u);
    const size_t chunkSize = (n + numThreads - 1) / numThreads;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    for (unsigned t = 0; t < numThreads; ++t) {
        const size_t start = t * chunkSize;
        const size_t end = std::min(start + chunkSize, n);
        if (start >= n) break;
        threads.emplace_back([this, &sqDists, start, end]() {
            for (size_t i = start; i < end; ++i) {
                const double query_pt[2] = {points_[i].x, points_[i].y};
                std::array<size_t, 2> indices;
                std::array<double, 2> out_dists_sqr;
                nanoflann::KNNResultSet<double> resultSet(2);
                resultSet.init(indices.data(), out_dists_sqr.data());
                kdtree_->findNeighbors(resultSet, query_pt, nanoflann::SearchParameters());
                sqDists[i] = (indices[0] == i) ? out_dists_sqr[1] : out_dists_sqr[0];
            }
        });
    }
    for (auto& th : threads) th.join();

    // Phase 2: most isolated point (compare on squared distances)
    size_t farthestIdx = 0;
    double maxSqDist = sqDists[0];
    for (size_t i = 1; i < n; ++i) {
        if (sqDists[i] > maxSqDist) {
            maxSqDist = sqDists[i];
            farthestIdx = i;
        }
    }
    result.mostIsolated = {names_[farthestIdx], points_[farthestIdx].x, points_[farthestIdx].y};
    result.minDistance = std::sqrt(maxSqDist);

    // Phase 3: top-K (partial_sort on squared distances)
    std::vector<size_t> order(n);
    std::iota(order.begin(), order.end(), 0);
    const size_t k = std::min(topK, n);
    std::partial_sort(order.begin(), order.begin() + k, order.end(),
        [&sqDists](size_t a, size_t b) { return sqDists[a] > sqDists[b]; });
    result.topK.resize(k);
    for (size_t i = 0; i < k; ++i) {
        size_t idx = order[i];
        result.topK[i] = {names_[idx], points_[idx].x, points_[idx].y};
    }

    // Phase 4: statistics (optional)
    if (computeStats) {
        result.hasStats = true;
        std::vector<double> dists(n);
        for (size_t i = 0; i < n; ++i) {
            dists[i] = std::sqrt(sqDists[i]);
        }

        auto& stats = result.stats;
        stats.minNearestDistance = *std::min_element(dists.begin(), dists.end());
        stats.maxNearestDistance = *std::max_element(dists.begin(), dists.end());

        double sum = 0.0;
        for (double d : dists) sum += d;
        stats.meanNearestDistance = sum / static_cast<double>(n);

        stats.medianNearestDistance = computeMedian(dists);
        stats.stdDeviation = computeStdDev(dists, stats.meanNearestDistance);

        const size_t numBins = 10;
        double binWidth = (stats.maxNearestDistance - stats.minNearestDistance) / static_cast<double>(numBins);
        if (binWidth > 0) {
            std::vector<size_t> binCounts(numBins, 0);
            for (double d : dists) {
                size_t bin = static_cast<size_t>((d - stats.minNearestDistance) / binWidth);
                if (bin >= numBins) bin = numBins - 1;
                binCounts[bin]++;
            }
            stats.distribution.reserve(numBins);
            for (size_t i = 0; i < numBins; ++i) {
                HistogramBin bin;
                bin.lowerBound = stats.minNearestDistance + i * binWidth;
                bin.upperBound = bin.lowerBound + binWidth;
                bin.count = binCounts[i];
                bin.percentage = (binCounts[i] * 100.0) / static_cast<double>(n);
                stats.distribution.push_back(bin);
            }
        }
    }

    return result;
}
