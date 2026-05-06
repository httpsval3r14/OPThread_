#include "entities/algorithms/loom/apollonian_gasket.h"
#include "entities/algorithms/loom/loom_constants.h"
#include "entities/algorithms/loom/loom_utils.h"
#include "entities/SeedManager.hpp"
#include <cmath>
#include <random>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <iostream>

namespace entities::algorithms::loom {


    std::optional<Circle> ApollonianGenerator::soddyCircle(const Circle& c1, const Circle& c2, const Circle& c3) {
        float k1 = 1.0f / c1.radius;
        float k2 = 1.0f / c2.radius;
        float k3 = 1.0f / c3.radius;
        float k4 = k1 + k2 + k3 + 2.0f * std::sqrt(k1 * k2 + k2 * k3 + k3 * k1);

        if (k4 <= 0.0f) return std::nullopt;
        float r4 = 1.0f / k4;
        if (r4 < 0.1f) return std::nullopt;

        sf::Vector2f z1 = c1.center, z2 = c2.center, z3 = c3.center;
        float R1 = c1.radius + r4;
        float R2 = c2.radius + r4;
        float R3 = c3.radius + r4;

        float dx = z2.x - z1.x, dy = z2.y - z1.y;
        float d2 = dx * dx + dy * dy;
        if (d2 < 1e-6f) return std::nullopt;

        float a = (R1 * R1 - R2 * R2 + d2) / (2.0f * d2);
        float h = std::sqrt(std::max(0.0f, R1 * R1 / d2 - a * a));
        float x0 = z1.x + a * dx;
        float y0 = z1.y + a * dy;
        float rx = -dy * h;
        float ry = dx * h;

        sf::Vector2f p1(x0 + rx, y0 + ry);
        sf::Vector2f p2(x0 - rx, y0 - ry);

        auto check = [&](const sf::Vector2f& p) {
            float d3 = std::hypot(p.x - z3.x, p.y - z3.y);
            return std::abs(d3 - R3) < 1e-3f;
            };

        if (check(p1)) return Circle(p1, r4);
        if (check(p2)) return Circle(p2, r4);
        return std::nullopt;
    }

    bool ApollonianGenerator::exists(const std::vector<Circle>& circles, const Circle& c, float eps) {
        for (const auto& other : circles) {
            if (std::hypot(c.center.x - other.center.x, c.center.y - other.center.y) < eps &&
                std::abs(c.radius - other.radius) < eps)
                return true;
        }
        return false;
    }

    bool ApollonianGenerator::areMutuallyTangent(const Circle& a, const Circle& b, const Circle& c, float eps) {
        float d12 = std::hypot(a.center.x - b.center.x, a.center.y - b.center.y);
        float d23 = std::hypot(b.center.x - c.center.x, b.center.y - c.center.y);
        float d31 = std::hypot(c.center.x - a.center.x, c.center.y - a.center.y);
        return (std::abs(d12 - (a.radius + b.radius)) < eps &&
            std::abs(d23 - (b.radius + c.radius)) < eps &&
            std::abs(d31 - (c.radius + a.radius)) < eps);
    }

    void ApollonianGenerator::generate(std::vector<Circle>& circles,
        const Circle& c1, const Circle& c2, const Circle& c3,
        int currentDepth, int maxDepth) {
        if (currentDepth > maxDepth) return;
        auto newC = soddyCircle(c1, c2, c3);
        if (!newC.has_value()) return;
        if (exists(circles, *newC)) return;

        circles.push_back(*newC);

        generate(circles, c1, c2, *newC, currentDepth + 1, maxDepth);
        generate(circles, c2, c3, *newC, currentDepth + 1, maxDepth);
        generate(circles, c3, c1, *newC, currentDepth + 1, maxDepth);
    }

