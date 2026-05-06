#include "entities/algorithms/fractal/NewtonLoomGenerator.h"
#include <cmath>
#include <queue>
#include <unordered_set>
#include <algorithm>

namespace entities::algorithms::fractal {

    NewtonLoomGenerator::NewtonLoomGenerator(const ConnectOptions& opts) : opts_(opts) {}

    void NewtonLoomGenerator::generate(Graph& graph, const NewtonField& field) const {
        const auto& cells = field.getCells();
        int res = field.getResolution();

        std::vector<std::vector<uint64_t>> ids(res, std::vector<uint64_t>(res));
        for (int y = 0; y < res; ++y) {
            for (int x = 0; x < res; ++x) {
                size_t idx = static_cast<size_t>(y) * res + x;
                const auto& cell = cells[idx];
                ids[y][x] = graph.addNode(cell.position.x, cell.position.y);
            }
        }

        connectGrid(graph, cells, ids, res);
        if (opts_.useDiagonal) connectDiagonals(graph, cells, ids, res);
        if (opts_.knn > 0) {
            std::vector<uint64_t> flatIds(res * res);
            for (int y = 0; y < res; ++y)
                for (int x = 0; x < res; ++x)
                    flatIds[static_cast<size_t>(y) * res + x] = ids[y][x];
            connectKNN(graph, cells, flatIds, opts_.knn);
        }
    }

    void NewtonLoomGenerator::connectGrid(Graph& graph, const std::vector<NewtonField::Cell>& cells,
        const std::vector<std::vector<uint64_t>>& ids, int res) const {
        for (int y = 0; y < res; ++y) {
            for (int x = 0; x < res; ++x) {
                size_t idx = static_cast<size_t>(y) * res + x;
                uint64_t idA = ids[y][x];
                int rootA = cells[idx].rootIndex;
                int iterA = cells[idx].iterations;

                if (x + 1 < res) {
                    size_t nbIdx = static_cast<size_t>(y) * res + (x + 1);
                    if (rootA == cells[nbIdx].rootIndex && std::abs(iterA - cells[nbIdx].iterations) <= 2) {
                        float dx = cells[idx].position.x - cells[nbIdx].position.x;
                        float dy = cells[idx].position.y - cells[nbIdx].position.y;
                        graph.addEdge(idA, ids[y][x + 1], std::hypot(dx, dy));
                    }
                }
                if (y + 1 < res) {
                    size_t nbIdx = static_cast<size_t>(y + 1) * res + x;
                    if (rootA == cells[nbIdx].rootIndex && std::abs(iterA - cells[nbIdx].iterations) <= 2) {
                        float dx = cells[idx].position.x - cells[nbIdx].position.x;
                        float dy = cells[idx].position.y - cells[nbIdx].position.y;
                        graph.addEdge(idA, ids[y + 1][x], std::hypot(dx, dy));
                    }
                }
            }
        }
    }

    void NewtonLoomGenerator::connectDiagonals(Graph& graph, const std::vector<NewtonField::Cell>& cells,
        const std::vector<std::vector<uint64_t>>& ids, int res) const {
        for (int y = 0; y < res; ++y) {
            for (int x = 0; x < res; ++x) {
                size_t idx = static_cast<size_t>(y) * res + x;
                uint64_t idA = ids[y][x];
                int iterA = cells[idx].iterations;
                if (iterA < 4) continue;

                if (x + 1 < res && y + 1 < res) {
                    size_t nbIdx = static_cast<size_t>(y + 1) * res + (x + 1);
                    if (std::abs(iterA - cells[nbIdx].iterations) <= 2) {
                        float dx = cells[idx].position.x - cells[nbIdx].position.x;
                        float dy = cells[idx].position.y - cells[nbIdx].position.y;
                        graph.addEdge(idA, ids[y + 1][x + 1], std::hypot(dx, dy));
                    }
                }
                if (x - 1 >= 0 && y + 1 < res) {
                    size_t nbIdx = static_cast<size_t>(y + 1) * res + (x - 1);
                    if (std::abs(iterA - cells[nbIdx].iterations) <= 2) {
                        float dx = cells[idx].position.x - cells[nbIdx].position.x;
                        float dy = cells[idx].position.y - cells[nbIdx].position.y;
                        graph.addEdge(idA, ids[y + 1][x - 1], std::hypot(dx, dy));
                    }
                }
            }
        }
    }

    void NewtonLoomGenerator::connectKNN(Graph& graph, const std::vector<NewtonField::Cell>& cells,
        const std::vector<uint64_t>& flatIds, int k) const {
        int n = static_cast<int>(cells.size());
        if (n == 0) return;

        for (int i = 0; i < n; ++i) {
            const auto& pos_i = cells[i].position;
            using DistPair = std::pair<float, int>;
            std::priority_queue<DistPair> maxHeap;

            for (int j = 0; j < n; ++j) {
                if (i == j) continue;
                float dx = pos_i.x - cells[j].position.x;
                float dy = pos_i.y - cells[j].position.y;
                float dist = std::hypot(dx, dy);
                if (dist > opts_.maxDistance) continue;

                if (static_cast<int>(maxHeap.size()) < k) {
                    maxHeap.emplace(dist, j);
                }
                else if (dist < maxHeap.top().first) {
                    maxHeap.pop();
                    maxHeap.emplace(dist, j);
                }
            }

            while (!maxHeap.empty()) {
                int j = maxHeap.top().second;
                maxHeap.pop();
                if (i < j) {
                    float dx = pos_i.x - cells[j].position.x;
                    float dy = pos_i.y - cells[j].position.y;
                    graph.addEdge(flatIds[i], flatIds[j], std::hypot(dx, dy));
                }
            }
        }
    }

} 