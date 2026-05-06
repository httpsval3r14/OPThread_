#define _USE_MATH_DEFINES
#include "entities/algorithms/fractal/NewtonFractal.h"
#include <cmath>

namespace entities::algorithms::fractal {

    NewtonFractal::NewtonFractal(EquationType type)
        : type_(type) {
        updateFunctions();
    }

    void NewtonFractal::setType(EquationType type) {
        type_ = type;
        updateFunctions();
    }

    void NewtonFractal::updateFunctions() {
        switch (type_) {
        case EquationType::Cubic:
            f_ = [](const Complex& z) { return z * z * z - 1.0; };
            fPrime_ = [](const Complex& z) { return 3.0 * z * z; };
            break;
        case EquationType::Quartic:
            f_ = [](const Complex& z) { return z * z * z * z - 1.0; };
            fPrime_ = [](const Complex& z) { return 4.0 * z * z * z; };
            break;
        case EquationType::Star:
            f_ = [](const Complex& z) { return z * z * z * z * z * z + z * z * z - 1.0; };
            fPrime_ = [](const Complex& z) { return 6.0 * z * z * z * z * z + 3.0 * z * z; };
            break;
        case EquationType::Trigonometric:
            f_ = [](const Complex& z) { return std::sin(z) - 1.0; };
            fPrime_ = [](const Complex& z) { return std::cos(z); };
            break;
        }
    }

    bool NewtonFractal::iterate(Complex& z, int maxIter, double eps) const {
        for (int i = 0; i < maxIter; ++i) {
            Complex fz = f_(z);
            if (std::abs(fz) < eps) return true;
            Complex fprime = fPrime_(z);
            if (std::abs(fprime) < 1e-9) return false;
            z = z - fz / fprime;
        }
        return false;
    }

    int NewtonFractal::computeRootIndex(const Complex& z) noexcept {
        double angle = std::arg(z);
        int idx = static_cast<int>((angle + M_PI) / (2.0 * M_PI) * 5.0);
        return idx % 5;
    }

    NewtonFractal::IterationResult NewtonFractal::analyze(const Complex& z0, int maxIter, double eps) const {
        IterationResult result;
        Complex z = z0;
        for (int i = 0; i < maxIter; ++i) {
            Complex fz = f_(z);
            if (std::abs(fz) < eps) {
                result.iterations = i + 1;
                result.finalZ = z;
                result.rootIndex = computeRootIndex(z);
                return result;
            }
            Complex fprime = fPrime_(z);
            if (std::abs(fprime) < 1e-9) break;
            z = z - fz / fprime;
        }
        result.iterations = maxIter;
        result.finalZ = z;
        result.rootIndex = computeRootIndex(z);
        return result;
    }

} 