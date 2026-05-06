#include "entities/algorithms/loom/chaos_blend.h"
#include "entities/algorithms/loom/loom_utils.h"
#include "entities/algorithms/loom/loom_constants.h"
#include "entities/algorithms/loom/tessellated.h"
#include "entities/algorithms/loom/fibonacci.h"
#include "entities/algorithms/loom/multi_fibonacci.h"
#include "entities/algorithms/loom/golden_tessellation.h"
#include "entities/algorithms/loom/bezier_triangle.h"
#include "entities/algorithms/loom/pythagoras_tree.h"
#include "entities/algorithms/loom/gosper_curve.h"
#include "entities/SeedManager.hpp"
#include <memory>
#include <functional>
#include <random>
#include <algorithm>
#include <unordered_map>

namespace entities::algorithms::loom {

    struct BSPNode {
        sf::FloatRect region;
        std::unique_ptr<BSPNode> left, right;
        int generatorIndex{ -1 };
        bool isLeaf() const { return !left && !right; }
    };

    static std::unique_ptr<BSPNode> buildBSP(const sf::FloatRect& bounds, int maxDepth, float minSize, int numGenerators, std::mt19937& rng) {
        auto node = std::make_unique<BSPNode>();
        node->region = bounds;
        if (maxDepth == 0 || bounds.size.x < minSize * 2 || bounds.size.y < minSize * 2) {
            std::uniform_int_distribution<int> pick(0, numGenerators - 1);
            node->generatorIndex = pick(rng);
            return node;
        }
        std::bernoulli_distribution coin(0.5);
        bool splitH = coin(rng);
        std::uniform_real_distribution<float> ratio(0.35f, 0.65f);
        float r = ratio(rng);
        sf::FloatRect leftRect, rightRect;
        if (splitH) {
            float cutY = bounds.position.y + bounds.size.y * r;
            leftRect = sf::FloatRect(bounds.position, sf::Vector2f(bounds.size.x, cutY - bounds.position.y));
            rightRect = sf::FloatRect(sf::Vector2f(bounds.position.x, cutY), sf::Vector2f(bounds.size.x, bounds.position.y + bounds.size.y - cutY));
        }
        else {
            float cutX = bounds.position.x + bounds.size.x * r;
            leftRect = sf::FloatRect(bounds.position, sf::Vector2f(cutX - bounds.position.x, bounds.size.y));
            rightRect = sf::FloatRect(sf::Vector2f(cutX, bounds.position.y), sf::Vector2f(bounds.position.x + bounds.size.x - cutX, bounds.size.y));
        }
        node->left = buildBSP(leftRect, maxDepth - 1, minSize, numGenerators, rng);
        node->right = buildBSP(rightRect, maxDepth - 1, minSize, numGenerators, rng);
        return node;
    }

    static void applyGenerators(const BSPNode* node, Graph& g, const std::vector<std::function<void(Graph&, sf::FloatRect)>>& generators) {
        if (!node) return;
        if (node->isLeaf()) {
            generators[node->generatorIndex](g, node->region);
            return;
        }
        applyGenerators(node->left.get(), g, generators);
        applyGenerators(node->right.get(), g, generators);
    }

    void chaosBlendLoomPattern(Graph& g, int depth) {
        const float minX = -LOOM_WIDTH / 2 + MARGIN;
        const float maxX = LOOM_WIDTH / 2 - MARGIN;
        const float minY = -LOOM_HEIGHT / 2 + MARGIN;
        const float maxY = LOOM_HEIGHT / 2 - MARGIN;
        sf::FloatRect bounds(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));

        auto anchors = createAnchors(g, bounds);
        connectAnchors(g, anchors);

        auto& rng = SeedManager::instance().rng();
        std::vector<int> allModes = { 2,3,4,5,6,7,9 };
        std::shuffle(allModes.begin(), allModes.end(), rng);
        int numGens = std::uniform_int_distribution<int>(3, std::min(4, (int)allModes.size()))(rng);
        allModes.resize(numGens);

        std::vector<std::function<void(Graph&, sf::FloatRect)>> generators;
        for (int mode : allModes) {
            generators.push_back([mode](Graph& gg, sf::FloatRect rect) {
                Graph local;
                switch (mode) {
                case 2: tessellatedLoomPattern(local, 2); break;
                case 3: fibonacciLoomPattern(local, 3); break;
                case 4: multiFibonacciLoomPattern(local, 3); break;
                case 5: goldenTessellationLoomPattern(local, 3); break;
                case 6: bezierTriangleLoomPattern(local, 3); break;
                case 7: pythagorasTreeLoomPattern(local, 3); break;
                case 9: gosperCurveLoomPattern(local, 3); break;
                default: break;
                }
                float scaleX = rect.size.x / (LOOM_WIDTH - 2 * MARGIN);
                float scaleY = rect.size.y / (LOOM_HEIGHT - 2 * MARGIN);
                float offsetX = rect.position.x + rect.size.x / 2;
                float offsetY = rect.position.y + rect.size.y / 2;

                std::unordered_map<uint64_t, uint64_t> idMap;
                for (const auto& [id, node] : local.getNodes()) {
                    float newX = offsetX + node.getX() * scaleX;
                    float newY = offsetY + node.getY() * scaleY;
                    uint64_t newId = gg.addNode(newX, newY);
                    idMap[id] = newId;
                }
                for (const auto& [id, node] : local.getNodes()) {
                    uint64_t newId = idMap[id];
                    for (uint64_t neigh : node.getNeighbors()) {
                        if (id < neigh) {
                            auto it = idMap.find(neigh);
                            if (it != idMap.end()) {
                                gg.addEdge(newId, it->second, node.getWeightTo(neigh));
                            }
                        }
                    }
                }
                });
        }

        auto tree = buildBSP(bounds, depth, 60.0f, numGens, rng);
        applyGenerators(tree.get(), g, generators);
        attachToAnchors(g, anchors, bounds);
    }

}