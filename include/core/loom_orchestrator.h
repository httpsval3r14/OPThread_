#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <chrono>
#include <optional>
#include <algorithm>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <string>

#include "entities/visual/sfml_renderer.h"
#include "entities/visual/performance_monitor.h"
#include "entities/graph.h"
#include "entities/algorithms/loom/loom.h"
#include "entities/SeedManager.hpp"

#include "ui/IScreen.h"
#include "ui/ScreenMain.h"
#include "ui/ScreenShowGraph.h"

class LoomOrchestrator {
private:
    sf::RenderWindow mainWindow;
    entities::Graph graph;
    entities::visual::PerformanceMonitor monitor;

    sf::Font globalFont;
    std::unordered_map<std::string, sf::Texture> uiTextures;

    ui::ScreenState currentState;
    std::unique_ptr<ui::IScreen> currentScreen;

    sf::Clock animationClock;
    float animationProgress = 0.0f;
    const float animationDuration = 3.0f;

    void changeState(ui::ScreenState newState) {
        currentState = newState;

        switch (newState) {
        case ui::ScreenState::Main:
            currentScreen = std::make_unique<ui::ScreenMain>(globalFont, uiTextures, false);
            break;

        case ui::ScreenState::ShowGraph:

            currentScreen = std::make_unique<ui::ScreenShowGraph>(globalFont, uiTextures, graph, true);
            break;
        case ui::ScreenState::Final:
            currentScreen = std::make_unique<ui::ScreenMain>(globalFont, uiTextures, true);
            break;
        case ui::ScreenState::Exit:
            mainWindow.close();
            break;
        }
    }

    void loadTexture(const std::string& key, const std::string& path) {
        if (!uiTextures[key].loadFromFile(path)) {
            std::cerr << "[Warning] No se pudo cargar: " << path << "\n";
        }
    }

public:
    LoomOrchestrator() {
        mainWindow.create(sf::VideoMode({ 1200, 900 }), "OPThread - Graph Engine");
        mainWindow.setFramerateLimit(60);

        if (!globalFont.openFromFile("assets/firacode.ttf")) {
            std::cerr << "[Warning] FiraCode no encontrado.\n";
        }

        loadTexture("circle", "assets/ui/circle.png");
        loadTexture("select_screen", "assets/ui/select_screen.png");

        loadTexture("halftone_bg", "assets/ui/halftone_bg.png");
        loadTexture("halftone", "assets/ui/halftone.png");
        loadTexture("toolbar_save", "assets/ui/toolbar_save.png");
        loadTexture("toolbar_undo", "assets/ui/toolbar_undo.png");
        loadTexture("toolbar_redo", "assets/ui/toolbar_redo.png");
        loadTexture("toolbar_zoom_in", "assets/ui/toolbar_zoom_in.png");
        loadTexture("toolbar_zoom_out", "assets/ui/toolbar_zoom_out.png");
        loadTexture("toolbar_pan", "assets/ui/toolbar_pan.png");
        loadTexture("toolbar_pencil", "assets/ui/toolbar_pencil.png");
        loadTexture("toolbar_edit", "assets/ui/toolbar_edit.png");
        loadTexture("toolbar_trash", "assets/ui/toolbar_trash.png");
        loadTexture("player_prev", "assets/ui/player_prev.png");
        loadTexture("player_pause", "assets/ui/player_pause.png");
        loadTexture("player_play", "assets/ui/player_play.png");
        loadTexture("player_next", "assets/ui/player_next.png");

        entities::SeedManager::instance().initRandom();
        changeState(ui::ScreenState::Main);
    }

    void run() {
        sf::Clock deltaClock;

        while (mainWindow.isOpen()) {
            float dt = deltaClock.restart().asSeconds();

            while (const std::optional<sf::Event> event = mainWindow.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    mainWindow.close();
                }
                else if (currentScreen) {
                    if (auto nextState = currentScreen->handleEvent(*event)) {
                        changeState(*nextState);
                    }
                }
            }

            sf::Vector2i pixelPos = sf::Mouse::getPosition(mainWindow);
            sf::Vector2f worldPos = mainWindow.mapPixelToCoords(pixelPos);

            if (currentState == ui::ScreenState::ShowGraph) {
                animationProgress = std::min(1.0f, animationClock.getElapsedTime().asSeconds() / animationDuration);
                if (auto* graphScreen = dynamic_cast<ui::ScreenShowGraph*>(currentScreen.get())) {
                    graphScreen->updateAnimation(animationProgress);
                }
            }

            if (currentScreen) currentScreen->update(dt, worldPos);

            if (mainWindow.isOpen() && currentScreen) {
                currentScreen->draw(mainWindow);
                mainWindow.display();
            }

            monitor.update();
        }
    }
};