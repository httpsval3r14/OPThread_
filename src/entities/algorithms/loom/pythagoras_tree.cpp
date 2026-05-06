#include "entities/algorithms/loom/pythagoras_tree.h"
#include "entities/algorithms/loom/loom_utils.h"
#include "entities/algorithms/loom/loom_constants.h"
#include "entities/SeedManager.hpp"
#include <cmath>
#include <random>
#include <vector>

namespace entities::algorithms::loom {

    static void pythagorasLaceBranch(Graph& g, float x, float y, float size, float angle, int depth, std::vector<uint64_t>& fabricNodes) {
        if (depth <= 0 || size < 4.0f) {
            return;
        }

        float x2 = x + size * std::cos(angle);
        float y2 = y + size * std::sin(angle);

        uint64_t id1 = g.addNode(x, y);
        uint64_t id2 = g.addNode(x2, y2);
        g.addEdge(id1, id2, size);

        fabricNodes.push_back(id1);
        fabricNodes.push_back(id2);

        float newSize = size * 0.72f;
        float angleSpread = 0.55f;

        pythagorasLaceBranch(g, x2, y2, newSize, angle - angleSpread, depth - 1, fabricNodes);
        pythagorasLaceBranch(g, x2, y2, newSize, angle + angleSpread, depth - 1, fabricNodes);
    }

    void pythagorasTreeLoomPattern(Graph& g, int depth) {
        const float minX = -LOOM_WIDTH / 2 + MARGIN;
        const float maxX = LOOM_WIDTH / 2 - MARGIN;
        const float minY = -LOOM_HEIGHT / 2 + MARGIN;
        const float maxY = LOOM_HEIGHT / 2 - MARGIN;
        sf::FloatRect bounds(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));

        std::vector<uint64_t> fabricNodes;

        int cols = 6;
        int rows = 4;
        float stepX = bounds.size.x / cols;
        float stepY = bounds.size.y / rows;

        int localDepth = depth + 3;
        float baseSize = bounds.size.y * 0.10f;

        for (int r = 0; r <= rows; ++r) {
            for (int c = 0; c <= cols; ++c) {
                float x = bounds.position.x + c * stepX;
                float y = bounds.position.y + r * stepY;

                float initialAngle = ((r + c) % 2 == 0) ? -1.5708f : 1.5708f;

                pythagorasLaceBranch(g, x, y, baseSize, initialAngle, localDepth, fabricNodes);
                pythagorasLaceBranch(g, x, y, baseSize, initialAngle + 3.14159f, localDepth, fabricNodes);
            }
        }

        float maxDist2 = 25.0f * 25.0f;
        for (size_t i = 0; i < fabricNodes.size(); i += 2) {
            auto itA = g.getNodes().find(fabricNodes[i]);
            if (itA == g.getNodes().end()) continue;

            int connections = 0;
            for (size_t j = i + 1; j < fabricNodes.size() && connections < 3; ++j) {
                auto itB = g.getNodes().find(fabricNodes[j]);
                if (itB == g.getNodes().end()) continue;

                float dx = itA->second.getX() - itB->second.getX();
                float dy = itA->second.getY() - itB->second.getY();
                float dist2 = dx * dx + dy * dy;

                if (dist2 > 5.0f && dist2 < maxDist2) {
                    g.addEdge(fabricNodes[i], fabricNodes[j], std::sqrt(dist2));
                    connections++;
                }
            }
        }

        tessellateEmptySpace(g, ANCHOR_SPACING * 1.5f, 5);
        applyTensionRules(g);

        auto anchors = createAnchors(g, bounds);
        connectAnchors(g, anchors);
        attachToAnchors(g, anchors, bounds);
    }

} 