#include "entities/algorithms/loom/peano_curve.h"
#include "entities/algorithms/loom/loom_utils.h"
#include "entities/algorithms/loom/loom_constants.h"
#include "entities/SeedManager.hpp"
#include <cmath>
#include <random>
#include <vector>

namespace entities::algorithms::loom {

    static void drawThickLine(Graph& g, float x1, float y1, float x2, float y2, float thickness, std::mt19937& rng) {
        float dist = std::hypot(x2 - x1, y2 - y1);
        if (dist < 0.1f) return;
        float angle = std::atan2(y2 - y1, x2 - x1);
        float perp = angle + 3.14159265f / 2.0f;
        float offset = thickness / 2.0f;
        float x1a = x1 + offset * std::cos(perp);
        float y1a = y1 + offset * std::sin(perp);
        float x1b = x1 - offset * std::cos(perp);
        float y1b = y1 - offset * std::sin(perp);
        float x2a = x2 + offset * std::cos(perp);
        float y2a = y2 + offset * std::sin(perp);
        float x2b = x2 - offset * std::cos(perp);
        float y2b = y2 - offset * std::sin(perp);
        uint64_t id1a = g.addNode(x1a, y1a);
        uint64_t id1b = g.addNode(x1b, y1b);
        uint64_t id2a = g.addNode(x2a, y2a);
        uint64_t id2b = g.addNode(x2b, y2b);
        g.addEdge(id1a, id1b, thickness);
        g.addEdge(id2a, id2b, thickness);
        g.addEdge(id1a, id2a, std::hypot(x2a - x1a, y2a - y1a));
        g.addEdge(id1b, id2b, std::hypot(x2b - x1b, y2b - y1b));
        g.addEdge(id1a, id2b, std::hypot(x2b - x1a, y2b - y1a));
        g.addEdge(id1b, id2a, std::hypot(x2a - x1b, y2a - y1b));
    }

    static void peanoThickRecursive(Graph& g, float x, float y, float w, float h, int depth,
        uint64_t& lastId, std::mt19937& rng, float thickness, bool jitter) {
        if (depth <= 0) {
            float stepX = w / 3.0f;
            float stepY = h / 3.0f;
            std::vector<std::pair<float, float>> points;
            for (int i = 0; i < 3; ++i) {
                float px = x + i * stepX + stepX / 2.0f;
                if (i % 2 == 0) {
                    for (int j = 0; j < 3; ++j) points.emplace_back(px, y + j * stepY + stepY / 2.0f);
                }
                else {
                    for (int j = 2; j >= 0; --j) points.emplace_back(px, y + j * stepY + stepY / 2.0f);
                }
            }
            uint64_t prev = lastId;
            for (auto [px, py] : points) {
                if (jitter) {
                    std::uniform_real_distribution<float> jit(-thickness * 0.5f, thickness * 0.5f);
                    px += jit(rng); py += jit(rng);
                }
                uint64_t curr = g.addNode(px, py);
                if (prev != 0 && prev != curr) {
                    drawThickLine(g, g.getNodes().at(prev).getX(), g.getNodes().at(prev).getY(), px, py, thickness, rng);
                }
                prev = curr;
            }
            lastId = prev;
        }
        else {
            float subW = w / 3.0f, subH = h / 3.0f;
            const int order[9][2] = { {0,0},{1,0},{2,0},{2,1},{1,1},{0,1},{0,2},{1,2},{2,2} };
            for (int k = 0; k < 9; ++k) {
                peanoThickRecursive(g, x + order[k][0] * subW, y + order[k][1] * subH,
                    subW, subH, depth - 1, lastId, rng, thickness, jitter);
            }
        }
    }

    void peanoCurveLoomPattern(Graph& g, int depth) {
        const float minX = -LOOM_WIDTH / 2 + MARGIN;
        const float maxX = LOOM_WIDTH / 2 - MARGIN;
        const float minY = -LOOM_HEIGHT / 2 + MARGIN;
        const float maxY = LOOM_HEIGHT / 2 - MARGIN;
        sf::FloatRect bounds(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));

        auto anchors = createAnchors(g, bounds);
        connectAnchors(g, anchors);

        auto& rng = SeedManager::instance().rng();
        float thickness = std::uniform_real_distribution<float>(4.0f, 12.0f)(rng);
        bool jitter = (rng() % 2 == 0);
        float size = std::min(bounds.size.x, bounds.size.y) * 0.85f;
        float startX = bounds.position.x + (bounds.size.x - size) / 2.0f;
        float startY = bounds.position.y + (bounds.size.y - size) / 2.0f;

        uint64_t lastId = 0;
        peanoThickRecursive(g, startX, startY, size, size, depth + 1, lastId, rng, thickness, jitter);

        if (lastId != 0 && !anchors.empty()) {
            auto itLast = g.getNodes().find(lastId);
            if (itLast != g.getNodes().end()) {
                float closeDist = std::numeric_limits<float>::max();
                uint64_t closestAnchor = 0;
                for (uint64_t aid : anchors) {
                    auto itA = g.getNodes().find(aid);
                    if (itA == g.getNodes().end()) continue;
                    float dx = itLast->second.getX() - itA->second.getX();
                    float dy = itLast->second.getY() - itA->second.getY();
                    float d2 = dx * dx + dy * dy;
                    if (d2 < closeDist) { closeDist = d2; closestAnchor = aid; }
                }
                if (closestAnchor != 0) {
                    auto itA = g.getNodes().find(closestAnchor);
                    if (itA != g.getNodes().end()) {
                        drawThickLine(g, itLast->second.getX(), itLast->second.getY(),
                            itA->second.getX(), itA->second.getY(), thickness, rng);
                    }
                }
            }
        }

        attachToAnchors(g, anchors, bounds);
    }
}