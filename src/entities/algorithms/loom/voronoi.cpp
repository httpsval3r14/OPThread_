#include "entities/algorithms/loom/voronoi.h"
#include "entities/algorithms/loom/loom_utils.h"
#include "entities/algorithms/loom/loom_constants.h"
#include "entities/SeedManager.hpp"
#include <vector>
#include <set>
#include <random>
#include <limits>
#include <cmath>

namespace entities::algorithms::loom {

    static std::vector<std::pair<float, float>> generatePoints(int numPoints, const sf::FloatRect& bounds, std::mt19937& rng) {
        std::vector<std::pair<float, float>> points;
        std::uniform_real_distribution<float> xDist(bounds.position.x + 10, bounds.position.x + bounds.size.x - 10);
        std::uniform_real_distribution<float> yDist(bounds.position.y + 10, bounds.position.y + bounds.size.y - 10);
        for (int i = 0; i < numPoints; ++i) {
            points.emplace_back(xDist(rng), yDist(rng));
        }
        return points;
    }

    static std::vector<std::pair<int, int>> computeVoronoiEdges(const std::vector<std::pair<float, float>>& points,
        const sf::FloatRect& bounds, int gridW, int gridH) {
        std::vector<int> label(gridW * gridH, -1);
        float cellW = bounds.size.x / gridW;
        float cellH = bounds.size.y / gridH;
        for (int gy = 0; gy < gridH; ++gy) {
            for (int gx = 0; gx < gridW; ++gx) {
                float cx = bounds.position.x + gx * cellW + cellW / 2;
                float cy = bounds.position.y + gy * cellH + cellH / 2;
                int closest = -1;
                float minDist = std::numeric_limits<float>::max();
                for (size_t i = 0; i < points.size(); ++i) {
                    float dx = cx - points[i].first;
                    float dy = cy - points[i].second;
                    float d2 = dx * dx + dy * dy;
                    if (d2 < minDist) { minDist = d2; closest = i; }
                }
                label[gy * gridW + gx] = closest;
            }
        }
        std::set<std::pair<int, int>> edgesSet;
        for (int gy = 0; gy < gridH; ++gy) {
            for (int gx = 0; gx < gridW; ++gx) {
                int idx = gy * gridW + gx;
                int lbl = label[idx];
                if (gx + 1 < gridW) {
                    int lblR = label[gy * gridW + (gx + 1)];
                    if (lbl != lblR) {
                        int a = std::min(lbl, lblR);
                        int b = std::max(lbl, lblR);
                        edgesSet.insert({ a, b });
                    }
                }
                if (gy + 1 < gridH) {
                    int lblD = label[(gy + 1) * gridW + gx];
                    if (lbl != lblD) {
                        int a = std::min(lbl, lblD);
                        int b = std::max(lbl, lblD);
                        edgesSet.insert({ a, b });
                    }
                }
            }
        }
        return std::vector<std::pair<int, int>>(edgesSet.begin(), edgesSet.end());
    }

    void voronoiLoomPattern(Graph& g, int depth) {
        const float minX = -LOOM_WIDTH / 2 + MARGIN;
        const float maxX = LOOM_WIDTH / 2 - MARGIN;
        const float minY = -LOOM_HEIGHT / 2 + MARGIN;
        const float maxY = LOOM_HEIGHT / 2 - MARGIN;
        sf::FloatRect bounds(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));

        auto& rng = SeedManager::instance().rng();
        int numPoints = std::max(30, depth * 15);
        auto points = generatePoints(numPoints, bounds, rng);
        std::vector<uint64_t> nodeIds(points.size());
        for (size_t i = 0; i < points.size(); ++i) {
            nodeIds[i] = g.addNode(points[i].first, points[i].second);
        }
        int gridW = 150, gridH = 112;
        auto edges = computeVoronoiEdges(points, bounds, gridW, gridH);
        for (auto& e : edges) {
            int i = e.first, j = e.second;
            float x1 = points[i].first, y1 = points[i].second;
            float x2 = points[j].first, y2 = points[j].second;
            float dist = std::hypot(x2 - x1, y2 - y1);
            g.addEdge(nodeIds[i], nodeIds[j], dist);
        }
        auto anchors = createAnchors(g, bounds);
        connectAnchors(g, anchors);
        attachToAnchors(g, anchors, bounds);
    }

}