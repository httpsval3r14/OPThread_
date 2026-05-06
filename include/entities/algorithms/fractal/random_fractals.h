#pragma once
#include "entities/graph.h"

namespace entities::algorithms::fractal {
    void randomSierpinski(Graph& g, int& depth, int& outerLayers, bool& vertexMode);
}