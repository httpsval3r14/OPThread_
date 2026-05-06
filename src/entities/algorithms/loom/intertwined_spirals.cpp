#include "entities/algorithms/loom/intertwined_spirals.h"
#include "entities/algorithms/loom/loom_utils.h"
#include "entities/algorithms/loom/loom_constants.h"
#include "entities/SeedManager.hpp"
#include <cmath>
#include <random>
#include <vector>
#include <limits>

namespace entities::algorithms::loom {

    void intertwinedSpiralsLoomPattern(Graph& g, int depth) {
        const float minX = -LOOM_WIDTH / 2 + MARGIN;
        const float maxX = LOOM_WIDTH / 2 - MARGIN;
        const float minY = -LOOM_HEIGHT / 2 + MARGIN;
        const float maxY = LOOM_HEIGHT / 2 - MARGIN;
        sf::FloatRect bounds(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));

        auto& rng = SeedManager::instance().rng();
        int numSpirals = std::uniform_int_distribution<int>(5, 12)(rng);
        float maxRadius = std::min(bounds.size.x, bounds.size.y) * 0.45f;

        std::vector<std::vector<uint64_t>> spiralNodes(numSpirals);
        std::vector<sf::Vector2f> centers(numSpirals);

        std::uniform_real_distribution<float> centerDist(0.2f, 0.8f);

        for (int s = 0; s < numSpirals; ++s) {
            centers[s] = sf::Vector2f(
                bounds.position.x + bounds.size.x * centerDist(rng),
                bounds.position.y + bounds.size.y * centerDist(rng)
            );

            float a = 8.0f;
            float b = std::uniform_real_distribution<float>(0.12f, 0.28f)(rng);
            float theta = 0.0f;
            float deltaTheta = 0.12f;
            uint64_t lastId = 0;

            while (true) {
                float r = a * std::exp(b * theta);
                if (r > maxRadius) break;

                float x = centers[s].x + r * std::cos(theta + s * 2.094f);
                float y = centers[s].y + r * std::sin(theta + s * 2.094f);

                if (x < bounds.position.x - 20 || x > bounds.position.x + bounds.size.x + 20 ||
                    y < bounds.position.y - 20 || y > bounds.position.y + bounds.size.y + 20) break;

                uint64_t id = g.addNode(x, y);
                spiralNodes[s].push_back(id);

                if (lastId != 0) {
                    auto itLast = g.getNodes().find(lastId);
                    if (itLast != g.getNodes().end()) {
                        g.addEdge(lastId, id, std::hypot(x - itLast->second.getX(), y - itLast->second.getY()));
                    }
                }
                lastId = id;
                theta += deltaTheta;
            }
        }

        for (int s1 = 0; s1 < numSpirals; ++s1) {
            for (int s2 = s1 + 1; s2 < numSpirals; ++s2) {
                float minDist = std::numeric_limits<float>::max();
                size_t bestI = 0, bestJ = 0;

                for (size_t i = 0; i < spiralNodes[s1].size(); ++i) {
                    auto itI = g.getNodes().find(spiralNodes[s1][i]);
                    if (itI == g.getNodes().end()) continue;
                    float xi = itI->second.getX(), yi = itI->second.getY();

                    for (size_t j = 0; j < spiralNodes[s2].size(); ++j) {
                        auto itJ = g.getNodes().find(spiralNodes[s2][j]);
                        if (itJ == g.getNodes().end()) continue;

                        float dx = xi - itJ->second.getX();
                        float dy = yi - itJ->second.getY();
                        float d2 = dx * dx + dy * dy;

                        if (d2 < minDist) {
                            minDist = d2;
                            bestI = i; bestJ = j;
                        }
                    }
                }

                if (minDist < 400.0f) {
                    g.addEdge(spiralNodes[s1][bestI], spiralNodes[s2][bestJ], std::sqrt(minDist));
                }
            }
        }

      
        applyTensionRules(g);

        auto anchors = createAnchors(g, bounds);
        connectAnchors(g, anchors);
        attachToAnchors(g, anchors, bounds);
    }

} 