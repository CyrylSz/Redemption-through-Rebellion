#include "inv.hpp"
#include <fmt/core.h>
#include <iostream>

Inventory::Inventory(sf::Font& font) : font(font), inventoryOpen(false) {
    for (int i = 0; i < hotbarSize; i++) {
        hotbar[i].setSize(sf::Vector2f(80.0f, 80.0f));
        hotbar[i].setFillColor(sf::Color::Transparent);
        hotbar[i].setOutlineColor(sf::Color::White);
        hotbar[i].setOutlineThickness(5.0f);
    }
    for (int i = 0; i < inventorySize; i++) {
        inventory[i].setSize(sf::Vector2f(80.0f, 80.0f));
        inventory[i].setFillColor(sf::Color::Transparent);
        inventory[i].setOutlineColor(sf::Color::White);
        inventory[i].setOutlineThickness(3.0f);
    }
}

void Inventory::draw(sf::RenderWindow& window) {
    float size = 95.0f;

    float windowWidth = window.getSize().x;
    float hotbarWidth = hotbarSize * size - 10.0f;
    float inventoryWidth = 3 * size - 10.0f;

    float hotbarBaseX = (windowWidth - hotbarWidth) / 2.0f;
    sf::Vector2f hotbarBasePosition(hotbarBaseX, 950.0f);

    float inventoryBaseX = (windowWidth - inventoryWidth) / 2.0f;
    sf::Vector2f inventoryBasePosition(inventoryBaseX, 650.0f);

    for (int i = 0; i < hotbarSize; i++) {
        hotbar[i].setPosition(hotbarBasePosition.x + i * size, hotbarBasePosition.y);
        window.draw(hotbar[i]);
        if (i < items.size()) {
            items[i].setPosition(hotbar[i].getPosition());
            window.draw(items[i]);
        }
    }

    if (inventoryOpen) {
        for (int i = 0; i < inventorySize; i++) {
            inventory[i].setPosition(inventoryBasePosition.x + (i % 3) * size, inventoryBasePosition.y + (i / 3) * size);
            window.draw(inventory[i]);
            if (i + hotbarSize < items.size()) {
                items[i + hotbarSize].setPosition(inventory[i].getPosition());
                window.draw(items[i + hotbarSize]);
            }
        }
    }
}

void Inventory::toggleInventory() {
    inventoryOpen = !inventoryOpen;
}

bool Inventory::addItem(const sf::Sprite& item) {
    if (item.getTexture() == nullptr) {
        fmt::println("Attempted to add an item with no texture!");
        return false;
    }

    if (items.size() < hotbarSize + inventorySize) {
        items.push_back(item);
        return true;
    }
    return false;
}


bool Inventory::removeItem(int index, sf::Vector2f position) {
    if (index < items.size()) {
        items.erase(items.begin() + index);
        return true;
    }
    return false;
}

void Inventory::moveItem(int fromIndex, int toIndex) {
    if (fromIndex < items.size() && toIndex < items.size()) {
        std::swap(items[fromIndex], items[toIndex]);
    }
}
