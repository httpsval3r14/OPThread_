#include "entities/algorithms/fractal/NewtonField.h"
#include "entities/algorithms/fractal/NewtonFractal.h"
#include <execution>
#include <algorithm>
#include <cmath>

namespace entities::algorithms::fractal {

    NewtonField::NewtonField(int resolution, double xMin, double xMax, double yMin, double yMax)
        : resolution_(resolution), xMin_(xMin), xMax_(xMax), yMin_(yMin), yMax_(yMax) {
        cells_.resize(resolution_ * resolution_);
        computeComplexPlane();
    }

    void NewtonField::computeComplexPlane() {
        double stepX = (xMax_ - xMin_) / (resolution_ - 1);
        double stepY = (yMax_ - yMin_) / (resolution_ - 1);
        for (int i = 0; i < resolution_; ++i) {
            double real = xMin_ + i * stepX;
            for (int j = 0; j < resolution_; ++j) {
                double imag = yMin_ + j * stepY;

                size_t idx = static_cast<size_t>(j) * resolution_ + i;

                cells_[idx].complexPlane = Complex(real, imag);
                cells_[idx].position = sf::Vector2f(
                    static_cast<float>((real - xMin_) / (xMax_ - xMin_) * 1200.0 - 600.0),
                    static_cast<float>((imag - yMin_) / (yMax_ - yMin_) * 900.0 - 450.0)
                );
            }
        }
    }

    void NewtonField::compute(const NewtonFractal& fractal, int maxIter, bool parallel) {
        auto computeCell = [&](Cell& cell) {
            auto res = fractal.analyze(cell.complexPlane, maxIter);
            cell.iterations = res.iterations;
            cell.rootIndex = res.rootIndex;
            };
        if (parallel) {
            std::for_each(std::execution::par, cells_.begin(), cells_.end(), computeCell);
        }
        else {
            std::for_each(cells_.begin(), cells_.end(), computeCell);
        }
    }

    void NewtonField::applyWarp(int type, double intensity, const sf::Vector2f& center) {
        switch (type) {
        case 1: applyRippleWarp(intensity, center); break;
        case 2: applyRadialTwistWarp(intensity, center); break;
        default: applySwirlWarp(intensity, center); break;
        }
    }

    void NewtonField::applySwirlWarp(double intensity, const sf::Vector2f& center) {
        for (auto& cell : cells_) {
            sf::Vector2f d = cell.position - center;
            float radius = std::sqrt(d.x * d.x + d.y * d.y);
            if (radius < 1e-6f) continue;
            float angle = std::atan2(d.y, d.x);
            float iterFactor = static_cast<float>(cell.iterations) / 40.0f;
            float swirl = static_cast<float>(intensity) * iterFactor * (radius / 300.0f);
            float newAngle = angle + swirl;
            cell.position.x = center.x + radius * std::cos(newAngle);
            cell.position.y = center.y + radius * std::sin(newAngle);
        }
    }

    void NewtonField::applyRippleWarp(double intensity, const sf::Vector2f& center) {
        for (auto& cell : cells_) {
            sf::Vector2f d = cell.position - center;
            float radius = std::sqrt(d.x * d.x + d.y * d.y);
            float ripple = static_cast<float>(intensity) * std::sin(radius * 0.05f);
            cell.position.x += ripple * d.y / (radius + 1e-6f);
            cell.position.y -= ripple * d.x / (radius + 1e-6f);
        }
    }

    void NewtonField::applyRadialTwistWarp(double intensity, const sf::Vector2f& center) {
        for (auto& cell : cells_) {
            sf::Vector2f d = cell.position - center;
            float radius = std::sqrt(d.x * d.x + d.y * d.y);
            float twist = static_cast<float>(intensity) * (radius / 200.0f);
            float angle = std::atan2(d.y, d.x) + twist;
            cell.position.x = center.x + radius * std::cos(angle);
            cell.position.y = center.y + radius * std::sin(angle);
        }
    }

} 