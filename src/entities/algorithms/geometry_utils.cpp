#include "entities/algorithms/geometry_utils.h"
#include <algorithm>
#include <vector>
#include <cmath>

namespace entities::algorithms::geometry {
    int computeOutCode(float x, float y, const sf::FloatRect& rect) {
        int code = INSIDE;
        if (x < rect.position.x)      code |= LEFT;
        else if (x > rect.position.x + rect.size.x) code |= RIGHT;
        if (y < rect.position.y)      code |= TOP;
        else if (y > rect.position.y + rect.size.y) code |= BOTTOM;
        return code;
    }

    std::optional<std::pair<std::pair<float, float>, std::pair<float, float>>>
        clipSegment(float x1, float y1, float x2, float y2, const sf::FloatRect& rect,
            std::vector<std::pair<float, float>>& intersections) {

        int code1 = computeOutCode(x1, y1, rect);
        int code2 = computeOutCode(x2, y2, rect);
        int safetyCounter = 0;

        while (safetyCounter < 10) { 
            if (!(code1 | code2)) return std::make_pair(std::make_pair(x1, y1), std::make_pair(x2, y2));
            if (code1 & code2) return std::nullopt;

            int codeOut = code1 ? code1 : code2;
            float x, y;

            if (codeOut & TOP) {
                x = x1 + (x2 - x1) * (rect.position.y - y1) / (y2 - y1);
                y = rect.position.y;
            }
            else if (codeOut & BOTTOM) {
                x = x1 + (x2 - x1) * (rect.position.y + rect.size.y - y1) / (y2 - y1);
                y = rect.position.y + rect.size.y;
            }
            else if (codeOut & LEFT) {
                y = y1 + (y2 - y1) * (rect.position.x - x1) / (x2 - x1);
                x = rect.position.x;
            }
            else { 
                y = y1 + (y2 - y1) * (rect.position.x + rect.size.x - x1) / (x2 - x1);
                x = rect.position.x + rect.size.x;
            }

            if (codeOut == code1) {
                x1 = x; y1 = y;
                code1 = computeOutCode(x1, y1, rect);
            }
            else {
                x2 = x; y2 = y;
                code2 = computeOutCode(x2, y2, rect);
            }
            intersections.emplace_back(x, y);
            safetyCounter++;
        }
        return std::nullopt;
    }
}