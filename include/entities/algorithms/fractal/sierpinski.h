#pragma once
#include "entities/graph.h"

namespace entities::algorithms::fractal {
    // kochDepth con valor por defecto 1 (como pediste)
    void sierpinski(Graph& g, int depth, float x1, float y1, float x2, float y2, float x3, float y3, int kochDepth = 1);
}