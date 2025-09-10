#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Map {
public:
    Map(const std::string& textureFile);
    void draw(sf::RenderWindow& window) const;
    float getWidth() const;
    float getHeight() const;
    sf::Sprite& getSprite();
private:
    sf::Texture texture;
    sf::Sprite sprite;
};