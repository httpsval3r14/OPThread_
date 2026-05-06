#pragma once
#include "entities/graph.h"

namespace entities::algorithms::fractal {
    void koch(Graph& g, int depth, float x1, float y1, float x2, float y2);
    void kochTriangle(Graph& g, int depth, float x1, float y1, float x2, float y2, float x3, float y3);
}