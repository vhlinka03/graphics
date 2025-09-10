#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Spirit {
public:
    Spirit(const std::string& textureFile, float scale = 1.0f);
    void setPosition(float x, float y);
    void move(const sf::Vector2f& offset);
    void draw(sf::RenderWindow& window) const;
    sf::Vector2f getPosition() const;
    sf::FloatRect getGlobalBounds() const;
    void setSelected(bool selected);
    bool isSelected() const;
    void updateSelectionBorder();
    void setDragOffset(const sf::Vector2f& offset);
    sf::Vector2f getDragOffset() const;
    sf::Sprite& getSprite();
    void setBorderColor(const sf::Color& color); // Add this method
private:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::RectangleShape selectionBorder;
    bool selected = false;
    sf::Vector2f dragOffset;
};