#include "entities/algorithms/fractal/expand.h"
#include "entities/algorithms/fractal/sierpinski.h"
#include <thread>
#include <mutex>
#include <vector>

namespace entities::algorithms::fractal {
    void expandSierpinski(Graph& g, int depth, int outerLayers,
        float x1, float y1, float x2, float y2, float x3, float y3) {
        sierpinski(g, depth, x1, y1, x2, y2, x3, y3);
        float ax = x1, ay = y1, bx = x2, by = y2, cx = x3, cy = y3;
        for (int layer = 1; layer <= outerLayers; ++layer) {
            float axNew = bx + cx - ax;
            float ayNew = by + cy - ay;
            float bxNew = cx + ax - bx;
            float byNew = cy + ay - by;
            float cxNew = ax + bx - cx;
            float cyNew = ay + by - cy;
            sierpinski(g, depth, axNew, ayNew, bx, by, cx, cy);
            sierpinski(g, depth, ax, ay, bxNew, byNew, cx, cy);
            sierpinski(g, depth, ax, ay, bx, by, cxNew, cyNew);
            ax = axNew; ay = ayNew;
            bx = bxNew; by = byNew;
            cx = cxNew; cy = cyNew;
        }
    }

    void expandAtVertices(Graph& g, int depth,
        float x1, float y1, float x2, float y2, float x3, float y3,
        int outerDepth) {
        if (outerDepth < 0) outerDepth = depth;
        sierpinski(g, depth, x1, y1, x2, y2, x3, y3);
        struct Tri { float ax, ay, bx, by, cx, cy; };
        Tri tA = { x1, y1, 2 * x1 - x2, 2 * y1 - y2, 2 * x1 - x3, 2 * y1 - y3 };
        Tri tB = { x2, y2, 2 * x2 - x3, 2 * y2 - y3, 2 * x2 - x1, 2 * y2 - y1 };
        Tri tC = { x3, y3, 2 * x3 - x1, 2 * y3 - y1, 2 * x3 - x2, 2 * y3 - y2 };
        std::mutex mergeMutex;
        auto threadWork = [&](const Tri& tri) {
            Graph local;
            sierpinski(local, outerDepth, tri.ax, tri.ay, tri.bx, tri.by, tri.cx, tri.cy);
            std::lock_guard<std::mutex> lock(mergeMutex);
            for (const auto& [id, node] : local.getNodes()) {
                uint64_t newId = g.addNode(node.getX(), node.getY());
                for (uint64_t neighborId : node.getNeighbors()) {
                    auto itNeighbor = local.getNodes().find(neighborId);
                    if (itNeighbor != local.getNodes().end()) {
                        uint64_t globalNeighborId = g.addNode(itNeighbor->second.getX(), itNeighbor->second.getY());
                        float weight = node.getWeightTo(neighborId);
                        g.addEdge(newId, globalNeighborId, weight);
                    }
                }
            }
            };
        std::thread thA(threadWork, tA);
        std::thread thB(threadWork, tB);
        std::thread thC(threadWork, tC);
        thA.join(); thB.join(); thC.join();
    }
}