#pragma once
#include "IScreen.h"
#include "Button.h"
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <iostream>

namespace ui {
    class ScreenGenerate : public IScreen {
    private:
        sf::Shader bgShader;
        sf::RectangleShape bgRect;
        float timeAccumulator = 0.0f;

        sf::ConvexShape whiteOverlay;

        sf::Text titleGenerarEcho;
        sf::Text titleGenerarText;
        sf::Text titlePatroEcho;
        sf::Text titlePatroText;

        sf::Text subtitleText;
        sf::Text promptText;

        Button btnRegresar;
        Button btnGenerar;

    public:
        ScreenGenerate(const sf::Font& font, const std::unordered_map<std::string, sf::Texture>& textures)
            : titleGenerarEcho(font, "G e n e r a r", 80),
            titleGenerarText(font, "G e n e r a r", 80),
            titlePatroEcho(font, "P a t r o", 80),
            titlePatroText(font, "P a t r o", 80),

            subtitleText(font, "R E L L E N E   L O S   S I G U I E N T E S   C A M P O S . . .", 16),
            promptText(font, "> Presione ENTER para Generar, ESC para Regresar", 16),

            btnRegresar(font, "R E G R E S A R", { 450.f, 750.f }, { 180.f, 40.f }, 0.f,
                sf::Color(0x3D, 0x3D, 0x3D), sf::Color(0xB1, 0xB1, 0xB1)),

            btnGenerar(font, "GENERAR PATRON", { 750.f, 750.f }, { 220.f, 40.f }, 0.f,
                sf::Color(0x3D, 0x3D, 0x3D), sf::Color(0xB1, 0xB1, 0xB1))
        {
            const std::string fragmentShader = R"(
                #version 120
                uniform float u_time;
                uniform vec2 u_resolution;

                float random(vec2 st) {
                    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
                }

                float noise(vec2 st) {
                    vec2 i = floor(st);
                    vec2 f = fract(st);
                    float a = random(i);
                    float b = random(i + vec2(1.0, 0.0));
                    float c = random(i + vec2(0.0, 1.0));
                    float d = random(i + vec2(1.0, 1.0));
                    vec2 u = f * f * (3.0 - 2.0 * f);
                    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
                }

                void main() {
                    vec2 uv = gl_FragCoord.xy / u_resolution.xy;
                    uv.x *= u_resolution.x / u_resolution.y;

                    float gridSize = 45.0; 
                    vec2 gridUV = fract(uv * gridSize);
                    vec2 gridId = floor(uv * gridSize);

                    vec2 center = vec2(0.5, 0.5);
                    float dist = distance(gridUV, center);

                    float n = noise(gridId * 0.15 + vec2(u_time * 0.2, u_time * 0.15));
                    float radius = mix(0.02, 0.45, n);
                    float circle = smoothstep(radius + 0.05, radius - 0.05, dist);

                    vec3 dotColor = vec3(0.85, 0.85, 0.85); 
                    vec3 bgColor = vec3(1.0, 1.0, 1.0);     

                    gl_FragColor = vec4(mix(bgColor, dotColor, circle), 1.0);
                }
            )";

            if (!bgShader.loadFromMemory(fragmentShader, sf::Shader::Type::Fragment)) {
                std::cerr << "[Warning] Error compilando el shader en ScreenGenerate.\n";
            }

            bgRect.setSize({ 1200.f, 900.f });
            bgRect.setPosition({ 0.f, 0.f });

            // trapecio asimétrico 
            // diseño superior
            whiteOverlay.setPointCount(4);
            whiteOverlay.setPoint(0, sf::Vector2f(0.f, 60.f));
            whiteOverlay.setPoint(1, sf::Vector2f(1200.f, 60.f));
            whiteOverlay.setPoint(2, sf::Vector2f(1200.f, 320.f));
            whiteOverlay.setPoint(3, sf::Vector2f(0.f, 220.f));
            whiteOverlay.setFillColor(sf::Color::White);

