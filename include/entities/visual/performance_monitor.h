#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

namespace entities::visual {

    struct PerformanceData {
        int depth;
        double timeMs;
        size_t nodes;
        size_t edges;
    };

    class PerformanceMonitor {
    private:
        sf::RenderWindow window;
        std::vector<PerformanceData> history;
        sf::Font font;

        void drawGraphs();
        void drawSingleGraph(sf::FloatRect area, sf::Color color, std::string label, double maxValue, bool isTime);
        void renderEmptyState();

    public:
        PerformanceMonitor();
        void addRecord(PerformanceData data);
        void update();
        bool isOpen() const { return window.isOpen(); }
    };
}