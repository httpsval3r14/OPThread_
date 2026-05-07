#include "entities/visual/sfml_renderer.h"
#include "entities/node.h"
#include <algorithm>
#include <unordered_set>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace entities::visual {

    static sf::Shader& getPointShader() {
        static sf::Shader shader;
        static bool loaded = false;
        if (!loaded) {
            const std::string vertexSrc = R"(
                void main() {
                    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                    gl_PointSize = 3.0;
                    gl_FrontColor = gl_Color;
                }
            )";
            bool ok = shader.loadFromMemory(vertexSrc, sf::Shader::Type::Vertex);
            loaded = true;
        }
        return shader;
    }

    sf::Vector2f SfmlRenderer::worldToScreen(const sf::FloatRect& bounds,
        const sf::Vector2u& winSize,
        float wx, float wy) {
        float scaleX = winSize.x / bounds.size.x;
        float scaleY = winSize.y / bounds.size.y;
        float x = (wx - bounds.position.x) * scaleX;
        float y = (bounds.position.y + bounds.size.y - wy) * scaleY;
        return { x, y };
    }

    static void drawFrame(sf::RenderTarget& target, const sf::Vector2u& winSize) {
        const float margin = 20.0f;

        sf::RectangleShape topFrame;
        topFrame.setSize(sf::Vector2f(static_cast<float>(winSize.x), margin));
        topFrame.setFillColor(sf::Color::Black);

        sf::RectangleShape bottomFrame;
        bottomFrame.setSize(sf::Vector2f(static_cast<float>(winSize.x), margin));
        bottomFrame.setPosition({ 0.0f, static_cast<float>(winSize.y) - margin });
        bottomFrame.setFillColor(sf::Color::Black);

        sf::RectangleShape leftFrame;
        leftFrame.setSize(sf::Vector2f(margin, static_cast<float>(winSize.y)));
        leftFrame.setFillColor(sf::Color::Black);

        sf::RectangleShape rightFrame;
        rightFrame.setSize(sf::Vector2f(margin, static_cast<float>(winSize.y)));
        rightFrame.setPosition({ static_cast<float>(winSize.x) - margin, 0.0f });
        rightFrame.setFillColor(sf::Color::Black);

        target.draw(topFrame);
        target.draw(bottomFrame);
        target.draw(leftFrame);
        target.draw(rightFrame);
    }

    void SfmlRenderer::draw(sf::RenderTarget& target, const Graph& graph,
        sf::Color nodeColor, sf::Color edgeColor) {
        drawAnimated(target, graph, 1.0f, nodeColor, nodeColor, edgeColor);
    }

    void SfmlRenderer::drawAnimated(sf::RenderTarget& target, const Graph& graph,
        float progress,
        sf::Color revealedNodeColor, sf::Color normalNodeColor,
        sf::Color edgeColor) {
        const auto& nodesMap = graph.getNodes();
        if (nodesMap.empty()) return;

        sf::FloatRect bounds(sf::Vector2f(-600.0f, -450.0f), sf::Vector2f(1200.0f, 900.0f));
        sf::Vector2u winSize = target.getSize();

        auto getPos = [&](uint64_t id) -> sf::Vector2f {
            auto it = nodesMap.find(id);
            if (it != nodesMap.end())
                return worldToScreen(bounds, winSize, it->second.getX(), it->second.getY());
            return { 0, 0 };
            };

        size_t maxEdges = graph.getEdgeCreationOrder().size();
        sf::VertexArray edgeArray(sf::PrimitiveType::Lines, maxEdges * 2);
        size_t edgeIdx = 0;
        for (const auto& [idA, nodeA] : nodesMap) {
            sf::Vector2f posA = getPos(idA);
            for (uint64_t idB : nodeA.getNeighbors()) {
                if (idA > idB) continue;
                auto itB = nodesMap.find(idB);
                if (itB == nodesMap.end()) continue;
                sf::Vector2f posB = getPos(idB);
                if (edgeIdx + 1 < edgeArray.getVertexCount()) {
                    edgeArray[edgeIdx].position = posA;
                    edgeArray[edgeIdx].color = edgeColor;
                    edgeArray[edgeIdx + 1].position = posB;
                    edgeArray[edgeIdx + 1].color = edgeColor;
                    edgeIdx += 2;
                }
                else {
                    edgeArray.append(sf::Vertex(posA, edgeColor));
                    edgeArray.append(sf::Vertex(posB, edgeColor));
                }
            }
        }
        target.draw(edgeArray);

        drawFrame(target, winSize);

        const auto& creationOrder = graph.getNodeCreationOrder();
        size_t totalNodes = creationOrder.size();
        size_t revealedCount = static_cast<size_t>(progress * totalNodes);
        std::unordered_set<uint64_t> revealedSet;
        for (size_t i = 0; i < revealedCount && i < totalNodes; ++i)
            revealedSet.insert(creationOrder[i]);

        sf::VertexArray nodeArray(sf::PrimitiveType::Points, nodesMap.size());
        size_t nodeIdx = 0;
        for (const auto& [id, node] : nodesMap) {
            sf::Vector2f pos = getPos(id);
            nodeArray[nodeIdx].position = pos;
            nodeArray[nodeIdx].color = revealedSet.count(id) ? revealedNodeColor : normalNodeColor;
            ++nodeIdx;
        }
        sf::RenderStates states;
        states.shader = &getPointShader();
        target.draw(nodeArray, states);
    }

    void SfmlRenderer::drawBuildAnimation(sf::RenderTarget& target, const Graph& graph,
        float progress,
        sf::Color baseNodeColor, sf::Color brightNodeColor,
        sf::Color baseEdgeColor, sf::Color brightEdgeColor) {

        const auto& nodesMap = graph.getNodes();
        if (nodesMap.empty()) return;

        target.clear(sf::Color::Black);

        sf::FloatRect bounds(sf::Vector2f(-600.0f, -450.0f), sf::Vector2f(1200.0f, 900.0f));
        sf::Vector2u winSize = target.getSize();

        auto getPos = [&](uint64_t id) -> sf::Vector2f {
            auto it = nodesMap.find(id);
            if (it != nodesMap.end())
                return worldToScreen(bounds, winSize, it->second.getX(), it->second.getY());
            return { 0, 0 };
            };

        const auto& nodeOrder = graph.getNodeCreationOrder();
        const auto& edgeOrder = graph.getEdgeCreationOrder();

        size_t nodeCut = static_cast<size_t>(progress * nodeOrder.size());
        size_t edgeCut = static_cast<size_t>(progress * edgeOrder.size());

        sf::Color glowColor(150, 220, 255, 100);

        sf::VertexArray edgeArray(sf::PrimitiveType::Lines, edgeCut * 2);
        for (size_t i = 0; i < edgeCut; ++i) {
            sf::Vector2f posA = getPos(edgeOrder[i].first);
            sf::Vector2f posB = getPos(edgeOrder[i].second);

            edgeArray[i * 2].position = posA;
            edgeArray[i * 2].color = glowColor;
            edgeArray[i * 2 + 1].position = posB;
            edgeArray[i * 2 + 1].color = glowColor;
        }

        sf::RenderStates glowStates;
        glowStates.blendMode = sf::BlendAdd;
        target.draw(edgeArray, glowStates);

        float margin = 20.0f;
        sf::RectangleShape frame;
        frame.setFillColor(sf::Color::Black);

        frame.setSize({ (float)winSize.x, margin });
        frame.setPosition({ 0.f, 0.f });
        target.draw(frame);

        frame.setPosition({ 0.f, (float)winSize.y - margin });
        target.draw(frame);

        frame.setSize({ margin, (float)winSize.y });
        frame.setPosition({ 0.f, 0.f });
        target.draw(frame);

        frame.setPosition({ (float)winSize.x - margin, 0.f });
        target.draw(frame);

        sf::VertexArray nodeArray(sf::PrimitiveType::Points, nodeCut);
        for (size_t i = 0; i < nodeCut; ++i) {
            nodeArray[i].position = getPos(nodeOrder[i]);
            nodeArray[i].color = sf::Color::White;
        }

        sf::RenderStates nodeStates;
        nodeStates.shader = &getPointShader();
        target.draw(nodeArray, nodeStates);
    }

}