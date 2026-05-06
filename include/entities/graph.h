#pragma once
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <algorithm>
#include "node.h"

namespace entities {
    class Graph {
    private:
        std::unordered_map<uint64_t, Node> nodes;
        std::vector<std::pair<uint64_t, uint64_t>> edgeCreationOrder;
        std::vector<uint64_t> nodeCreationOrder;

    public:
        Graph() = default;

        void flattenGraph();

        const std::unordered_map<uint64_t, Node>& getNodes() const { return nodes; }
        const std::vector<std::pair<uint64_t, uint64_t>>& getEdgeCreationOrder() const { return edgeCreationOrder; }
        const std::vector<uint64_t>& getNodeCreationOrder() const { return nodeCreationOrder; }

        uint64_t addNode(float x, float y) {
            Node newNode(x, y);
            uint64_t id = newNode.getID();
            auto [it, inserted] = nodes.try_emplace(id, std::move(newNode));
            if (inserted) nodeCreationOrder.push_back(id);
            return id;
        }

        void addEdge(uint64_t id_A, uint64_t id_B, float weight, int angle = 0) {
            if (id_A == id_B) return;
            auto itA = nodes.find(id_A);
            auto itB = nodes.find(id_B);
            if (itA == nodes.end() || itB == nodes.end()) return;

            if (itA->second.getNeighbors().count(id_B)) return;

            itA->second.addNeighbor(id_B, weight);
            itB->second.addNeighbor(id_A, weight);

            if (id_A < id_B)
                edgeCreationOrder.emplace_back(id_A, id_B);
            else
                edgeCreationOrder.emplace_back(id_B, id_A);
        }

        void deleteEdge(uint64_t id_A, uint64_t id_B) {
            auto itA = nodes.find(id_A);
            auto itB = nodes.find(id_B);
            if (itA != nodes.end() && itB != nodes.end()) {
                itA->second.removeEdge(id_B);
                itB->second.removeEdge(id_A);

                // limpiar historial de animacion
                auto pair = (id_A < id_B) ? std::make_pair(id_A, id_B) : std::make_pair(id_B, id_A);
                edgeCreationOrder.erase(std::remove(edgeCreationOrder.begin(), edgeCreationOrder.end(), pair), edgeCreationOrder.end());
            }
        }

        void deleteNode(uint64_t targetID) {
            auto targetIt = nodes.find(targetID);
            if (targetIt == nodes.end()) return;

            std::vector<uint64_t> neighborsToClean(targetIt->second.getNeighbors().begin(), targetIt->second.getNeighbors().end());
            for (uint64_t neighborID : neighborsToClean) {
                deleteEdge(targetID, neighborID);
            }

            nodeCreationOrder.erase(std::remove(nodeCreationOrder.begin(), nodeCreationOrder.end(), targetID), nodeCreationOrder.end());

            nodes.erase(targetIt);
        }

        void applyKruskal();
    };
}