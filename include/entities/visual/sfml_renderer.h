#pragma once
#include <SFML/Graphics.hpp>
#include "entities/graph.h"

namespace entities::visual {

    class SfmlRenderer {
    public:
        static void draw(sf::RenderTarget& target, const Graph& graph,
            sf::Color nodeColor = sf::Color(0x7C, 0x7C, 0x7C),
            sf::Color edgeColor = sf::Color(0x3D, 0x3D, 0x3D));

        static void drawAnimated(sf::RenderTarget& target, const Graph& graph,
            float progress,
            sf::Color revealedNodeColor = sf::Color::White,
            sf::Color normalNodeColor = sf::Color(0x7C, 0x7C, 0x7C),
            sf::Color edgeColor = sf::Color(0x3D, 0x3D, 0x3D));

        static void drawBuildAnimation(sf::RenderTarget& target, const Graph& graph,
            float progress,
            sf::Color baseNodeColor = sf::Color(0x3D, 0x3D, 0x3D),
            sf::Color brightNodeColor = sf::Color::White,
            sf::Color baseEdgeColor = sf::Color(0x1A, 0x1A, 0x1A),
            sf::Color brightEdgeColor = sf::Color(0xB1, 0xB1, 0xB1));

    private:
        static sf::Vector2f worldToScreen(const sf::FloatRect& bounds,
            const sf::Vector2u& winSize,
            float wx, float wy);
    };

} 