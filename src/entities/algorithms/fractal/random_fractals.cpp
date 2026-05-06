#include "entities/algorithms/fractal/random_fractals.h"
#include "entities/algorithms/fractal/sierpinski.h"
#include "entities/algorithms/fractal/expand.h"
#include <random>
#include <cmath>

namespace entities::algorithms::fractal {
    void randomSierpinski(Graph& g, int& depth, int& outerLayers, bool& vertexMode) {
        thread_local std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> depthDist(2, 4);
        depth = depthDist(rng);
        std::uniform_int_distribution<int> modeDist(0, 1);
        vertexMode = (modeDist(rng) == 1);
        constexpr float canvasW = 1200.0f;
        constexpr float canvasH = 900.0f;
        constexpr float cellW = 300.0f;
        constexpr float cellH = 300.0f;
        int cols = static_cast<int>(canvasW / cellW) + 1;
        int rows = static_cast<int>(canvasH / cellH) + 1;
        std::uniform_real_distribution<float> jitter(-60.0f, 60.0f);
        std::uniform_real_distribution<float> radiusDist(30.0f, 120.0f);
        std::uniform_real_distribution<float> angleDist(0.0f, 6.28318f);
        std::uniform_int_distribution<int> outerDist(4, 8);
        int triangleCount = 0;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                float centerX = -canvasW / 2 + col * cellW + cellW / 2;
                float centerY = -canvasH / 2 + row * cellH + cellH / 2;
                float ox = jitter(rng);
                float oy = jitter(rng);
                float cx = centerX + ox;
                float cy = centerY + oy;
                float x1, y1, x2, y2, x3, y3;
                do {
                    auto randPt = [&](float baseX, float baseY) {
                        float a = angleDist(rng);
                        float r = radiusDist(rng);
                        return std::make_pair(baseX + r * cosf(a), baseY + r * sinf(a));
                        };
                    auto [px1, py1] = randPt(cx, cy);
                    auto [px2, py2] = randPt(cx, cy);
                    auto [px3, py3] = randPt(cx, cy);
                    x1 = px1; y1 = py1; x2 = px2; y2 = py2; x3 = px3; y3 = py3;
                    float area = 0.5f * std::abs(x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
                    if (area > 500.0f) break;
                } while (true);
                if (vertexMode) {
                    expandAtVertices(g, depth, x1, y1, x2, y2, x3, y3);
                }
                else {
                    int layers = outerDist(rng);
                    expandSierpinski(g, depth, layers, x1, y1, x2, y2, x3, y3);
                }
                ++triangleCount;
            }
        }
        outerLayers = triangleCount;
    }
}