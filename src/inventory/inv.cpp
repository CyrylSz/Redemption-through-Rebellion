#include "inv.hpp"
#include <fmt/core.h>
#include <iostream>
#include "../game/game.hpp"

Inventory::Inventory(sf::Font& font, Game* gameInstance) : font(font), game(gameInstance), selectedItemIndex(-1),
    heldItemIndex(-1), backpackOpen(false), isDragging(false) {
    for (int i = 0; i < hotbarSize; i++) {
        hotbar[i].setSize(sf::Vector2f(80.0f, 80.0f));
        hotbar[i].setFillColor(sf::Color::Transparent);
        hotbar[i].setOutlineColor(sf::Color::White);
        hotbar[i].setOutlineThickness(5.0f);
    }
    for (int i = 0; i < backpackSize; i++) {
        backpack[i].setSize(sf::Vector2f(80.0f, 80.0f));
        backpack[i].setFillColor(sf::Color::Transparent);
        backpack[i].setOutlineColor(sf::Color::White);
        backpack[i].setOutlineThickness(3.0f);
    }

    InvItems.resize(hotbarSize + backpackSize);

    selectedItemBorder.setSize(sf::Vector2f(80.0f, 80.0f));
    selectedItemBorder.setFillColor(sf::Color::Transparent);
    selectedItemBorder.setOutlineColor(sf::Color::Yellow);
    selectedItemBorder.setOutlineThickness(5.0f);

    heldItemBorder.setSize(sf::Vector2f(80.0f, 80.0f));
    heldItemBorder.setFillColor(sf::Color::Transparent);
    heldItemBorder.setOutlineColor(sf::Color::Green);
    heldItemBorder.setOutlineThickness(5.0f);

    itemNameText.setFont(font);
    itemNameText.setCharacterSize(14);
    itemNameText.setFillColor(sf::Color::White);

    itemDescriptionText.setFont(font);
    itemDescriptionText.setCharacterSize(12);
    itemDescriptionText.setFillColor(sf::Color::White);

    descriptionBackground.setFillColor(sf::Color(0, 0, 0, 150));
    descriptionBackground.setSize(sf::Vector2f(160.0f, 50.0f));
}

bool Inventory::addItem(const sf::Sprite& item, const std::string& name, const std::string& description) {
    if (item.getTexture() == nullptr) {
        fmt::println("Attempted to add an item with no texture!");
        return false;
    }

    for (auto& slot : InvItems) {
        if (slot.isEmpty()) {
            slot = Item(item, false, name, description);
            return true;
        }
    }
    return false;
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
        if (!InvItems[i].isEmpty()) {
            InvItems[i].sprite.setPosition(hotbar[i].getPosition());
            window.draw(InvItems[i].sprite);
        }
    }

    if (backpackOpen) {
        for (int i = 0; i < backpackSize; i++) {
            backpack[i].setPosition(inventoryBasePosition.x + (i % 3) * size, inventoryBasePosition.y + (i / 3) * size);
            window.draw(backpack[i]);
            if (!InvItems[i + hotbarSize].isEmpty()) {
                InvItems[i + hotbarSize].sprite.setPosition(backpack[i].getPosition());
                window.draw(InvItems[i + hotbarSize].sprite);
            }
        }
    }

    if (selectedItemIndex != -1) {
        selectedItemBorder.setPosition(InvItems[selectedItemIndex].sprite.getPosition());
        window.draw(selectedItemBorder);

        const Item& selectedItem = InvItems[selectedItemIndex];
        itemNameText.setString(selectedItem.name);
        itemDescriptionText.setString(selectedItem.description);

        descriptionBackground.setPosition(InvItems[selectedItemIndex].sprite.getPosition() - sf::Vector2f(40.0f, 50.0f));
        window.draw(descriptionBackground);

        itemNameText.setPosition(descriptionBackground.getPosition() + sf::Vector2f(10.0f, 5.0f));
        window.draw(itemNameText);

        itemDescriptionText.setPosition(descriptionBackground.getPosition() + sf::Vector2f(10.0f, 25.0f));
        window.draw(itemDescriptionText);
    }

    if (heldItemIndex != -1) {
        heldItemBorder.setPosition(InvItems[heldItemIndex].sprite.getPosition());
        window.draw(heldItemBorder);
    }

    if (isDragging) {
        window.draw(draggedItem);
    }
}

