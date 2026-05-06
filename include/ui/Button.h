#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include <string>
#include <cstdint> 

namespace ui {
    class Button {
    private:
        sf::RectangleShape layerBottom;
        sf::RectangleShape layerMid;
        sf::RectangleShape layerTop;
        sf::Text label;

        sf::Vector2f originalPos;
        float rotation;

        bool isHovered = false;

        float currentActive = 0.0f;
        sf::Clock deltaClock;

        sf::Color blendColors(sf::Color c1, sf::Color c2, float t) {
            return sf::Color(
                static_cast<std::uint8_t>(c1.r + (c2.r - c1.r) * t),
                static_cast<std::uint8_t>(c1.g + (c2.g - c1.g) * t),
                static_cast<std::uint8_t>(c1.b + (c2.b - c1.b) * t),
                c1.a
            );
        }

    public:
        // etopCol, bottomCol 
        Button(const sf::Font& font, const std::string& text, sf::Vector2f centerPos, sf::Vector2f size, float rotationDeg = 0.f,
            sf::Color topCol = sf::Color(0x3D, 0x3D, 0x3D), sf::Color bottomCol = sf::Color(0xB1, 0xB1, 0xB1))
            : layerBottom(size),
            layerMid(size),
            layerTop(size),
            label(font, text, 18),
            originalPos(centerPos),
            rotation(rotationDeg)
        {
            sf::Color midCol = blendColors(topCol, bottomCol, 0.5f);

            // centrar origenes
            layerBottom.setOrigin({ size.x / 2.f, size.y / 2.f });
            layerMid.setOrigin({ size.x / 2.f, size.y / 2.f });
            layerTop.setOrigin({ size.x / 2.f, size.y / 2.f });

            // colores
            layerBottom.setFillColor(bottomCol);
            layerMid.setFillColor(midCol);
            layerTop.setFillColor(topCol);

            // bordes
            sf::Color outlineCol(0x1A, 0x1A, 0x1A); 
            float thick = 1.5f;

            layerBottom.setOutlineColor(outlineCol);
            layerBottom.setOutlineThickness(thick);
            layerMid.setOutlineColor(outlineCol);
            layerMid.setOutlineThickness(thick);
            layerTop.setOutlineColor(outlineCol);
            layerTop.setOutlineThickness(thick);

            // rotacion
            layerBottom.setRotation(sf::degrees(rotationDeg));
            layerMid.setRotation(sf::degrees(rotationDeg));
            layerTop.setRotation(sf::degrees(rotationDeg));

            // texto
            label.setFillColor(sf::Color::White);
            sf::FloatRect textBounds = label.getLocalBounds();
            label.setOrigin({ textBounds.position.x + textBounds.size.x / 2.f,
                              textBounds.position.y + textBounds.size.y / 2.f });
            label.setRotation(sf::degrees(rotationDeg));

            updatePositions(0.0f);
        }

        void update(sf::Vector2f mousePos) {
            float dt = deltaClock.restart().asSeconds();
            if (dt > 0.1f) dt = 0.1f; 

            isHovered = layerTop.getGlobalBounds().contains(mousePos) || layerBottom.getGlobalBounds().contains(mousePos);

            float targetActive = 0.0f; 

            if (isHovered) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    targetActive = 0.5f; 
                }
                else {
                    targetActive = 1.0f; // Hover
                }
            }

            currentActive += (targetActive - currentActive) * 15.0f * dt;

            updatePositions(currentActive);
        }

        void draw(sf::RenderTarget& target) const {
            target.draw(layerBottom);
            target.draw(layerMid);
            target.draw(layerTop);
            target.draw(label);
        }

        bool isCurrentlyHovered() const {
            return isHovered;
        }

    private:
        void updatePositions(float activeValue) {
            float step = 6.0f;

            sf::Vector2f offset(-step, -step);

            layerBottom.setPosition(originalPos);

            layerMid.setPosition(originalPos + (offset * activeValue));

            layerTop.setPosition(originalPos + (offset * 2.0f * activeValue));
            label.setPosition(originalPos + (offset * 2.0f * activeValue));
        }
    };
}