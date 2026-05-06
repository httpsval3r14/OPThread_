#include "entities/graph.h"
#include <cmath>
#include <vector>
#include <execution> 
#include <algorithm>
#include <mutex>
#include <SFML/System.hpp> 

namespace entities {

    static bool getLineIntersection(float p0_x, float p0_y, float p1_x, float p1_y,
        float p2_x, float p2_y, float p3_x, float p3_y,
        float* outX, float* outY) {
        float s1_x = p1_x - p0_x; float s1_y = p1_y - p0_y;
        float s2_x = p3_x - p2_x; float s2_y = p3_y - p2_y;
        float denominator = (-s2_x * s1_y + s1_x * s2_y);

        if (std::abs(denominator) < 1e-5f) return false;

        float s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / denominator;
        float t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / denominator;

        if (s > 0.001f && s < 0.999f && t > 0.001f && t < 0.999f) {
            if (outX) *outX = p0_x + (t * s1_x);
            if (outY) *outY = p0_y + (t * s1_y);
            return true;
        }
        return false;
    }

    void entities::Graph::flattenGraph() {
        struct PendingIntersection {
            float x, y;
            uint64_t a1, b1, a2, b2;
        };

        sf::Clock safetyTimer;
        const float TIME_LIMIT = 4.0f;

        while (safetyTimer.getElapsedTime().asSeconds() < TIME_LIMIT) {
            std::vector<std::pair<uint64_t, uint64_t>> currentEdges;
            for (const auto& [id, node] : nodes) {
                for (uint64_t neighborId : node.getNeighbors()) {
                    if (id < neighborId) currentEdges.push_back({ id, neighborId });
                }
            }

            if (currentEdges.size() < 2) break;

            std::vector<PendingIntersection> foundIntersections;
            std::mutex listMutex;
            std::atomic<bool> anyFound{ false };

            std::for_each(std::execution::par, currentEdges.begin(), currentEdges.end(), [&](auto& edge1) {
                if (anyFound) return;

                for (const auto& edge2 : currentEdges) {
                    uint64_t a1 = edge1.first, b1 = edge1.second;
                    uint64_t a2 = edge2.first, b2 = edge2.second;

                    if (a1 == a2 || a1 == b2 || b1 == a2 || b1 == b2) continue;

                    float ix, iy;
                    if (getLineIntersection(nodes.at(a1).getX(), nodes.at(a1).getY(), nodes.at(b1).getX(), nodes.at(b1).getY(),
                        nodes.at(a2).getX(), nodes.at(a2).getY(), nodes.at(b2).getX(), nodes.at(b2).getY(),
                        &ix, &iy)) {

                        std::lock_guard<std::mutex> lock(listMutex);
                        foundIntersections.push_back({ ix, iy, a1, b1, a2, b2 });
                        anyFound = true; 
                        return;
                    }
                }
                });

            if (!foundIntersections.empty()) {
                auto& inter = foundIntersections[0];

                if (nodes.count(inter.a1) && nodes.count(inter.b1) &&
                    nodes.count(inter.a2) && nodes.count(inter.b2)) {

                    uint64_t nId = addNode(inter.x, inter.y);
                    deleteEdge(inter.a1, inter.b1);
                    deleteEdge(inter.a2, inter.b2);
                    addEdge(inter.a1, nId, 1.0f); addEdge(inter.b1, nId, 1.0f);
                    addEdge(inter.a2, nId, 1.0f); addEdge(inter.b2, nId, 1.0f);
                }
            }
            else {
                break; 
            }
        }
    }

    class DisjointSet {
    private:
        std::unordered_map<uint64_t, uint64_t> parent;
        std::unordered_map<uint64_t, int> rank;

    public:
        void makeSet(uint64_t v) {
            parent[v] = v;
            rank[v] = 0;
        }

        uint64_t find(uint64_t v) {
            if (parent[v] != v) {
                parent[v] = find(parent[v]);
            }
            return parent[v];
        }

        void unionSets(uint64_t a, uint64_t b) {
            uint64_t rootA = find(a);
            uint64_t rootB = find(b);

            if (rootA != rootB) {
                if (rank[rootA] < rank[rootB]) {
                    parent[rootA] = rootB;
                }
                else if (rank[rootA] > rank[rootB]) {
                    parent[rootB] = rootA;
                }
                else {
                    parent[rootB] = rootA;
                    rank[rootA]++;
                }
            }
        }
    };

    void Graph::applyKruskal() {
        struct EdgeData {
            uint64_t u, v;
            float weight;
            bool operator<(const EdgeData& other) const {
                return weight < other.weight;
            }
        };

        std::vector<EdgeData> allEdges;

        for (const auto& [u, node] : nodes) {
            for (uint64_t v : node.getNeighbors()) {
                if (u < v) { 
                    allEdges.push_back({ u, v, node.getWeightTo(v) });
                }
            }
        }

        std::sort(allEdges.begin(), allEdges.end());

        DisjointSet ds;
        for (const auto& [u, node] : nodes) {
            ds.makeSet(u);
        }

        for (auto& [u, node] : nodes) {
            node.clearEdges();
        }
        edgeCreationOrder.clear();

        for (const auto& edge : allEdges) {
            if (ds.find(edge.u) != ds.find(edge.v)) {
                ds.unionSets(edge.u, edge.v);
                addEdge(edge.u, edge.v, edge.weight);
            }
        }
    }
} 