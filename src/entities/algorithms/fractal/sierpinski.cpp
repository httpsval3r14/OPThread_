#include "entities/algorithms/fractal/sierpinski.h"
#include <cmath>

namespace entities::algorithms::fractal {
    static void kochEdge(Graph& g, int depth, float x1, float y1, float x2, float y2) {
        if (depth == 0) {
            uint64_t id1 = g.addNode(x1, y1);
            uint64_t id2 = g.addNode(x2, y2);
            g.addEdge(id1, id2, std::hypot(x2 - x1, y2 - y1));
            return;
        }
        float dx = x2 - x1;
        float dy = y2 - y1;
        float p1x = x1 + dx / 3.0f;
        float p1y = y1 + dy / 3.0f;
        float p3x = x1 + 2.0f * dx / 3.0f;
        float p3y = y1 + 2.0f * dy / 3.0f;
        float vx = p3x - p1x;
        float vy = p3y - p1y;
        float out_px = p1x + vx * 0.5f - vy * 0.866025f;
        float out_py = p1y + vx * 0.866025f + vy * 0.5f;
        float in_px = p1x + vx * 0.5f + vy * 0.866025f;
        float in_py = p1y - vx * 0.866025f + vy * 0.5f;
        kochEdge(g, depth - 1, x1, y1, p1x, p1y);
        kochEdge(g, depth - 1, p1x, p1y, out_px, out_py);
        kochEdge(g, depth - 1, out_px, out_py, p3x, p3y);
        kochEdge(g, depth - 1, p1x, p1y, in_px, in_py);
        kochEdge(g, depth - 1, in_px, in_py, p3x, p3y);
        kochEdge(g, depth - 1, p3x, p3y, x2, y2);
    }

    void sierpinski(Graph& g, int depth, float x1, float y1, float x2, float y2, float x3, float y3, int kochDepth) {
        if (depth == 0) {
            kochEdge(g, kochDepth, x1, y1, x2, y2);
            kochEdge(g, kochDepth, x2, y2, x3, y3);
            kochEdge(g, kochDepth, x3, y3, x1, y1);
        }
        else {
            float m12_x = (x1 + x2) / 2.0f;
            float m12_y = (y1 + y2) / 2.0f;
            float m23_x = (x2 + x3) / 2.0f;
            float m23_y = (y2 + y3) / 2.0f;
            float m31_x = (x3 + x1) / 2.0f;
            float m31_y = (y1 + y3) / 2.0f;
            sierpinski(g, depth - 1, x1, y1, m12_x, m12_y, m31_x, m31_y, kochDepth);
            sierpinski(g, depth - 1, m12_x, m12_y, x2, y2, m23_x, m23_y, kochDepth);
            sierpinski(g, depth - 1, m31_x, m31_y, m23_x, m23_y, x3, y3, kochDepth);
        }
    }
}