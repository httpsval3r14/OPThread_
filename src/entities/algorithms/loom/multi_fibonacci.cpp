#include "entities/algorithms/loom/multi_fibonacci.h"
#include "entities/algorithms/loom/loom_utils.h"
#include "entities/algorithms/loom/loom_constants.h"
#include "entities/SeedManager.hpp"
#include <cmath>
#include <random>
#include <vector>

namespace entities::algorithms::loom {

    void multiFibonacciLoomPattern(Graph& g, int depth) {
        const float minX = -LOOM_WIDTH / 2 + MARGIN;
        const float maxX = LOOM_WIDTH / 2 - MARGIN;
        const float minY = -LOOM_HEIGHT / 2 + MARGIN;
        const float maxY = LOOM_HEIGHT / 2 - MARGIN;
        sf::FloatRect bounds(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));

        auto anchors = createAnchors(g, bounds);
        connectAnchors(g, anchors);

        const float usableWidth = (maxX - minX) * 0.8f;
        const float usableHeight = (maxY - minY) * 0.8f;
        auto& rng = SeedManager::instance().rng();

        std::uniform_int_distribution<int> numSeedsDist(5, 12);
        int numSeeds = numSeedsDist(rng);
        std::uniform_real_distribution<float> bDist(0.12f, 0.4f);
        std::uniform_real_distribution<float> rotDist(0.0f, 2.0f * 3.14159265f);
        std::uniform_real_distribution<float> xDist(-usableWidth / 2.0f, usableWidth / 2.0f);
        std::uniform_real_distribution<float> yDist(-usableHeight / 2.0f, usableHeight / 2.0f);

        std::vector<uint64_t> seedCenters;
        for (int s = 0; s < numSeeds; ++s) {
            float seedX = xDist(rng);
            float seedY = yDist(rng);
            float b = bDist(rng);
            float seedRotation = rotDist(rng);
            uint64_t seedCenterId = g.addNode(seedX, seedY);
            seedCenters.push_back(seedCenterId);

            float a = 15.0f, theta = 0.0f, r = 0.0f;
            uint64_t lastNodeId = 0;
            while (r < 1000.0f) {
                r = a * std::exp(b * theta);
                float nx = seedX + r * std::cos(theta + seedRotation);
                float ny = seedY + r * std::sin(theta + seedRotation);
                if (nx < minX - 100.0f || nx > maxX + 100.0f || ny < minY - 100.0f || ny > maxY + 100.0f) break;

                uint64_t currentNodeId = g.addNode(nx, ny);
                if (lastNodeId != 0) {
                    auto itLast = g.getNodes().find(lastNodeId);
                    if (itLast != g.getNodes().end()) {
                        g.addEdge(currentNodeId, lastNodeId, std::hypot(nx - itLast->second.getX(), ny - itLast->second.getY()));
                    }
                }
                g.addEdge(currentNodeId, seedCenterId, std::hypot(nx - seedX, ny - seedY));
                lastNodeId = currentNodeId;
                theta += 0.12f;
            }
        }

        for (size_t i = 0; i < seedCenters.size(); ++i) {
            for (size_t j = i + 1; j < seedCenters.size(); ++j) {
                auto itA = g.getNodes().find(seedCenters[i]);
                auto itB = g.getNodes().find(seedCenters[j]);
                if (itA != g.getNodes().end() && itB != g.getNodes().end()) {
                    float dist = std::hypot(itA->second.getX() - itB->second.getX(), itA->second.getY() - itB->second.getY());
                    if (dist < 400.0f) {
                        g.addEdge(seedCenters[i], seedCenters[j], dist);
                    }
                }
            }
        }
        attachToAnchors(g, anchors, bounds);
    }

}