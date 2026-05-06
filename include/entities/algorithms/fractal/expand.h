#pragma once
#include "entities/graph.h"

namespace entities::algorithms::fractal {
    void expandSierpinski(Graph& g, int depth, int outerLayers,
        float x1, float y1, float x2, float y2, float x3, float y3);
    void expandAtVertices(Graph& g, int depth,
        float x1, float y1, float x2, float y2, float x3, float y3,
        int outerDepth = -1);
}