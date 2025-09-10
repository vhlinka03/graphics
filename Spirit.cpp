#include "Spirit.h"
#include <iostream>

Spirit::Spirit(const std::string& textureFile, float scale) {
    if (!texture.loadFromFile(textureFile)) {
        std::cerr << "Error: Could not load " << textureFile << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setScale(scale, scale);

    // Set origin to center for both sprite and border
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    selectionBorder.setFillColor(sf::Color::Transparent);
    selectionBorder.setOutlineColor(sf::Color::Red);
    selectionBorder.setOutlineThickness(3.0f);

    updateSelectionBorder();
}

void Spirit::setPosition(float x, float y) {
    sprite.setPosition(x, y);
    updateSelectionBorder();
}

void Spirit::move(const sf::Vector2f& offset) {
    sprite.move(offset);
    updateSelectionBorder();
}

void Spirit::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
    if (selected) {
        window.draw(selectionBorder);
    }
}

sf::Vector2f Spirit::getPosition() const {
    return sprite.getPosition();
}

sf::FloatRect Spirit::getGlobalBounds() const {
    return sprite.getGlobalBounds();
}

void Spirit::setSelected(bool sel) {
    selected = sel;
}

bool Spirit::isSelected() const {
    return selected;
}

void Spirit::updateSelectionBorder() {
    // Always update after sprite position/scale/origin changes
    sf::FloatRect bounds = sprite.getGlobalBounds();
    selectionBorder.setSize(sf::Vector2f(bounds.width, bounds.height));
    selectionBorder.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    selectionBorder.setPosition(sprite.getPosition());
}

void Spirit::setDragOffset(const sf::Vector2f& offset) {
    dragOffset = offset;
}

sf::Vector2f Spirit::getDragOffset() const {
    return dragOffset;
}

sf::Sprite& Spirit::getSprite() {
    return sprite;
}

void Spirit::setBorderColor(const sf::Color& color) {
    selectionBorder.setOutlineColor(color);
}