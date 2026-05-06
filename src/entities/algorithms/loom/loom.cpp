#include "entities/algorithms/loom/loom.h"
#include "entities/algorithms/loom/loom_constants.h"
#include "entities/algorithms/loom/tessellated.h"
#include "entities/algorithms/loom/fibonacci.h"
#include "entities/algorithms/loom/multi_fibonacci.h"
#include "entities/algorithms/loom/golden_tessellation.h"
#include "entities/algorithms/loom/bezier_triangle.h"
#include "entities/algorithms/loom/pythagoras_tree.h"
#include "entities/algorithms/loom/gosper_curve.h"
#include "entities/algorithms/loom/voronoi.h"
#include "entities/algorithms/loom/chaos_blend.h"
#include "entities/algorithms/loom/intertwined_spirals.h"
#include "entities/algorithms/fractal/newton_fractal.h"
#include "entities/algorithms/loom/apollonian_gasket.h"  

#include "entities/SeedManager.hpp"
#include <random>
#include <vector>

namespace entities::algorithms::loom {
    void generateLoomPattern(Graph& g, int depth, bool mirror, int mode) {
        switch (mode) {
        case 2: tessellatedLoomPattern(g, depth); break;
        case 3: fibonacciLoomPattern(g, depth); break;
        case 4: multiFibonacciLoomPattern(g, depth); break;
        case 5: goldenTessellationLoomPattern(g, depth); break;
        case 6: bezierTriangleLoomPattern(g, depth); break;
        case 7: pythagorasTreeLoomPattern(g, depth); break;
        case 8: // Peano eliminado - redirigir a Gosper
            gosperCurveLoomPattern(g, depth);
            break;
        case 9: gosperCurveLoomPattern(g, depth); break;
        case 10: voronoiLoomPattern(g, depth); break;
        case 11: chaosBlendLoomPattern(g, depth); break;
        case 12: intertwinedSpiralsLoomPattern(g, depth); break;
        case 13: entities::algorithms::fractal::NewtonLoomPattern::generate(g, depth); break; 
        case 14:
            apollonianLoomPattern(g, depth);
            break;
        default: tessellatedLoomPattern(g, depth); break;
        }
    }

    void randomLoomPattern(Graph& g, int& depth, int& mode) {
        auto& rng = SeedManager::instance().rng();
        std::uniform_int_distribution<int> depthDist(3, 5);
        depth = depthDist(rng);

        if (mode < 0 || mode > 13) {
            std::vector<int> available = { 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13,14 };
            std::uniform_int_distribution<size_t> idxDist(0, available.size() - 1);
            mode = available[idxDist(rng)];
        }

        std::cout << "\n[LoomOrchestrator] Lanzando patron Modo: " << mode << " | Profundidad: " << depth << "\n";

        generateLoomPattern(g, depth, true, mode);
    }
}