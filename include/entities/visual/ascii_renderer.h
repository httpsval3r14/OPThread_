#pragma once
#include "entities/graph.h"

namespace entities::visual {
    class AsciiRenderer {
    public:
        static void print(const Graph& graph, int screenWidth = 80, int screenHeight = 24);
    };

}