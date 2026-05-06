#pragma once
#include "entities/graph.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>

namespace entities::algorithms::loom {

    // Estructura base para manejar la geometría de las semillas
    struct Circle {
        sf::Vector2f center;
        float radius;

        Circle() : center(0.0f, 0.0f), radius(0.0f) {}
        Circle(float x, float y, float r) : center(x, y), radius(r) {}
        Circle(sf::Vector2f c, float r) : center(c), radius(r) {}
    };

    // Motor matemático para el cálculo de circunferencias tangentes
    class ApollonianGenerator {
    public:
        static std::optional<Circle> soddyCircle(const Circle& c1, const Circle& c2, const Circle& c3);
        static bool exists(const std::vector<Circle>& circles, const Circle& c, float eps = 1.0f);
        static bool areMutuallyTangent(const Circle& a, const Circle& b, const Circle& c, float eps = 1.0f);
        static void generate(std::vector<Circle>& circles,
            const Circle& c1, const Circle& c2, const Circle& c3,
            int currentDepth, int maxDepth);
    };

    // Función principal que el Orchestrator llamará
    void apollonianLoomPattern(Graph& g, int depth);

} // namespace entities::algorithms::loom