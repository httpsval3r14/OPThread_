#pragma once
#include <vector>
#include <cstdint>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

namespace entities {
    class Node {
    private:
        uint64_t id;
        float x, y;
        std::unordered_set<uint64_t> neighbors;
        std::unordered_map<uint64_t, float> weights;

    public:
        Node(float x, float y);
        Node() = default;

        uint64_t getID() const { return id; }
        float getX() const { return x; }
        float getY() const { return y; }

        const std::unordered_set<uint64_t>& getNeighbors() const { return neighbors; }
        float getWeightTo(uint64_t neighbor) const {
            auto it = weights.find(neighbor);
            return (it != weights.end()) ? it->second : 0.0f;
        }

        void addNeighbor(uint64_t neighbor, float weight) {
            neighbors.insert(neighbor);
            weights[neighbor] = weight;
        }

        void removeEdge(uint64_t targetDestinationID);

        void clearEdges() {
            neighbors.clear();
            weights.clear();
        };
    };
}