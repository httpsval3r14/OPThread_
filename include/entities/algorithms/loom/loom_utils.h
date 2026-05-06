#pragma once
#include "entities/graph.h"
#include <SFML/Graphics/Rect.hpp>
#include <vector>

struct PerformanceData {
    int depth;
    double timeMs;      // Tiempo de ejecución
    size_t nodeCount;   // Complejidad Espacial (Nodos)
    size_t edgeCount;   // Complejidad Espacial (Aristas)
};

namespace entities::algorithms::loom {
    std::vector<uint64_t> createAnchors(Graph& g, const sf::FloatRect& bounds);
    void connectAnchors(Graph& g, const std::vector<uint64_t>& anchors);
    void attachToAnchors(Graph& g, const std::vector<uint64_t>& anchors, const sf::FloatRect& bounds);
    void applyTensionRules(Graph& g);
    void tessellateEmptySpace(Graph& g, float maxDist, int sparsityThreshold = 4);
}