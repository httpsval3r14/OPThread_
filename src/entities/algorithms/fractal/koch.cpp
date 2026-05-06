#include "entities/algorithms/fractal/koch.h"
#include <cmath>

namespace entities::algorithms::fractal {
    static void kochRecursive(Graph& g, int depth, float x1, float y1, float x2, float y2) {
        if (depth == 0) {
            uint64_t id1 = g.addNode(x1, y1);
            uint64_t id2 = g.addNode(x2, y2);
            g.addEdge(id1, id2, std::hypot(x2 - x1, y2 - y1));
            return;
        }

        float dx = (x2 - x1) / 3.0f;
        float dy = (y2 - y1) / 3.0f;

        float p1x = x1 + dx;
        float p1y = y1 + dy;

        float p3x = x1 + 2.0f * dx;
        float p3y = y1 + 2.0f * dy;

        float angle = -60.0f * 3.14159265f / 180.0f;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);

        float p2x = p1x + (p3x - p1x) * cosA - (p3y - p1y) * sinA;
        float p2y = p1y + (p3x - p1x) * sinA + (p3y - p1y) * cosA;

        kochRecursive(g, depth - 1, x1, y1, p1x, p1y);
        kochRecursive(g, depth - 1, p1x, p1y, p2x, p2y);
        kochRecursive(g, depth - 1, p2x, p2y, p3x, p3y);
        kochRecursive(g, depth - 1, p3x, p3y, x2, y2);
    }

    void koch(Graph& g, int depth, float x1, float y1, float x2, float y2) {
        kochRecursive(g, depth, x1, y1, x2, y2);
    }

    void kochTriangle(Graph& g, int depth, float x1, float y1, float x2, float y2, float x3, float y3) {
        kochRecursive(g, depth, x1, y1, x2, y2);
        kochRecursive(g, depth, x2, y2, x3, y3);
        kochRecursive(g, depth, x3, y3, x1, y1);
    }
}