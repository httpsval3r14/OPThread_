#include "entities/algorithms/loom/fibonacci.h"
#include "entities/algorithms/loom/loom_utils.h"
#include "entities/algorithms/loom/loom_constants.h"
#include "entities/SeedManager.hpp"
#include <cmath>
#include <random>
#include <vector>

namespace entities::algorithms::loom {

    void fibonacciLoomPattern(Graph& g, int depth) {
        const float minX = -LOOM_WIDTH / 2 + MARGIN;
        const float maxX = LOOM_WIDTH / 2 - MARGIN;
        const float minY = -LOOM_HEIGHT / 2 + MARGIN;
        const float maxY = LOOM_HEIGHT / 2 - MARGIN;
        const float centerX = 0.0f, centerY = 0.0f;

        float maxRadius = std::min({ std::abs(minX), std::abs(maxX), std::abs(minY), std::abs(maxY) });
        maxRadius = std::sqrt(maxRadius * maxRadius + maxRadius * maxRadius) * 0.9f;

        auto& rng = SeedManager::instance().rng();
        std::uniform_int_distribution<int> armsDist(5, 12);
        int numArms = armsDist(rng);
        std::uniform_real_distribution<float> bDist(0.08f, 0.35f);
        float b = bDist(rng);
        std::uniform_real_distribution<float> rotDist(0.0f, 2.0f * 3.14159265f);
        float initialRotation = rotDist(rng);

        float thetaMax = 8.0f * depth;
        float a = maxRadius / std::exp(b * thetaMax);
        float deltaTheta = 0.08f / static_cast<float>(std::max(1, depth));
        int maxPoints = static_cast<int>(thetaMax / deltaTheta) + 1;

        uint64_t centerId = g.addNode(centerX, centerY);
        std::vector<std::vector<uint64_t>> spiralNodes(numArms);
        for (auto& vec : spiralNodes) vec.reserve(maxPoints);

        for (int i = 0; i < maxPoints; ++i) {
            float theta = i * deltaTheta;
            float r = a * std::exp(b * theta);
            if (r > maxRadius) break;

            for (int k = 0; k < numArms; ++k) {
                float angle = theta + initialRotation + k * 2.0f * 3.14159265f / numArms;
                float x = r * std::cos(angle);
                float y = r * std::sin(angle);
                uint64_t nodeId = g.addNode(x, y);
                spiralNodes[k].push_back(nodeId);
                g.addEdge(nodeId, centerId, std::hypot(x - centerX, y - centerY));
            }
        }

        for (int k = 0; k < numArms; ++k) {
            auto& nodes = spiralNodes[k];
            for (size_t i = 1; i < nodes.size(); ++i) {
                auto itA = g.getNodes().find(nodes[i - 1]);
                auto itB = g.getNodes().find(nodes[i]);
                if (itA != g.getNodes().end() && itB != g.getNodes().end()) {
                    g.addEdge(nodes[i - 1], nodes[i], std::hypot(itA->second.getX() - itB->second.getX(), itA->second.getY() - itB->second.getY()));
                }
            }
            int nextK = (k + 1) % numArms;
            size_t minSize = std::min(nodes.size(), spiralNodes[nextK].size());
            for (size_t i = 0; i < minSize; i += 2) {
                auto itA = g.getNodes().find(nodes[i]);
                auto itB = g.getNodes().find(spiralNodes[nextK][i]);
                if (itA != g.getNodes().end() && itB != g.getNodes().end()) {
                    g.addEdge(nodes[i], spiralNodes[nextK][i], std::hypot(itA->second.getX() - itB->second.getX(), itA->second.getY() - itB->second.getY()));
                }
            }
        }

        float phaseShift = 3.14159265f / numArms;
        for (int k = 0; k < numArms; ++k) {
            for (size_t i = 0; i < spiralNodes[k].size(); i += 2) {
                float angleAdd = phaseShift * (k % 2 == 0 ? 1 : -1);
                float r = maxRadius * (static_cast<float>(i) / spiralNodes[k].size());
                float x = r * std::cos(angleAdd), y = r * std::sin(angleAdd);
                uint64_t extraNode = g.addNode(x, y);
                g.addEdge(spiralNodes[k][i], extraNode, r * 0.5f);
            }
        }

        sf::FloatRect fullBounds(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));
        auto anchors = createAnchors(g, fullBounds);
        connectAnchors(g, anchors);
        attachToAnchors(g, anchors, fullBounds);
    }

} 