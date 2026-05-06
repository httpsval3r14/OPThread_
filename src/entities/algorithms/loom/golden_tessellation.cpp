#include "entities/algorithms/loom/golden_tessellation.h"
#include "entities/algorithms/loom/loom_utils.h"
#include "entities/algorithms/loom/loom_constants.h"
#include "entities/SeedManager.hpp"
#include <random>
#include <functional>

namespace entities::algorithms::loom {

    static void drawStringArtRect(Graph& g, float rx, float ry, float rw, float rh, int N) {
        for (int dir = 0; dir < 4; ++dir) {
            for (int i = 0; i <= N; ++i) {
                float t = static_cast<float>(i) / N;
                float invT = 1.0f - t;
                float x1, y1, x2, y2;
                if (dir == 0) { x1 = rx + t * rw; y1 = ry; x2 = rx; y2 = ry + invT * rh; }
                else if (dir == 1) { x1 = rx + rw - t * rw; y1 = ry; x2 = rx + rw; y2 = ry + invT * rh; }
                else if (dir == 2) { x1 = rx + rw - t * rw; y1 = ry + rh; x2 = rx + rw; y2 = ry + rh - invT * rh; }
                else { x1 = rx + t * rw; y1 = ry + rh; x2 = rx; y2 = ry + rh - invT * rh; }

                uint64_t id1 = g.addNode(x1, y1);
                uint64_t id2 = g.addNode(x2, y2);
                if (id1 != id2) g.addEdge(id1, id2, std::hypot(x2 - x1, y2 - y1));
            }
        }
    }

    static void randomRectSubdivision(Graph& g, float x, float y, float w, float h, int depth, std::mt19937& rng) {
        if (depth <= 0 || w < 20.0f || h < 20.0f) {
            std::uniform_int_distribution<int> densityDist(15, 40);
            drawStringArtRect(g, x, y, w, h, densityDist(rng));
            return;
        }
        std::uniform_int_distribution<int> axisDist(0, 1);
        int axis = axisDist(rng);
        std::uniform_real_distribution<float> ratioDist(0.3f, 0.7f);
        float ratio = ratioDist(rng);
        if (axis == 0) {
            float leftW = w * ratio;
            randomRectSubdivision(g, x, y, leftW, h, depth - 1, rng);
            randomRectSubdivision(g, x + leftW, y, w - leftW, h, depth - 1, rng);
        }
        else {
            float topH = h * ratio;
            randomRectSubdivision(g, x, y, w, topH, depth - 1, rng);
            randomRectSubdivision(g, x, y + topH, w, h - topH, depth - 1, rng);
        }
    }

    void goldenTessellationLoomPattern(Graph& g, int depth) {
        const float minX = -LOOM_WIDTH / 2 + MARGIN;
        const float maxX = LOOM_WIDTH / 2 - MARGIN;
        const float minY = -LOOM_HEIGHT / 2 + MARGIN;
        const float maxY = LOOM_HEIGHT / 2 - MARGIN;
        sf::FloatRect bounds(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));

        auto anchors = createAnchors(g, bounds);
        connectAnchors(g, anchors);
        auto& rng = SeedManager::instance().rng();
        randomRectSubdivision(g, bounds.position.x, bounds.position.y, bounds.size.x, bounds.size.y, depth, rng);
        attachToAnchors(g, anchors, bounds);
    }

}