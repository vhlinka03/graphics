#include "Map.h"
#include <iostream>

Map::Map(const std::string& textureFile) {
    if (!texture.loadFromFile(textureFile)) {
        std::cerr << "Error: Could not load " << textureFile << std::endl;
    }
    sprite.setTexture(texture);
}

void Map::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}

float Map::getWidth() const {
    return sprite.getLocalBounds().width;
}

float Map::getHeight() const {
    return sprite.getLocalBounds().height;
}

sf::Sprite& Map::getSprite() {
    return sprite;
}