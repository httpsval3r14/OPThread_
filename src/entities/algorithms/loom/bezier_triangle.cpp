#include "entities/algorithms/loom/bezier_triangle.h"
#include "entities/algorithms/loom/loom_utils.h"
#include "entities/algorithms/loom/loom_constants.h"
#include "entities/SeedManager.hpp"
#include <cmath>
#include <random>

namespace entities::algorithms::loom {

    static void drawBezierTriangle(Graph& g, float x1, float y1, float x2, float y2, float x3, float y3, int N) {
        auto lerp = [](float a, float b, float t) { return a + (b - a) * t; };
        for (int i = 0; i <= N; ++i) {
            float t = static_cast<float>(i) / N;
            float p12x = lerp(x1, x2, t), p12y = lerp(y1, y2, t);
            float p23x = lerp(x2, x3, t), p23y = lerp(y2, y3, t);
            float p31x = lerp(x3, x1, t), p31y = lerp(y3, y1, t);

            uint64_t id12 = g.addNode(p12x, p12y);
            uint64_t id23 = g.addNode(p23x, p23y);
            uint64_t id31 = g.addNode(p31x, p31y);

            if (id12 != id23) g.addEdge(id12, id23, std::hypot(p23x - p12x, p23y - p12y));
            if (id23 != id31) g.addEdge(id23, id31, std::hypot(p31x - p23x, p31y - p23y));
            if (id31 != id12) g.addEdge(id31, id12, std::hypot(p12x - p31x, p12y - p31y));
        }
    }

    void bezierTriangleLoomPattern(Graph& g, int depth) {
        const float minX = -LOOM_WIDTH / 2 + MARGIN;
        const float maxX = LOOM_WIDTH / 2 - MARGIN;
        const float minY = -LOOM_HEIGHT / 2 + MARGIN;
        const float maxY = LOOM_HEIGHT / 2 - MARGIN;
        sf::FloatRect bounds(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));

        auto anchors = createAnchors(g, bounds);
        connectAnchors(g, anchors);
        auto& rng = SeedManager::instance().rng();

        std::uniform_real_distribution<float> sideDist(50.0f, 160.0f);
        float cellSide = sideDist(rng);
        float cellH = cellSide * std::sqrt(3.0f) / 2.0f;
        std::uniform_real_distribution<float> offsetX(0.0f, cellSide);
        std::uniform_real_distribution<float> offsetY(0.0f, cellH);
        float offX = offsetX(rng);
        float offY = offsetY(rng);
        std::uniform_int_distribution<int> segDist(15, 35);
        int numCols = static_cast<int>((maxX - minX) / cellSide) + 2;
        int numRows = static_cast<int>((maxY - minY) / cellH) + 2;

        for (int col = -1; col < numCols; ++col) {
            float xBase = minX + col * cellSide + offX;
            for (int row = -1; row < numRows; ++row) {
                float yBase = minY + row * cellH + offY;
                drawBezierTriangle(g, xBase, yBase, xBase + cellSide, yBase, xBase + cellSide / 2.0f, yBase + cellH, segDist(rng));
                drawBezierTriangle(g, xBase, yBase + cellH, xBase + cellSide, yBase + cellH, xBase + cellSide / 2.0f, yBase, segDist(rng));
            }
        }
        attachToAnchors(g, anchors, bounds);
    }

} 