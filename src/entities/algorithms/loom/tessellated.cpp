#include "entities/algorithms/loom/tessellated.h"
#include "entities/algorithms/loom/loom_utils.h"
#include "entities/algorithms/loom/loom_constants.h"
#include "entities/algorithms/fractal/sierpinski.h"
#include "entities/algorithms/fractal/expand.h"
#include "entities/SeedManager.hpp"
#include <cmath>
#include <random>
#include <vector>

namespace entities::algorithms::loom {

    struct TriCell { float x1, y1, x2, y2, x3, y3; };
    static bool triangleIntersectsBounds(const TriCell& tri, const sf::FloatRect& bounds) {
        auto inside = [&](float x, float y) {
            return x >= bounds.position.x && x <= bounds.position.x + bounds.size.x &&
                y >= bounds.position.y && y <= bounds.position.y + bounds.size.y;
            };
        return inside(tri.x1, tri.y1) || inside(tri.x2, tri.y2) || inside(tri.x3, tri.y3);
    }

    void tessellatedLoomPattern(Graph& g, int depth) {
        const float minX = -LOOM_WIDTH / 2 + MARGIN;
        const float maxX = LOOM_WIDTH / 2 - MARGIN;
        const float minY = -LOOM_HEIGHT / 2 + MARGIN;
        const float maxY = LOOM_HEIGHT / 2 - MARGIN;
        sf::FloatRect bounds(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));

        auto anchors = createAnchors(g, bounds);
        connectAnchors(g, anchors);

        auto& rng = SeedManager::instance().rng();
        std::uniform_real_distribution<float> sideDist(60.0f, 180.0f);
        float cellSide = sideDist(rng);
        float cellH = cellSide * std::sqrt(3.0f) / 2.0f;

        std::uniform_real_distribution<float> offsetX(0.0f, cellSide);
        std::uniform_real_distribution<float> offsetY(0.0f, cellH);
        float offX = offsetX(rng);
        float offY = offsetY(rng);

        std::uniform_int_distribution<int> patternDist(0, 3);
        std::uniform_int_distribution<int> depthDist(std::max(1, depth - 2), std::max(1, depth - 1));

        std::vector<TriCell> meshCells;
        auto addTri = [&](float ax, float ay, float bx, float by, float cx, float cy) {
            TriCell cell = { ax, ay, bx, by, cx, cy };
            if (triangleIntersectsBounds(cell, bounds)) meshCells.push_back(cell);
            };

        int numCols = static_cast<int>((maxX - minX) / cellSide) + 2;
        int numRows = static_cast<int>((maxY - minY) / cellH) + 2;

        for (int col = -1; col < numCols; ++col) {
            float xBase = minX + col * cellSide + offX;
            for (int row = -1; row < numRows; ++row) {
                float yBase = minY + row * cellH + offY;
                addTri(xBase, yBase, xBase + cellSide, yBase, xBase + cellSide / 2.0f, yBase + cellH);
                addTri(xBase, yBase + cellH, xBase + cellSide, yBase + cellH, xBase + cellSide / 2.0f, yBase);
            }
        }

        for (const auto& cell : meshCells) {
            int pattern = patternDist(rng);
            int localDepth = std::max(1, depthDist(rng));
            uint64_t id1 = g.addNode(cell.x1, cell.y1);
            uint64_t id2 = g.addNode(cell.x2, cell.y2);
            uint64_t id3 = g.addNode(cell.x3, cell.y3);
            g.addEdge(id1, id2, std::hypot(cell.x2 - cell.x1, cell.y2 - cell.y1));
            g.addEdge(id2, id3, std::hypot(cell.x3 - cell.x2, cell.y3 - cell.y2));
            g.addEdge(id3, id1, std::hypot(cell.x1 - cell.x3, cell.y1 - cell.y3));

            if (pattern == 1) entities::algorithms::fractal::sierpinski(g, localDepth, cell.x1, cell.y1, cell.x2, cell.y2, cell.x3, cell.y3, 0);
            else if (pattern == 2) entities::algorithms::fractal::expandSierpinski(g, localDepth, 1, cell.x1, cell.y1, cell.x2, cell.y2, cell.x3, cell.y3);
            else if (pattern == 3) entities::algorithms::fractal::sierpinski(g, localDepth, cell.x1, cell.y1, cell.x2, cell.y2, cell.x3, cell.y3, 1);
        }
        attachToAnchors(g, anchors, bounds);
    }

} 