    static std::tuple<Circle, Circle, Circle> generateLoomFillingSeedTriplet(std::mt19937& rng, const sf::FloatRect& bounds) {
        std::uniform_real_distribution<float> rDist(bounds.size.x * 0.15f, bounds.size.x * 0.35f);
        float r1 = rDist(rng);
        float r2 = rDist(rng) * 0.7f; 
        float r3 = rDist(rng) * 0.4f; 

        std::uniform_real_distribution<float> xDist(bounds.position.x, bounds.position.x + bounds.size.x);
        std::uniform_real_distribution<float> yDist(bounds.position.y, bounds.position.y + bounds.size.y);
        float cx = xDist(rng);
        float cy = yDist(rng);

        std::uniform_real_distribution<float> angDist(0.0f, 2.0f * 3.14159265f);
        float angle1 = angDist(rng);
        float x2 = cx + (r1 + r2) * std::cos(angle1);
        float y2 = cy + (r1 + r2) * std::sin(angle1);

        float a = r1 + r2; 
        float b = r1 + r3; 
        float c = r2 + r3;

        float cosTheta = (a * a + b * b - c * c) / (2.0f * a * b);
        float theta = std::acos(std::clamp(cosTheta, -1.0f, 1.0f));

        std::uniform_int_distribution<int> coinFlip(0, 1);
        if (coinFlip(rng)) theta = -theta;

        float angle2 = angle1 + theta;
        float x3 = cx + b * std::cos(angle2);
        float y3 = cy + b * std::sin(angle2);

        std::vector<Circle> triplet = { Circle(cx, cy, r1), Circle(x2, y2, r2), Circle(x3, y3, r3) };


        float minSeedX = std::numeric_limits<float>::max();
        float maxSeedX = std::numeric_limits<float>::lowest();
        float minSeedY = std::numeric_limits<float>::max();
        float maxSeedY = std::numeric_limits<float>::lowest();

        for (const auto& circ : triplet) {
            if (circ.center.x - circ.radius < minSeedX) minSeedX = circ.center.x - circ.radius;
            if (circ.center.x + circ.radius > maxSeedX) maxSeedX = circ.center.x + circ.radius;
            if (circ.center.y - circ.radius < minSeedY) minSeedY = circ.center.y - circ.radius;
            if (circ.center.y + circ.radius > maxSeedY) maxSeedY = circ.center.y + circ.radius;
        }

        float seedWidth = maxSeedX - minSeedX;
        float seedHeight = maxSeedY - minSeedY;

        float scaleFactorX = bounds.size.x / seedWidth;
        float scaleFactorY = bounds.size.y / seedHeight;
        float scaleFactor = std::min(scaleFactorX, scaleFactorY);

        sf::Vector2f scaleCenter((minSeedX + maxSeedX) / 2.0f, (minSeedY + maxSeedY) / 2.0f);
        sf::Vector2f boundsCenter(bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f);

        for (auto& circ : triplet) {
            circ.radius *= scaleFactor;
            circ.center = (circ.center - scaleCenter) * scaleFactor + boundsCenter;
        }

        return { triplet[0], triplet[1], triplet[2] };
    }

    void apollonianLoomPattern(Graph& g, int depth) {
        auto& rng = SeedManager::instance().rng();

        std::uniform_int_distribution<int> depthDist(4, 7);
        int actualDepth = depthDist(rng);

        const float minX = -LOOM_WIDTH / 2 + MARGIN;
        const float maxX = LOOM_WIDTH / 2 - MARGIN;
        const float minY = -LOOM_HEIGHT / 2 + MARGIN;
        const float maxY = LOOM_HEIGHT / 2 - MARGIN;
        sf::FloatRect bounds(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));

        auto [base1, base2, base3] = generateLoomFillingSeedTriplet(rng, bounds);

        std::vector<Circle> circles;
        circles.push_back(base1);
        circles.push_back(base2);
        circles.push_back(base3);

        ApollonianGenerator::generate(circles, base1, base2, base3, 1, actualDepth);

        std::vector<uint64_t> centerIds(circles.size());
        for (size_t i = 0; i < circles.size(); ++i) {
            centerIds[i] = g.addNode(circles[i].center.x, circles[i].center.y);
        }


        std::unordered_map<uint64_t, std::vector<std::pair<float, uint64_t>>> candidatesMap;

        for (size_t i = 0; i < circles.size(); ++i) {
            for (size_t j = i + 1; j < circles.size(); ++j) {
                float dist = std::hypot(circles[i].center.x - circles[j].center.x,
                    circles[i].center.y - circles[j].center.y);
                float sumR = circles[i].radius + circles[j].radius;
                if (std::abs(dist - sumR) < 2.0f) {
                    candidatesMap[centerIds[i]].push_back({ dist, centerIds[j] });
                    candidatesMap[centerIds[j]].push_back({ dist, centerIds[i] });
                }
            }
        }

        for (auto& [nodeId, candidates] : candidatesMap) {
            std::sort(candidates.begin(), candidates.end()); 

            int connectionsMade = 0;
            for (const auto& cand : candidates) {
                if (connectionsMade >= 2) break; 
                g.addEdge(nodeId, cand.second, cand.first);
                connectionsMade++;
            }
        }

        std::uniform_int_distribution<int> pointsDist(7, 15); 
        std::uniform_real_distribution<float> phaseDist(0.0f, 6.28318f); 

        for (size_t i = 0; i < circles.size(); ++i) {
            const auto& circ = circles[i];

            if (circ.radius < 5.0f) continue;

            std::vector<uint64_t> ring;
            int pointsPerCircle = pointsDist(rng);
            float phaseOffset = phaseDist(rng); 

            for (int k = 0; k < pointsPerCircle; ++k) {
                float ang = phaseOffset + (2.0f * 3.14159265f * k / pointsPerCircle);
                float x = circ.center.x + circ.radius * std::cos(ang);
                float y = circ.center.y + circ.radius * std::sin(ang);

                if (x >= minX && x <= maxX && y >= minY && y <= maxY) {
                    uint64_t pid = g.addNode(x, y);
                    ring.push_back(pid);
                }
            }

            for (size_t p = 0; p < ring.size(); ++p) {
                g.addEdge(ring[p], ring[(p + 1) % ring.size()], circ.radius * 0.2f);

                if (p % 3 == 0) {
                    g.addEdge(ring[p], centerIds[i], circ.radius, 15);
                }
            }
        }

        auto anchors = createAnchors(g, bounds);
        connectAnchors(g, anchors);
        attachToAnchors(g, anchors, bounds);
        tessellateEmptySpace(g, ANCHOR_SPACING * 1.5f, 5);
        applyTensionRules(g);


    }

} 