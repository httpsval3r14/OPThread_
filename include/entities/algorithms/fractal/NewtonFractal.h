#pragma once
#include <complex>
#include <functional>

namespace entities::algorithms::fractal {

    class NewtonFractal {
    public:
        using Complex = std::complex<double>;

        enum class EquationType : int {
            Cubic = 0,
            Quartic = 1,
            Star = 2,
            Trigonometric = 3
        };

        explicit NewtonFractal(EquationType type = EquationType::Cubic);

        EquationType getType() const noexcept { return type_; }
        void setType(EquationType type);

        bool iterate(Complex& z, int maxIter = 40, double eps = 1e-6) const;

        struct IterationResult {
            int iterations = 0;
            int rootIndex = -1;
            Complex finalZ{};
        };
        IterationResult analyze(const Complex& z0, int maxIter = 40, double eps = 1e-6) const;

    private:
        EquationType type_;
        std::function<Complex(const Complex&)> f_;
        std::function<Complex(const Complex&)> fPrime_;

        void updateFunctions();
        static int computeRootIndex(const Complex& z) noexcept;
    };

} // namespace entities::algorithms::fractal