            // "Generar"
            titleGenerarEcho.setFillColor(sf::Color(0xDF, 0xDF, 0xDF));
            sf::FloatRect genEchoBounds = titleGenerarEcho.getLocalBounds();
            titleGenerarEcho.setOrigin({ genEchoBounds.position.x + genEchoBounds.size.x / 2.f, genEchoBounds.position.y + genEchoBounds.size.y / 2.f });
            titleGenerarEcho.setPosition({ 406.f, 166.f });

            titleGenerarText.setFillColor(sf::Color(0x3D, 0x3D, 0x3D));
            sf::FloatRect genBounds = titleGenerarText.getLocalBounds();
            titleGenerarText.setOrigin({ genBounds.position.x + genBounds.size.x / 2.f, genBounds.position.y + genBounds.size.y / 2.f });
            titleGenerarText.setPosition({ 400.f, 160.f });

            // "Patron"
            titlePatroEcho.setFillColor(sf::Color(0xDF, 0xDF, 0xDF));
            sf::FloatRect patEchoBounds = titlePatroEcho.getLocalBounds();
            titlePatroEcho.setOrigin({ patEchoBounds.position.x + patEchoBounds.size.x / 2.f, patEchoBounds.position.y + patEchoBounds.size.y / 2.f });
            titlePatroEcho.setPosition({ 806.f, 166.f });

            titlePatroText.setFillColor(sf::Color(0x3D, 0x3D, 0x3D));
            sf::FloatRect patBounds = titlePatroText.getLocalBounds();
            titlePatroText.setOrigin({ patBounds.position.x + patBounds.size.x / 2.f, patBounds.position.y + patBounds.size.y / 2.f });
            titlePatroText.setPosition({ 800.f, 160.f });

            subtitleText.setFillColor(sf::Color(0xB1, 0xB1, 0xB1));
            sf::FloatRect subBounds = subtitleText.getLocalBounds();
            subtitleText.setOrigin({ subBounds.position.x + subBounds.size.x / 2.f, subBounds.position.y + subBounds.size.y / 2.f });
            subtitleText.setPosition({ 600.f, 225.f });

            promptText.setFillColor(sf::Color(0x3D, 0x3D, 0x3D));
            sf::FloatRect promptBounds = promptText.getLocalBounds();
            promptText.setOrigin({ promptBounds.position.x + promptBounds.size.x / 2.f, promptBounds.position.y + promptBounds.size.y / 2.f });
            promptText.setPosition({ 600.f, 400.f });
        }

        std::optional<ScreenState> handleEvent(const sf::Event& event) override {
            if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Enter) return ScreenState::ShowGraph;
                if (key->code == sf::Keyboard::Key::Escape) return ScreenState::Select; 
            }

            if (const auto* mouseBtn = event.getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseBtn->button == sf::Mouse::Button::Left) {
                    if (btnGenerar.isCurrentlyHovered()) return ScreenState::ShowGraph;
                    if (btnRegresar.isCurrentlyHovered()) return ScreenState::Select; 
                }
            }
            return std::nullopt;
        }

        void update(float dt, sf::Vector2f mousePos) override {
            btnRegresar.update(mousePos);
            btnGenerar.update(mousePos);

            // Animar el Shader
            timeAccumulator += dt;
            bgShader.setUniform("u_time", timeAccumulator);
            bgShader.setUniform("u_resolution", sf::Glsl::Vec2(1200.f, 900.f));
        }

        void draw(sf::RenderTarget& target) override {
            target.clear(sf::Color::White);

            sf::RenderStates states;
            states.shader = &bgShader;
            target.draw(bgRect, states);

            target.draw(whiteOverlay);

            target.draw(titleGenerarEcho);
            target.draw(titleGenerarText);
            target.draw(titlePatroEcho);
            target.draw(titlePatroText);

            target.draw(subtitleText);
            target.draw(promptText);

            btnRegresar.draw(target);
            btnGenerar.draw(target);
        }
    };
}