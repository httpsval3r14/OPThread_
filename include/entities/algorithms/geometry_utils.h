#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <optional>
#include <vector>
#include <utility>

namespace entities::algorithms::geometry {
    enum OutCode { INSIDE = 0, LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8 };
    int computeOutCode(float x, float y, const sf::FloatRect& rect);
    std::optional<std::pair<std::pair<float, float>, std::pair<float, float>>>
        clipSegment(float x1, float y1, float x2, float y2, const sf::FloatRect& rect,
            std::vector<std::pair<float, float>>& intersections);
}