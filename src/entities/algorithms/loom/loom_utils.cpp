#include "entities/algorithms/loom/loom_utils.h"
#include "entities/algorithms/loom/loom_constants.h" 
#include <cmath>
#include <limits>
#include <algorithm>
#include <unordered_map>

namespace entities::algorithms::loom {

    std::vector<uint64_t> createAnchors(Graph& g, const sf::FloatRect& bounds) {
        std::vector<uint64_t> anchors;

        for (float x = bounds.position.x; x <= bounds.position.x + bounds.size.x; x += ANCHOR_SPACING) {
            anchors.push_back(g.addNode(x, bounds.position.y));
            anchors.push_back(g.addNode(x, bounds.position.y + bounds.size.y));
        }

        for (float y = bounds.position.y + ANCHOR_SPACING; y < bounds.position.y + bounds.size.y; y += ANCHOR_SPACING) {
            anchors.push_back(g.addNode(bounds.position.x, y));
            anchors.push_back(g.addNode(bounds.position.x + bounds.size.x, y));
        }

        return anchors;
    }

    void connectAnchors(Graph& g, const std::vector<uint64_t>& anchors) {
        float maxDist = ANCHOR_SPACING * 1.5f;

        for (size_t i = 0; i < anchors.size(); ++i) {
            for (size_t j = i + 1; j < anchors.size(); ++j) {
                auto itA = g.getNodes().find(anchors[i]);
                auto itB = g.getNodes().find(anchors[j]);

                if (itA != g.getNodes().end() && itB != g.getNodes().end()) {
                    float dx = itA->second.getX() - itB->second.getX();
                    float dy = itA->second.getY() - itB->second.getY();
                    float dist = std::sqrt(dx * dx + dy * dy);

                    if (dist > 0.0f && dist <= maxDist) {
                        g.addEdge(anchors[i], anchors[j], dist);
                    }
                }
            }
        }
    }

    void applyTensionRules(Graph& g) {
        // luego
    }

    void tessellateEmptySpace(Graph& g, float maxDist, int sparsityThreshold) {
        auto& nodes = g.getNodes();
        float maxDist2 = maxDist * maxDist;
        float minDist2 = 10.0f * 10.0f;

        std::unordered_map<uint64_t, int> addedConnections;
        std::vector<std::pair<uint64_t, uint64_t>> newTessellationEdges;

        for (auto itA = nodes.begin(); itA != nodes.end(); ++itA) {
            uint64_t idA = itA->first;

            auto itB = itA;
            for (++itB; itB != nodes.end(); ++itB) {
                uint64_t idB = itB->first;

                int currentConnsA = itA->second.getNeighbors().size() + addedConnections[idA];
                int currentConnsB = itB->second.getNeighbors().size() + addedConnections[idB];

                if (currentConnsA >= sparsityThreshold) break;
                if (currentConnsB >= sparsityThreshold) continue;
                if (itA->second.getNeighbors().contains(idB)) continue;

                float dx = itA->second.getX() - itB->second.getX();
                float dy = itA->second.getY() - itB->second.getY();
                float d2 = dx * dx + dy * dy;

                if (d2 > minDist2 && d2 <= maxDist2) {
                    newTessellationEdges.push_back({ idA, idB });
                    addedConnections[idA]++;
                    addedConnections[idB]++;
                }
            }
        }

        for (const auto& edge : newTessellationEdges) {
            auto it1 = nodes.find(edge.first);
            auto it2 = nodes.find(edge.second);
            if (it1 != nodes.end() && it2 != nodes.end()) {
                float dx = it1->second.getX() - it2->second.getX();
                float dy = it1->second.getY() - it2->second.getY();
                g.addEdge(edge.first, edge.second, std::sqrt(dx * dx + dy * dy));
            }
        }
    }

    void attachToAnchors(Graph& g, const std::vector<uint64_t>& anchors, const sf::FloatRect& bounds) {
        auto& nodes = g.getNodes();
        float maxAttachDist = ANCHOR_SPACING * 2.0f;

        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            uint64_t id = it->first;

            if (std::find(anchors.begin(), anchors.end(), id) != anchors.end()) continue;
            if (it->second.getNeighbors().size() > 3) continue;

            float x = it->second.getX();
            float y = it->second.getY();

            if (x < bounds.position.x + maxAttachDist || x > bounds.position.x + bounds.size.x - maxAttachDist ||
                y < bounds.position.y + maxAttachDist || y > bounds.position.y + bounds.size.y - maxAttachDist) {

                float minDist = std::numeric_limits<float>::max();
                uint64_t bestAnchor = 0;

                for (uint64_t anchorId : anchors) {
                    auto aIt = nodes.find(anchorId);
                    if (aIt != nodes.end()) {
                        float dx = x - aIt->second.getX();
                        float dy = y - aIt->second.getY();
                        float dist = std::sqrt(dx * dx + dy * dy);

                        if (dist < minDist) {
                            minDist = dist;
                            bestAnchor = anchorId;
                        }
                    }
                }

                if (bestAnchor != 0 && minDist < maxAttachDist) {
                    g.addEdge(id, bestAnchor, minDist);
                }
            }
        }

        for (uint64_t anchorId : anchors) {
            auto aIt = nodes.find(anchorId);
            if (aIt == nodes.end()) continue;

            float ax = aIt->second.getX();
            float ay = aIt->second.getY();
            float minDist = std::numeric_limits<float>::max();
            uint64_t bestFabricNode = 0;

            for (auto it = nodes.begin(); it != nodes.end(); ++it) {
                uint64_t id = it->first;
                if (std::find(anchors.begin(), anchors.end(), id) != anchors.end()) continue;

                float dx = ax - it->second.getX();
                float dy = ay - it->second.getY();
                float dist = std::sqrt(dx * dx + dy * dy);

                if (dist < minDist) {
                    minDist = dist;
                    bestFabricNode = id;
                }
            }

            if (bestFabricNode != 0) {
                g.addEdge(anchorId, bestFabricNode, minDist);
            }
        }
    }

} 