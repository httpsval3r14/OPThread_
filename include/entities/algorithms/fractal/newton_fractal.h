#pragma once
#include <SFML/Graphics.hpp>
#include <complex>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>
#include "entities/graph.h"
#include "entities/SeedManager.hpp"
#include "NewtonFractal.h"
#include "NewtonField.h"
#include "NewtonLoomGenerator.h"

namespace entities::algorithms::fractal {

    namespace NewtonLoomPattern {

        inline void generate(entities::Graph& graph, int depth) {
            auto& rng = entities::SeedManager::instance().rng();

            std::uniform_int_distribution<int> eqDist(0, 3);
            NewtonFractal fractal(static_cast<NewtonFractal::EquationType>(eqDist(rng)));

            std::uniform_real_distribution<double> zoomDist(0.8, 2.5);
            double zoom = zoomDist(rng);
            std::uniform_real_distribution<double> offsetDist(-0.5, 0.5);
            double offX = offsetDist(rng), offY = offsetDist(rng);

            double range = 4.0 / zoom;
            double xMin = -range / 2 + offX, xMax = range / 2 + offX;
            double yMin = -range / 2 + offY, yMax = range / 2 + offY;

            int resolution = 35 + depth * 12;
            NewtonField field(resolution, xMin, xMax, yMin, yMax);
            field.compute(fractal, 40, true);

            std::uniform_int_distribution<int> warpType(0, 2);
            std::uniform_real_distribution<double> warpIntensity(0.8, 2.2);
            field.applyWarp(warpType(rng), warpIntensity(rng), sf::Vector2f(0, 0));

            NewtonLoomGenerator::ConnectOptions opts;
            opts.useDiagonal = true;
            opts.knn = 4;
            opts.maxDistance = 75.0f;
            NewtonLoomGenerator generator(opts);
            generator.generate(graph, field);

            std::cout << "\n======================================\n";
            std::cout << "[OPThread] PATRON GENERADO: Newton Loom (Modular)\n";
            std::cout << "-> Profundidad (Depth): " << depth << "\n";
            std::cout << "-> Tipo Ecuacion: " << static_cast<int>(fractal.getType()) << "\n";
            std::cout << "-> Total Nodos: " << graph.getNodes().size() << "\n";
            std::cout << "-> Total Aristas: " << graph.getEdgeCreationOrder().size() << "\n";
            std::cout << "======================================\n";
        }

    } // namespace NewtonLoomPattern
} // namespace entities::algorithms::fractal