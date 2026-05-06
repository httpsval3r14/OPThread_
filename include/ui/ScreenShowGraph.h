#pragma once
#include "IScreen.h"
#include "Button.h"
#include "entities/graph.h"
#include "entities/visual/sfml_renderer.h"
#include "entities/algorithms/loom/loom.h" 
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream> 
#include <future> 

namespace ui {
    class ScreenShowGraph : public IScreen {
    private:
        sf::RectangleShape topToolbar;
        sf::RectangleShape leftSidebar;
        std::vector<sf::Text> statsTexts;

        sf::Shader uiShader;
        float timeAccumulator = 0.0f;

        Button btnRegresar;
        Button btnAnimar;
        Button btnGenerarAleatorio;
        Button btnOptimizar;
        Button btnSalir;

        const sf::Font& fontRef;
        entities::Graph& graphRef;
        float currentProgress = 0.0f;

        sf::Clock replayClock;
        bool isReplaying = false;
        const float REPLAY_DURATION = 3.0f;

        std::future<void> backgroundTask;
        bool isLoading = false;

        sf::RectangleShape loadingOverlay;
        sf::CircleShape spinner;
        sf::Text loadingText;
        float spinnerRotation = 0.0f;

        void updateStats() {
            statsTexts.clear();
            double totalThreadLength = 0;

            for (const auto& edge : graphRef.getEdgeCreationOrder()) {
                auto itA = graphRef.getNodes().find(edge.first);
                if (itA != graphRef.getNodes().end()) {
                    totalThreadLength += itA->second.getWeightTo(edge.second);
                }
            }

            double lengthInCm = totalThreadLength / 10.0;

            std::vector<std::string> labels = {
                 "Nodos (Clavos):\n" + std::to_string(graphRef.getNodes().size()),
                 "Total Aristas:\n" + std::to_string(graphRef.getEdgeCreationOrder().size()),
                 "Longitud de\nhilo:\n" + std::to_string(static_cast<int>(lengthInCm)) + " cm",
                 "Costo est.\n$" + std::to_string(static_cast<int>(lengthInCm * 0.5)) + " MXN"
            };

            for (size_t i = 0; i < labels.size(); ++i) {
                sf::Text t(fontRef, labels[i], 14);
                t.setFillColor(sf::Color(0x20, 0x20, 0x20)); 
                t.setPosition({ 10.f, 130.f + (i * 80.f) });
                statsTexts.push_back(t);
            }
        }

        void setLoadingText(const std::string& textStr) {
            loadingText.setString(textStr);
            sf::FloatRect textBounds = loadingText.getLocalBounds();
            loadingText.setOrigin({ textBounds.size.x / 2.f, textBounds.size.y / 2.f });
            loadingText.setPosition({ 600.f, 500.f });
        }

    public:
        ScreenShowGraph(const sf::Font& font, const std::unordered_map<std::string, sf::Texture>& textures, entities::Graph& g, bool startGenerating = false)
            : fontRef(font), graphRef(g),
            btnRegresar(font, "REGRESAR", { 200.f, 45.f }, { 90.f, 45.f }),
            btnAnimar(font, "ANIMAR", { 350.f, 45.f }, { 90.f, 45.f }),
            btnGenerarAleatorio(font, "NUEVO", { 500.f, 45.f }, { 90.f, 45.f }),
            btnOptimizar(font, "OPTIMIZAR", { 675.f, 45.f }, { 160.f, 45.f }, 0.f, sf::Color(0x2E, 0x8B, 0x57), sf::Color(0x98, 0xFB, 0x98)),
            btnSalir(font, "SALIR", { 850.f, 45.f }, { 90.f, 45.f }),
            loadingText(font)
        {
            const std::string fragmentShader = R"(
                #version 120
                uniform float u_time;
                void main() {
                    // Escala y desplazamiento
                    vec2 st = gl_FragCoord.xy / 100.0;
                    st.x += u_time * 0.25;
                    st.y -= u_time * 0.15;
                    
                    vec2 f = fract(st);
                    
                    // Cortes geométricos (Triángulos y diamantes)
                    float v1 = step(f.x, f.y);
                    float v2 = step(1.0 - f.x, f.y);
                    
                    // Mezcla de tonos grises muy claros (para que el texto se lea)
                    float color = 0.82 + 0.06 * v1 + 0.08 * v2; 
                    
                    gl_FragColor = vec4(color, color, color, 1.0);
                }
            )";

            if (!uiShader.loadFromMemory(fragmentShader, sf::Shader::Type::Fragment)) {
                std::cerr << "[Warning] Error compilando el shader de UI.\n";
            }

            topToolbar.setSize({ 1200.f, 100.f });
            topToolbar.setOutlineColor(sf::Color(0x1A, 0x1A, 0x1A));
            topToolbar.setOutlineThickness(2.f);

