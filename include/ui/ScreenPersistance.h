#pragma once
#include "IScreen.h"
#include <SFML/Graphics.hpp>

namespace ui {
    class ScreenPersistance : public IScreen {
    private:
        sf::Text titleText;
        sf::Text promptText;

    public:
        ScreenPersistance(const sf::Font& font)
            : titleText(font, "Cargar un Patron", 48),
            promptText(font, "> Ingrese ruta JSON (ENTER simula carga, ESC regresa)", 16)
        {
            titleText.setFillColor(sf::Color::Black);
            titleText.setPosition({ 120.f, 100.f });

            promptText.setFillColor(sf::Color(0x7C, 0x7C, 0x7C));
            promptText.setPosition({ 125.f, 200.f });
        }

        std::optional<ScreenState> handleEvent(const sf::Event& event) override {
            if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Enter) return ScreenState::ShowGraph;
                if (key->code == sf::Keyboard::Key::Escape) return ScreenState::Main;
            }
            return std::nullopt;
        }

        void update(float dt, sf::Vector2f mousePos) override {
            // jajaja no tiene nada
        }

        void draw(sf::RenderTarget& target) override {
            target.clear(sf::Color::White);
            target.draw(titleText);
            target.draw(promptText);
        }
    };
}