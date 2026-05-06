#pragma once
#include "IScreen.h"
#include "Button.h"
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <cmath> 

namespace ui {
    class ScreenMain : public IScreen {
    private:
        sf::Sprite backgroundHalftone;
        sf::Sprite innerCircle;

        sf::RectangleShape whiteMaskBand;

        Button btnSaberMas;
        Button btnSalir;

        sf::Text titleEcho;
        sf::Text titleText;
        sf::Text titleDots;
        sf::RectangleShape titleStrike;

        sf::Text prompt1;
        sf::Text promptEnter;
        sf::Text prompt2;

        sf::Text versionText;

        bool isFinalScreen;
        float blinkTime = 0.0f;

    public:
        ScreenMain(const sf::Font& font, const std::unordered_map<std::string, sf::Texture>& textures, bool isFinal = false)
            : isFinalScreen(isFinal),
            backgroundHalftone(textures.at("halftone_bg")),
            innerCircle(textures.at("circle")),

            btnSaberMas(font, "Saber mas...\n   GitHub", { 280.f, 360.f }, { 180.f, 60.f }, -6.f,
                sf::Color(0x3D, 0x3D, 0x3D), sf::Color(0xB1, 0xB1, 0xB1)),

            btnSalir(font, "S A L I R", { 600.f, 620.f }, { 160.f, 50.f }, 4.f,
                sf::Color(0xB1, 0xB1, 0xB1), sf::Color(0x3D, 0x3D, 0x3D)),

            titleEcho(font, isFinal ? "" : "O P T h r e a d", 85),
            titleText(font, isFinal ? "" : "O P T h r e a d", 85),
            titleDots(font, isFinal ? "" : ".......................................................", 24),
            titleStrike({ 600.f, 10.f }),

            prompt1(font, "presione  ", 25),
            promptEnter(font, "ENTER", 30),
            prompt2(font, isFinal ? "para SALIR" : "  para generar un patron", 20),

            versionText(font, "ver. 1.0.0", 16)
        {
            sf::FloatRect bgBounds = backgroundHalftone.getLocalBounds();
            backgroundHalftone.setOrigin({ bgBounds.size.x / 2.0f, bgBounds.size.y / 2.0f });
            backgroundHalftone.setPosition({ 600.f, 450.f });
            backgroundHalftone.setScale({ 3.5f, 3.5f });

            sf::FloatRect circleBounds = innerCircle.getLocalBounds();
            innerCircle.setOrigin({ circleBounds.size.x / 2.0f, circleBounds.size.y / 2.0f });
            innerCircle.setPosition({ 600.f, 450.f });
            innerCircle.setScale({ 1.8f, 1.8f });

            whiteMaskBand.setSize({ 1200.f, 230.f });
            whiteMaskBand.setOrigin({ 600.f, 115.f });
            whiteMaskBand.setPosition({ 600.f, 450.f });
            whiteMaskBand.setFillColor(sf::Color::White);

            titleEcho.setFillColor(sf::Color(0xB1, 0xB1, 0xB1));
            sf::FloatRect echoBounds = titleEcho.getLocalBounds();
            titleEcho.setOrigin({ echoBounds.position.x + echoBounds.size.x / 2.f, echoBounds.position.y + echoBounds.size.y / 2.f });
            titleEcho.setPosition({ 590.f, 420.f });

            titleText.setFillColor(sf::Color(0x3D, 0x3D, 0x3D));
            sf::FloatRect titleBounds = titleText.getLocalBounds();
            titleText.setOrigin({ titleBounds.position.x + titleBounds.size.x / 2.f, titleBounds.position.y + titleBounds.size.y / 2.f });
            titleText.setPosition({ 600.f, 420.f });

            titleStrike.setOrigin({ 300.f, 5.f });
            titleStrike.setPosition({ 600.f, 435.f });
            titleStrike.setFillColor(sf::Color(0x3D, 0x3D, 0x3D, 210));

            titleDots.setFillColor(sf::Color(0x7C, 0x7C, 0x7C));
            sf::FloatRect dotBounds = titleDots.getLocalBounds();
            titleDots.setOrigin({ dotBounds.position.x + dotBounds.size.x / 2.f, dotBounds.position.y + dotBounds.size.y / 2.f });
            titleDots.setPosition({ 600.f, 490.f });

            promptEnter.setStyle(sf::Text::Bold);

            auto centerVertical = [](sf::Text& t) {
                sf::FloatRect b = t.getLocalBounds();
                t.setOrigin({ 0.f, b.position.y + b.size.y / 2.f });
                };

            centerVertical(prompt1);
            centerVertical(promptEnter);
            centerVertical(prompt2);

            float spacing = 8.f;
            float w1 = prompt1.getLocalBounds().size.x;
            float w2 = promptEnter.getLocalBounds().size.x;
            float w3 = prompt2.getLocalBounds().size.x;
            float totalW = w1 + spacing + w2 + spacing + w3;

            float startX = 600.f - (totalW / 2.f);

            float yPos = isFinalScreen ? 450.f : 540.f; 

            prompt1.setPosition({ startX, yPos });
            promptEnter.setPosition({ startX + w1 + spacing, yPos });
            prompt2.setPosition({ startX + w1 + spacing + w2 + spacing, yPos });

            versionText.setFillColor(sf::Color(0xB1, 0xB1, 0xB1));
            versionText.setPosition({ 20.f, 860.f });
        }

        std::optional<ScreenState> handleEvent(const sf::Event& event) override {
            if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Enter) {
                    return isFinalScreen ? ScreenState::Exit : ScreenState::ShowGraph;
                }
            }
            return std::nullopt;
        }

        void update(float dt, sf::Vector2f mousePos) override {
            if (!isFinalScreen) {
                btnSaberMas.update(mousePos);
                btnSalir.update(mousePos);
            }

            backgroundHalftone.rotate(sf::degrees(10.f * dt));

            blinkTime += dt;
            uint8_t alpha = (std::fmod(blinkTime, 1.2f) < 0.8f) ? 255 : 0;

            sf::Color promptColor(0x7C, 0x7C, 0x7C, alpha);
            prompt1.setFillColor(promptColor);
            promptEnter.setFillColor(promptColor);
            prompt2.setFillColor(promptColor);
        }

        void draw(sf::RenderTarget& target) override {
            target.clear(sf::Color::White);

            target.draw(backgroundHalftone);
            target.draw(innerCircle);

            target.draw(whiteMaskBand);

            if (!isFinalScreen) {
                target.draw(titleEcho);
                target.draw(titleText);
                target.draw(titleStrike);
                target.draw(titleDots);
                target.draw(versionText);
                btnSaberMas.draw(target);

                btnSalir.draw(target);
            }

            target.draw(prompt1);
            target.draw(promptEnter);
            target.draw(prompt2);
        }
    };
}