            leftSidebar.setSize({ 250.f, 800.f });
            leftSidebar.setPosition({ 0.f, 100.f });
            leftSidebar.setOutlineColor(sf::Color(0x1A, 0x1A, 0x1A));
            leftSidebar.setOutlineThickness(2.f);

            // pantalla de carga
            loadingOverlay.setSize({ 1200.f, 900.f });
            loadingOverlay.setFillColor(sf::Color(0, 0, 0, 200)); 

            loadingText.setCharacterSize(28);
            loadingText.setFillColor(sf::Color::White); 

            spinner.setRadius(40.f);
            spinner.setPointCount(6);
            spinner.setOrigin({ 40.f, 40.f });
            spinner.setPosition({ 600.f, 400.f });
            spinner.setFillColor(sf::Color::Transparent);
            spinner.setOutlineColor(sf::Color(0x2E, 0x8B, 0x57));
            spinner.setOutlineThickness(5.f);

            if (startGenerating) {
                isLoading = true;
                setLoadingText("G E N E R A N D O . . .");

                backgroundTask = std::async(std::launch::async, [&]() {
                    graphRef = entities::Graph();
                    int depth = 0;
                    int mode = -1;
                    entities::algorithms::loom::randomLoomPattern(graphRef, depth, mode);
                    });
            }
            else {
                updateStats();
            }
        }

        void updateAnimation(float p) {
            if (isReplaying) {
                float localP = replayClock.getElapsedTime().asSeconds() / REPLAY_DURATION;
                if (localP >= 1.0f) {
                    localP = 1.0f;
                    isReplaying = false;
                }
                currentProgress = localP;
            }
            else {
                currentProgress = p;
            }
        }

        std::optional<ScreenState> handleEvent(const sf::Event& event) override {
            if (isLoading) return std::nullopt;

            if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    return ScreenState::Final;
                }
            }

            if (const auto* mouseBtn = event.getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseBtn->button == sf::Mouse::Button::Left) {

                    if (btnRegresar.isCurrentlyHovered()) return ScreenState::Main;

                    if (btnSalir.isCurrentlyHovered()) return ScreenState::Final;

                    if (btnAnimar.isCurrentlyHovered()) {
                        replayClock.restart();
                        isReplaying = true;
                        return std::nullopt;
                    }

                    if (btnGenerarAleatorio.isCurrentlyHovered()) {
                        return ScreenState::ShowGraph;
                    }

                    if (btnOptimizar.isCurrentlyHovered()) {
                        isLoading = true;
                        setLoadingText("O P T I M I Z A N D O . . .");

                        backgroundTask = std::async(std::launch::async, [&]() {
                            graphRef.applyKruskal();
                            });
                        return std::nullopt;
                    }
                }
            }
            return std::nullopt;
        }

        void update(float dt, sf::Vector2f mousePos) override {
            timeAccumulator += dt;
            uiShader.setUniform("u_time", timeAccumulator);

            if (isLoading) {
                spinnerRotation += 200.f * dt;
                spinner.setRotation(sf::degrees(spinnerRotation));

                if (backgroundTask.valid() && backgroundTask.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                    isLoading = false;
                    updateStats();

                    replayClock.restart();
                    isReplaying = true;
                }
            }
            else {
                btnRegresar.update(mousePos);
                btnAnimar.update(mousePos);
                btnGenerarAleatorio.update(mousePos);
                btnOptimizar.update(mousePos);
                btnSalir.update(mousePos);
            }
        }

        void draw(sf::RenderTarget& target) override {
            // La limpieza del renderizador la maneja SfmlRenderer, pero por seguridad:
            target.clear(sf::Color::Black);

            if (!isLoading) {
                entities::visual::SfmlRenderer::drawBuildAnimation(
                    target, graphRef, currentProgress,
                    sf::Color(150, 150, 150), sf::Color::White, // Nodos: Gris -> Blanco
                    sf::Color::White, sf::Color(200, 200, 200)  // Aristas: Blancas
                );
            }

            sf::RenderStates states;
            states.shader = &uiShader;
            target.draw(topToolbar, states);
            target.draw(leftSidebar, states);

            sf::RectangleShape separator({ 250.f, 2.f });
            separator.setFillColor(sf::Color::Black);
            separator.setPosition({ 0.f, 100.f });
            target.draw(separator);

            btnRegresar.draw(target);
            btnAnimar.draw(target);
            btnGenerarAleatorio.draw(target);
            btnOptimizar.draw(target);
            btnSalir.draw(target);

            for (const auto& text : statsTexts) target.draw(text);

            if (isLoading) {
                target.draw(loadingOverlay);
                target.draw(spinner);
                target.draw(loadingText);
            }
        }
    };
}