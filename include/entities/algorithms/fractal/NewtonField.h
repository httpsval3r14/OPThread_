#pragma once
#include <SFML/Graphics.hpp>
#include <complex>
#include <vector>

namespace entities::algorithms::fractal {

    class NewtonFractal;

    class NewtonField {
    public:
        using Complex = std::complex<double>;

        struct Cell {
            sf::Vector2f position;
            Complex complexPlane;
            int rootIndex = -1;
            int iterations = 0;
        };

        NewtonField(int resolution, double xMin, double xMax, double yMin, double yMax);

        void compute(const NewtonFractal& fractal, int maxIter = 40, bool parallel = true);
        void applyWarp(int type, double intensity = 1.0, const sf::Vector2f& center = { 0,0 });

        const std::vector<Cell>& getCells() const noexcept { return cells_; }
        int getResolution() const noexcept { return resolution_; }

    private:
        int resolution_;
        double xMin_, xMax_, yMin_, yMax_;
        std::vector<Cell> cells_;

        void computeComplexPlane();
        void applySwirlWarp(double intensity, const sf::Vector2f& center);
        void applyRippleWarp(double intensity, const sf::Vector2f& center);
        void applyRadialTwistWarp(double intensity, const sf::Vector2f& center);
    };

}