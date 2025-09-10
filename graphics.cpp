#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Map.h"
#include "Spirit.h"

const unsigned int WINDOW_WIDTH = 1500;
const unsigned int WINDOW_HEIGHT = 1000;
const float PLAYER_SPEED = 200.0f;

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "2D Map View with Multiple Spirits");
    window.setFramerateLimit(60);

    Map map("map.png");

    // --- Create multiple spirits for each player ---
    std::vector<Spirit> germanSpirits;
    std::vector<Spirit> americanSpirits;

    // German spirits (red border)
    Spirit fw190("fw190.png", 0.2f);

//    germanSpirits.emplace_back("fw190.png", 0.2f);
    germanSpirits.emplace_back(fw190);
    germanSpirits.back().setPosition(map.getWidth() / 3.0f, map.getHeight() / 2.0f);
    germanSpirits.emplace_back(fw190);
    germanSpirits.back().setPosition(map.getWidth() / 3.0f, map.getHeight() / 3.0f);

    // American spirits (blue border)
    Spirit p51("p51.png", 0.2f);
    americanSpirits.emplace_back(p51);
    americanSpirits.back().setPosition(map.getWidth() * 2.0f / 3.0f, map.getHeight() / 2.0f);
    americanSpirits.emplace_back(p51);
    americanSpirits.back().setPosition(map.getWidth() * 2.0f / 3.0f, map.getHeight() / 3.0f);

    // Set blue border for American spirits
    for (auto& spirit : americanSpirits) {
        spirit.setSelected(false);
        spirit.updateSelectionBorder();
        spirit.setBorderColor(sf::Color::Blue); // Make border blue
    }
    // Set red border for German spirits
    for (auto& spirit : germanSpirits) {
        spirit.setSelected(false);
        spirit.updateSelectionBorder();
        spirit.setBorderColor(sf::Color::Red); // Make border red
    }

    // Create a view to display a portion of the map
    sf::View view(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    view.setCenter(map.getWidth() / 2.0f, map.getHeight() / 2.0f);

    sf::Clock clock;

    // Variables for selection and dragging
    Spirit* selectedSpirit = nullptr;
    bool dragging = false;
    sf::Vector2f dragOffset;
    bool panning = false;
    sf::Vector2f panStart;

    const float PAN_MARGIN = 50.0f;
    const float PAN_SPEED = 0.25f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::Resized) {
                view.setSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
                sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

                selectedSpirit = nullptr;
                // Check German spirits first (topmost last)
                for (auto it = germanSpirits.rbegin(); it != germanSpirits.rend(); ++it) {
                    if (it->getGlobalBounds().contains(mouseWorld)) {
                        selectedSpirit = &(*it);
                        break;
                    }
                }
                // Then check American spirits
                if (!selectedSpirit) {
                    for (auto it = americanSpirits.rbegin(); it != americanSpirits.rend(); ++it) {
                        if (it->getGlobalBounds().contains(mouseWorld)) {
                            selectedSpirit = &(*it);
                            break;
                        }
                    }
                }
                // Set selection states
                for (auto& s : germanSpirits) s.setSelected(selectedSpirit == &s);
                for (auto& s : americanSpirits) s.setSelected(selectedSpirit == &s);

                if (selectedSpirit) {
                    dragging = true;
                    dragOffset = selectedSpirit->getPosition() - mouseWorld;
                    selectedSpirit->setDragOffset(dragOffset);
                } else {
                    panning = true;
                    panStart = mouseWorld;
                }
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                dragging = false;
                panning = false;
            }
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
                sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

                if (dragging && selectedSpirit) {
                    selectedSpirit->setPosition(mouseWorld.x + selectedSpirit->getDragOffset().x,
                                               mouseWorld.y + selectedSpirit->getDragOffset().y);

                    sf::Vector2i spiritInViewPixel = window.mapCoordsToPixel(selectedSpirit->getPosition(), view);
                    sf::Vector2f spiritInView(static_cast<float>(spiritInViewPixel.x), static_cast<float>(spiritInViewPixel.y));

                    bool pan = false;
                    sf::Vector2f panDelta(0.f, 0.f);

                    if (spiritInView.x < PAN_MARGIN) {
                        pan = true;
                        panDelta.x = spiritInView.x - PAN_MARGIN;
                    } else if (spiritInView.x > WINDOW_WIDTH - PAN_MARGIN) {
                        pan = true;
                        panDelta.x = spiritInView.x - (WINDOW_WIDTH - PAN_MARGIN);
                    }
                    if (spiritInView.y < PAN_MARGIN) {
                        pan = true;
                        panDelta.y = spiritInView.y - PAN_MARGIN;
                    } else if (spiritInView.y > WINDOW_HEIGHT - PAN_MARGIN) {
                        pan = true;
                        panDelta.y = spiritInView.y - (WINDOW_HEIGHT - PAN_MARGIN);
                    }

                    if (pan) {
                        view.move(panDelta * PAN_SPEED);
                    }
                } else if (panning && !selectedSpirit) {
                    sf::Vector2f panCurrent = mouseWorld;
                    sf::Vector2f panDelta = panStart - panCurrent;
                    view.move(panDelta * PAN_SPEED);
                    panStart = window.mapPixelToCoords(mousePixel);
                }
            }
        }

        sf::Time deltaTime = clock.restart();

        // Keyboard movement for all spirits (arrows for German, WASD for American)
        for (auto& spirit : germanSpirits) {
            sf::Vector2f movement(0.f, 0.f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) movement.y -= PLAYER_SPEED * deltaTime.asSeconds();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) movement.y += PLAYER_SPEED * deltaTime.asSeconds();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) movement.x -= PLAYER_SPEED * deltaTime.asSeconds();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) movement.x += PLAYER_SPEED * deltaTime.asSeconds();
            spirit.move(movement);
            spirit.updateSelectionBorder();
        }
        for (auto& spirit : americanSpirits) {
            sf::Vector2f movement(0.f, 0.f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) movement.y -= PLAYER_SPEED * deltaTime.asSeconds();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) movement.y += PLAYER_SPEED * deltaTime.asSeconds();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) movement.x -= PLAYER_SPEED * deltaTime.asSeconds();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movement.x += PLAYER_SPEED * deltaTime.asSeconds();
            spirit.move(movement);
            spirit.updateSelectionBorder();
        }

        window.clear(sf::Color::Black);
        window.setView(view);

        map.draw(window);
        for (const auto& spirit : germanSpirits) spirit.draw(window);
        for (const auto& spirit : americanSpirits) spirit.draw(window);

        window.display();
    }

    return 0;
}
