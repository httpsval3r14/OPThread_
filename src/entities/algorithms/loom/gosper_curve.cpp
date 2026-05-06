#include "entities/algorithms/loom/gosper_curve.h"
#include "entities/algorithms/loom/loom_utils.h"
#include "entities/algorithms/loom/loom_constants.h"
#include "entities/SeedManager.hpp"
#include <cmath>
#include <random>
#include <memory>

namespace entities::algorithms::loom {

    static void drawGosperFlake(Graph& g, float cx, float cy, float radius, int depth, std::mt19937& rng, bool chaos, float rotation) {
        float hexRadius = radius;
        float apothem = hexRadius * 0.866025f;
        std::pair<float, float> centers[7] = {
            {cx, cy},
            {cx + apothem * 2, cy},
            {cx + apothem, cy + hexRadius * 1.5f},
            {cx - apothem, cy + hexRadius * 1.5f},
            {cx - apothem * 2, cy},
            {cx - apothem, cy - hexRadius * 1.5f},
            {cx + apothem, cy - hexRadius * 1.5f}
        };
        for (int i = 0; i < 7; ++i) {
            uint64_t idCenter = g.addNode(centers[i].first, centers[i].second);
            for (int side = 0; side < 6; ++side) {
                float angle = rotation + side * 3.14159265f / 3.0f;
                float x = centers[i].first + hexRadius * std::cos(angle);
                float y = centers[i].second + hexRadius * std::sin(angle);
                uint64_t idVertex = g.addNode(x, y);
                g.addEdge(idCenter, idVertex, hexRadius);
                if (side > 0) {
                    uint64_t prevVertex = g.addNode(centers[i].first + hexRadius * std::cos(angle - 3.14159265f / 3.0f),
                        centers[i].second + hexRadius * std::sin(angle - 3.14159265f / 3.0f));
                    g.addEdge(prevVertex, idVertex, hexRadius * 2.0f * std::sin(3.14159265f / 6.0f));
                }
            }
            if (depth > 1) {
                drawGosperFlake(g, centers[i].first, centers[i].second, hexRadius * 0.5f, depth - 1, rng, chaos, rotation + 0.3f);
            }
        }
    }

    void gosperCurveLoomPattern(Graph& g, int depth) {
        const float minX = -LOOM_WIDTH / 2 + MARGIN;
        const float maxX = LOOM_WIDTH / 2 - MARGIN;
        const float minY = -LOOM_HEIGHT / 2 + MARGIN;
        const float maxY = LOOM_HEIGHT / 2 - MARGIN;
        sf::FloatRect bounds(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));

        auto anchors = createAnchors(g, bounds);
        connectAnchors(g, anchors);

        auto& rng = SeedManager::instance().rng();
        bool chaos = (rng() % 2 == 0);
        float centerX = bounds.position.x + bounds.size.x / 2.0f;
        float centerY = bounds.position.y + bounds.size.y / 2.0f;
        float maxRadius = std::min(bounds.size.x, bounds.size.y) * 0.4f;

        drawGosperFlake(g, centerX, centerY, maxRadius, depth, rng, chaos, 0.0f);
        attachToAnchors(g, anchors, bounds);
    }

} 