#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <optional>
#include "ScreenState.h"

namespace ui {
    class IScreen {
    public:
        virtual ~IScreen() = default;
        virtual std::optional<ScreenState> handleEvent(const sf::Event& event) = 0;

        virtual void update(float dt, sf::Vector2f mousePos) = 0;

        virtual void draw(sf::RenderTarget& target) = 0;
    };
}