#include <SFML/Graphics.hpp>
#include <iostream>

// The size of the window
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

// The speed at which the player moves
const float PLAYER_SPEED = 200.0f; 

int main() {
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "2D Map View with Movable Item");
    window.setFramerateLimit(60);

    // Load the main map texture
    sf::Texture mapTexture;
    if (!mapTexture.loadFromFile("map.png")) {
        std::cerr << "Error: Could not load map.png" << std::endl;
        return 1;
    }

    // Create a sprite for the map using the texture
    sf::Sprite mapSprite(mapTexture);

    // Create a view to display a portion of the map
    sf::View view(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));

    // Load the player texture
    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("fw190.png")) {
        std::cerr << "Error: Could not load player.png" << std::endl;
        return 1;
    }

    // Create a sprite for the player
    sf::Sprite playerSprite(playerTexture);
    // Set the initial position of the player
    playerSprite.setPosition(mapSprite.getLocalBounds().width / 2.0f, mapSprite.getLocalBounds().height / 2.0f);

    // Create a rectangle shape for the selection border
    sf::RectangleShape selectionBorder;
    selectionBorder.setFillColor(sf::Color::Transparent);
    selectionBorder.setOutlineColor(sf::Color::Red);
    selectionBorder.setOutlineThickness(3.0f);
    selectionBorder.setSize(sf::Vector2f(playerSprite.getGlobalBounds().width, playerSprite.getGlobalBounds().height));
    selectionBorder.setOrigin(playerSprite.getOrigin());

    // Variable to track elapsed time for consistent movement
    sf::Clock clock;

    // Variables for selection and dragging
    bool playerSelected = false;
    bool dragging = false;
    sf::Vector2f dragOffset;
    bool panning = false;
    sf::Vector2f panStart;

    // Margin from the edge of the view to trigger panning when dragging player
    const float PAN_MARGIN = 50.0f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Mouse button pressed
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                // Convert mouse position to world coordinates
                sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
                sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

                // Check if mouse is over playerSprite
                if (playerSprite.getGlobalBounds().contains(mouseWorld)) {
                    playerSelected = true;
                    dragging = true;
                    dragOffset = playerSprite.getPosition() - mouseWorld;
                } else {
                    playerSelected = false;
                    panning = true;
                    panStart = mouseWorld;
                }
            }
            // Mouse button released
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                dragging = false;
                panning = false;
            }
            // Mouse moved
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
                sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

                if (dragging && playerSelected) {
                    // Move player
                    playerSprite.setPosition(mouseWorld + dragOffset);

                    // Get player's position in view coordinates
                    sf::Vector2i playerInViewPixel = window.mapCoordsToPixel(playerSprite.getPosition(), view);
                    sf::Vector2f playerInView(static_cast<float>(playerInViewPixel.x), static_cast<float>(playerInViewPixel.y));

                    // Check if player is near the edge of the view and pan if needed
                    bool pan = false;
                    sf::Vector2f panDelta(0.f, 0.f);

                    if (playerInView.x < PAN_MARGIN) {
                        pan = true;
                        panDelta.x = playerInView.x - PAN_MARGIN;
                    } else if (playerInView.x > WINDOW_WIDTH - PAN_MARGIN) {
                        pan = true;
                        panDelta.x = playerInView.x - (WINDOW_WIDTH - PAN_MARGIN);
                    }
                    if (playerInView.y < PAN_MARGIN) {
                        pan = true;
                        panDelta.y = playerInView.y - PAN_MARGIN;
                    } else if (playerInView.y > WINDOW_HEIGHT - PAN_MARGIN) {
                        pan = true;
                        panDelta.y = playerInView.y - (WINDOW_HEIGHT - PAN_MARGIN);
                    }

                    if (pan) {
                        // Move the view by the negative of the panDelta in world coordinates
                        view.move(panDelta);
                    }
                } else if (panning && !playerSelected) {
                    sf::Vector2f panCurrent = mouseWorld;
                    sf::Vector2f panDelta = panStart - panCurrent;
                    view.move(panDelta);
                    panStart = window.mapPixelToCoords(mousePixel); // update for smooth panning
                }
            }
        }

        // Calculate time since last frame
        sf::Time deltaTime = clock.restart();

        // Handle player movement based on keyboard input (optional, can be removed if only mouse movement is desired)
        sf::Vector2f playerMovement(0.0f, 0.0f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            playerMovement.y -= PLAYER_SPEED * deltaTime.asSeconds();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            playerMovement.y += PLAYER_SPEED * deltaTime.asSeconds();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            playerMovement.x -= PLAYER_SPEED * deltaTime.asSeconds();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            playerMovement.x += PLAYER_SPEED * deltaTime.asSeconds();
        }

        // Move the player sprite (keyboard)
        playerSprite.move(playerMovement);

        // Center the view on the player sprite only if not panning or dragging
        if (!panning && !(dragging && playerSelected)) {
            view.setCenter(playerSprite.getPosition());
        }

        // Update selection border position and size if selected
        if (playerSelected) {
            selectionBorder.setPosition(playerSprite.getPosition());
            selectionBorder.setSize(sf::Vector2f(playerSprite.getGlobalBounds().width, playerSprite.getGlobalBounds().height));
            selectionBorder.setOrigin(playerSprite.getOrigin());
        }

        // Clear the window with black color
        window.clear(sf::Color::Black);

        // Apply the view before drawing anything
        window.setView(view);

        // Draw the map
        window.draw(mapSprite);

        // Draw the player
        window.draw(playerSprite);

        // Draw the selection border if selected
        if (playerSelected) {
            window.draw(selectionBorder);
        }

        // Display the contents of the window
        window.display();
    }

    return 0;
}