void Inventory::holdItem(int index) {
    if (index >= 0 && index < hotbarSize && index < InvItems.size() && !InvItems[index].isEmpty()) {
        heldItemIndex = index;
    } else {
        heldItemIndex = -1;
    }
}

void Inventory::handleMouseWheel(float delta) {
    if (delta > 0) {
        heldItemIndex = (heldItemIndex + 1) % hotbarSize;
    } else if (delta < 0) {
        heldItemIndex = (heldItemIndex - 1 + hotbarSize) % hotbarSize;
    }

    if (heldItemIndex >= InvItems.size() || InvItems[heldItemIndex].isEmpty()) {
        heldItemIndex = -1;
    }
}

void Inventory::toggleBackpack() {
    backpackOpen = !backpackOpen;
}

void Inventory::handleMouseClick(sf::Vector2i mousePos) {
    sf::Vector2f mouseWorldPos(mousePos.x, mousePos.y);

    bool foundItem = false;
    for (int i = 0; i < hotbarSize; i++) {
        if (hotbar[i].getGlobalBounds().contains(mouseWorldPos) && !InvItems[i].isEmpty()) {
            if (selectedItemIndex == i) {
                selectedItemIndex = -1;
            } else {
                selectedItemIndex = i;
            }
            foundItem = true;
            break;
        }
    }

    if (!foundItem && backpackOpen) {
        for (int i = 0; i < backpackSize; i++) {
            if (backpack[i].getGlobalBounds().contains(mouseWorldPos) && !InvItems[i + hotbarSize].isEmpty()) {
                if (selectedItemIndex == i + hotbarSize) {
                    selectedItemIndex = -1;
                } else {
                    selectedItemIndex = i + hotbarSize;
                }
                foundItem = true;
                break;
            }
        }
    }

    if (!foundItem) {
        selectedItemIndex = -1;
    }
}

void Inventory::handleMouseDrag(sf::Vector2i mousePos, bool isMousePressed, sf::Vector2f playerPosition) {
    if (isMousePressed) {
        if (!isDragging && selectedItemIndex != -1 && !InvItems[selectedItemIndex].isEmpty()) {
            isDragging = true;
            draggedItem = InvItems[selectedItemIndex].sprite;
            dragOffset = sf::Vector2i(mousePos) - sf::Vector2i(draggedItem.getPosition());
        }
        if (isDragging) {
            draggedItem.setPosition(mousePos.x - dragOffset.x, mousePos.y - dragOffset.y);
        }
    } else {
        if (isDragging) {
            sf::Vector2f dropPosition(mousePos.x, mousePos.y);
            bool placed = false;
            int targetIndex = -1;

            for (int i = 0; i < hotbarSize; i++) {
                if (hotbar[i].getGlobalBounds().contains(dropPosition)) {
                    targetIndex = i;
                    placed = true;
                    break;
                }
            }

            if (!placed && backpackOpen) {
                for (int i = 0; i < backpackSize; i++) {
                    if (backpack[i].getGlobalBounds().contains(dropPosition)) {
                        targetIndex = i + hotbarSize;
                        placed = true;
                        break;
                    }
                }
            }

            if (placed) {
                if (InvItems[targetIndex].isEmpty()) {
                    InvItems[targetIndex] = InvItems[selectedItemIndex];
                    InvItems[selectedItemIndex] = Item();
                } else {
                    std::swap(InvItems[selectedItemIndex], InvItems[targetIndex]);
                }
            } else {
                removeItem(selectedItemIndex, playerPosition);
            }

            selectedItemIndex = -1;
            isDragging = false;
        }
    }
}

bool Inventory::removeItem(int index, sf::Vector2f playerPosition) {
    if (index < InvItems.size() && !InvItems[index].isEmpty()) {
        InvItems[index].sprite.setPosition(playerPosition);
        InvItems[index].pickedUp = false;
        if (game) {
            game->MapItems.emplace_back(InvItems[index]);
        }
        InvItems[index] = Item();
        if (index == selectedItemIndex) {
            selectedItemIndex = -1;
        }
        if (index == heldItemIndex) {
            heldItemIndex = -1;
        }
        return true;
    }
    return false;
}

void Inventory::clearSelection() {
    selectedItemIndex = -1;
}

bool Inventory::isBackpackOpen() const {
    return backpackOpen;
}
