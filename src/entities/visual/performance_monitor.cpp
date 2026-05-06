#include "entities/visual/performance_monitor.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Text.hpp>

namespace entities::visual {
    //
    PerformanceMonitor::PerformanceMonitor() {
        window.create(sf::VideoMode({ 400, 600 }), "OPThread - Performance Monitor");
        window.setFramerateLimit(60);

        if (!font.openFromFile("C:/Users/valer/source/repos/OPThread/assets/firacode.ttf")) {
            std::cerr << "Error CRITICO: No se pudo cargar Fira Code." << std::endl;
        }
    }

    void PerformanceMonitor::addRecord(PerformanceData data) {
        history.push_back(data);
        if (history.size() > 100) {
            history.erase(history.begin());
        }
    }

    void PerformanceMonitor::update() {
        if (!window.isOpen()) return;

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color(15, 15, 20));

        if (history.empty()) {
            renderEmptyState();
        }
        else {
            drawGraphs();
        }

        window.display();
    }

    void PerformanceMonitor::drawGraphs() {
        if (history.empty()) return;

        double maxTime = 0.0;
        size_t maxNodes = 0;
        size_t maxEdges = 0;

        for (const auto& data : history) {
            if (data.timeMs > maxTime) maxTime = data.timeMs;
            if (data.nodes > maxNodes) maxNodes = data.nodes;
            if (data.edges > maxEdges) maxEdges = data.edges;
        }

        if (maxTime == 0.0) maxTime = 1.0;
        if (maxNodes == 0) maxNodes = 1;
        if (maxEdges == 0) maxEdges = 1;

        sf::FloatRect timeArea({ 20.0f, 60.0f }, { 360.0f, 100.0f });
        sf::FloatRect nodesArea({ 20.0f, 240.0f }, { 360.0f, 100.0f });
        sf::FloatRect edgesArea({ 20.0f, 420.0f }, { 360.0f, 100.0f });

        drawSingleGraph(timeArea, sf::Color::Cyan, "Tiempo de Ejecucion", maxTime, true);
        drawSingleGraph(nodesArea, sf::Color::Green, "Cantidad de Nodos", static_cast<double>(maxNodes), false);
        drawSingleGraph(edgesArea, sf::Color::Yellow, "Cantidad de Aristas", static_cast<double>(maxEdges), false);
    }

    void PerformanceMonitor::drawSingleGraph(sf::FloatRect area, sf::Color color, std::string label, double maxValue, bool isTime) {
        sf::RectangleShape bg;
        bg.setSize(area.size);
        bg.setPosition(area.position);
        bg.setFillColor(sf::Color(30, 30, 40));
        bg.setOutlineColor(sf::Color(60, 60, 80));
        bg.setOutlineThickness(1.0f);
        window.draw(bg);

        std::ostringstream oss;
        if (isTime) {
            oss << label << " (Max: " << std::fixed << std::setprecision(2) << maxValue << "ms)";
        }
        else {
            oss << label << " (Max: " << static_cast<size_t>(maxValue) << ")";
        }

        sf::Text titleText(font, oss.str(), 14);
        titleText.setFillColor(color);
        titleText.setPosition({ area.position.x, area.position.y - 25.0f });
        window.draw(titleText);

        if (history.size() < 2) return;

        sf::VertexArray graphLine(sf::PrimitiveType::LineStrip, history.size());

        const float maxHistory = 100.0f;
        float stepX = area.size.x / (maxHistory - 1.0f);

        float startX = area.position.x + (maxHistory - history.size()) * stepX;

        for (size_t i = 0; i < history.size(); ++i) {
            double currentValue = 0.0;
            if (label.find("Tiempo") != std::string::npos) currentValue = history[i].timeMs;
            else if (label.find("Nodos") != std::string::npos) currentValue = static_cast<double>(history[i].nodes);
            else if (label.find("Aristas") != std::string::npos) currentValue = static_cast<double>(history[i].edges);

            float x = startX + static_cast<float>(i) * stepX;
            float normalizedValue = static_cast<float>(currentValue / maxValue);
            float y = area.position.y + area.size.y - (normalizedValue * area.size.y);

            graphLine[i].position = { x, y };
            graphLine[i].color = color;
        }

        window.draw(graphLine);
    }

    void PerformanceMonitor::renderEmptyState() {
        sf::Text text(font, "Esperando datos...", 16);
        text.setFillColor(sf::Color(150, 150, 150));
        text.setPosition({ 80.0f, 280.0f });
        window.draw(text);
    }
}