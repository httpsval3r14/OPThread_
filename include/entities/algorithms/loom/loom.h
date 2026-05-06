#pragma once
#include "entities/graph.h"

namespace entities::algorithms::loom {
    void generateLoomPattern(Graph& g, int depth, bool mirror, int mode);
    void randomLoomPattern(Graph& g, int& depth, int& mode);

    // Declaraciones de todos los patrones
    void tessellatedLoomPattern(Graph& g, int depth);
    void fibonacciLoomPattern(Graph& g, int depth);
    void multiFibonacciLoomPattern(Graph& g, int depth);
    void goldenTessellationLoomPattern(Graph& g, int depth);
    void bezierTriangleLoomPattern(Graph& g, int depth);
    void pythagorasTreeLoomPattern(Graph& g, int depth);
    void gosperCurveLoomPattern(Graph& g, int depth);
    void voronoiLoomPattern(Graph& g, int depth);
    void chaosBlendLoomPattern(Graph& g, int depth);
    void intertwinedSpiralsLoomPattern(Graph& g, int depth);
    // Dentro del namespace entities::algorithms::loom, después de las otras declaraciones:
    void apollonianLoomPattern(Graph& g, int depth);   // <-- Nueva línea
}