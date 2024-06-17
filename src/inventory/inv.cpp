#include "inv.hpp"
#include <fmt/core.h>
#include <iostream>
#include <fstream>
#include "../game/game.hpp"

Inventory::Inventory(sf::Font& font, Game* gameInstance)
        : font(font),
          game(gameInstance),
          selectedItemIndex(-1),
          heldItemIndex(-1),
          backpackOpen(false),
          isDragging(false),
          isBackpackAv(false) {

    for (auto & i : hotbar) {
        i.setSize(sf::Vector2f(80.0f, 80.0f));
        i.setFillColor(sf::Color(150, 92, 48, 100));
        i.setOutlineColor(sf::Color(229, 169, 123, 200));
        i.setOutlineThickness(5.0f);
    }
    for (auto & i : backpack) {
        i.setSize(sf::Vector2f(80.0f, 80.0f));
        i.setFillColor(sf::Color(150, 92, 48, 100));
        i.setOutlineColor(sf::Color(229, 169, 123, 200));
        i.setOutlineThickness(3.0f);
    }

    InvItems.resize(hotbarSize);

    selectedItemBorder.setSize(sf::Vector2f(80.0f, 80.0f));
    selectedItemBorder.setFillColor(sf::Color::Transparent);
    selectedItemBorder.setOutlineColor(sf::Color::Yellow);
    selectedItemBorder.setOutlineThickness(5.0f);

    heldItemBorder.setSize(sf::Vector2f(80.0f, 80.0f));
    heldItemBorder.setFillColor(sf::Color::Transparent);
    heldItemBorder.setOutlineColor(sf::Color::Green);
    heldItemBorder.setOutlineThickness(5.0f);

    itemNameText.setFont(font);
    itemNameText.setStyle(sf::Text::Bold);
    itemNameText.setCharacterSize(26);
    itemNameText.setFillColor(sf::Color::White);

    itemDescriptionText.setFont(font);
    itemDescriptionText.setCharacterSize(18);
    itemDescriptionText.setFillColor(sf::Color::White);

    descriptionBackground.setFillColor(sf::Color(0, 0, 0, 150));
}

bool Inventory::addItem(const Item& item) {
    for (auto& slot : InvItems) {
        if (slot && slot->canStackWith(item)) {
            slot->quantity += item.quantity;
            return true;
        }
    }

    for (auto& slot : InvItems) {
        if (!slot || slot->isEmpty()) {
            slot = std::make_shared<Item>(item);
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
        if (InvItems[i] && !InvItems[i]->isEmpty()) {
            InvItems[i]->sprite.setPosition(hotbar[i].getPosition());
            window.draw(InvItems[i]->sprite);

            if (InvItems[i]->quantity > 1) {
                sf::Text quantityText;
                quantityText.setFont(font);
                quantityText.setString(std::to_string(InvItems[i]->quantity));
                quantityText.setCharacterSize(25);
                quantityText.setFillColor(sf::Color(228, 222, 106));

                sf::Text digitText("0", font, 25);
                float digitWidth = digitText.getGlobalBounds().width;

                float offsetX = -2.0f;
                if (InvItems[i]->quantity > 9 && InvItems[i]->quantity < 100) {
                    offsetX = digitWidth + 1;
                } else if (InvItems[i]->quantity >= 100) {
                    offsetX = 2 * digitWidth + 4;
                }

                quantityText.setPosition(hotbar[i].getPosition().x + 60 - offsetX, hotbar[i].getPosition().y + 52);
                window.draw(quantityText);
            }
        }
    }

    if (backpackOpen) {
        for (int i = 0; i < backpackSize; i++) {
            backpack[i].setPosition(inventoryBasePosition.x + (i % 3) * size, inventoryBasePosition.y + (i / 3) * size);
            window.draw(backpack[i]);
            if (InvItems[i + hotbarSize] && !InvItems[i + hotbarSize]->isEmpty()) {
                InvItems[i + hotbarSize]->sprite.setPosition(backpack[i].getPosition());
                window.draw(InvItems[i + hotbarSize]->sprite);

                if (InvItems[i + hotbarSize]->quantity > 1) {
                    sf::Text quantityText;
                    quantityText.setFont(font);
                    quantityText.setString(std::to_string(InvItems[i + hotbarSize]->quantity));
                    quantityText.setCharacterSize(25);
                    quantityText.setFillColor(sf::Color(228, 222, 106));

                    sf::Text digitText("0", font, 25);
                    float digitWidth = digitText.getGlobalBounds().width;

                    float offsetX = -2.0f;
                    if (InvItems[i + hotbarSize]->quantity > 9 && InvItems[i + hotbarSize]->quantity < 100) {
                        offsetX = digitWidth + 1;
                    } else if (InvItems[i + hotbarSize]->quantity >= 100) {
                        offsetX = 2 * digitWidth + 4;
                    }

                    quantityText.setPosition(backpack[i].getPosition().x + 60 - offsetX, backpack[i].getPosition().y + 52);
                    window.draw(quantityText);
                }
            }
        }
    }

    if (selectedItemIndex != -1) {
        selectedItemBorder.setPosition(InvItems[selectedItemIndex]->sprite.getPosition());
        window.draw(selectedItemBorder);

        const Item& selectedItem = *InvItems[selectedItemIndex];
        itemNameText.setString(selectedItem.name);
        itemDescriptionText.setString(selectedItem.description);

        sf::FloatRect nameBounds = itemNameText.getGlobalBounds();
        sf::FloatRect descriptionBounds = itemDescriptionText.getGlobalBounds();

        float rectWidth = std::max(nameBounds.width, descriptionBounds.width) + margin;
        float rectHeight = nameBounds.height + descriptionBounds.height + margin * 3;

        descriptionBackground.setSize(sf::Vector2f(rectWidth, rectHeight));

        sf::Vector2f slotPosition = InvItems[selectedItemIndex]->sprite.getPosition();
        descriptionBackground.setPosition(slotPosition.x + (size - rectWidth - margin) / 2.0f, slotPosition.y - rectHeight);
        window.draw(descriptionBackground);

        itemNameText.setPosition(
                descriptionBackground.getPosition().x + (rectWidth - nameBounds.width) / 2.0f,
                descriptionBackground.getPosition().y + margin / 2
        );
        window.draw(itemNameText);

        itemDescriptionText.setPosition(
                descriptionBackground.getPosition().x + (rectWidth - descriptionBounds.width) / 2.0f,
                itemNameText.getPosition().y + nameBounds.height + margin
        );
        window.draw(itemDescriptionText);
    }

    if (heldItemIndex != -1) {
        heldItemBorder.setPosition(InvItems[heldItemIndex]->sprite.getPosition());
        window.draw(heldItemBorder);
    }

    if (isDragging) {
        window.draw(draggedItem);
    }
}

void Inventory::handleMouseWheel(float delta) {
    if (delta > 0) {
        heldItemIndex = (heldItemIndex + 1) % hotbarSize;
    } else if (delta < 0) {
        heldItemIndex = (heldItemIndex - 1 + hotbarSize) % hotbarSize;
    }

    if (heldItemIndex >= InvItems.size() || !InvItems[heldItemIndex] || InvItems[heldItemIndex]->isEmpty()) {
        heldItemIndex = -1;
        holdItem(-1);
    } else {
        heldItem = InvItems[heldItemIndex];
    }
}

void Inventory::handleLeftMouseClick(sf::Vector2i mousePos) {
    sf::Vector2f mouseWorldPos(mousePos.x, mousePos.y);

    bool foundItem = false;
    for (int i = 0; i < hotbarSize; i++) {
        if (hotbar[i].getGlobalBounds().contains(mouseWorldPos) && InvItems[i] && !InvItems[i]->isEmpty()) {
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
            if (backpack[i].getGlobalBounds().contains(mouseWorldPos) && InvItems[i + hotbarSize] && !InvItems[i + hotbarSize]->isEmpty()) {
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

void Inventory::handleRightMouseClick(sf::Vector2i mousePos) {
    sf::Vector2f mouseWorldPos(mousePos.x, mousePos.y);

    for (int i = 0; i < hotbarSize; i++) {
        if (hotbar[i].getGlobalBounds().contains(mouseWorldPos) && InvItems[i] && !InvItems[i]->isEmpty()) {
            splitItem(i);
            return;
        }
    }

    if (backpackOpen) {
        for (int i = 0; i < backpackSize; i++) {
            if (backpack[i].getGlobalBounds().contains(mouseWorldPos) && InvItems[i + hotbarSize] && !InvItems[i + hotbarSize]->isEmpty()) {
                splitItem(i + hotbarSize);
                return;
            }
        }
    }
}

void Inventory::splitItem(int index) {
    if (InvItems[index]->quantity > 1 && InvItems[index]->stackable) {
        int halfQuantity = (InvItems[index]->quantity + 1) / 2;
        InvItems[index]->quantity -= halfQuantity;

        for (auto& slot : InvItems) {
            if (!slot || slot->isEmpty()) {
                slot = std::make_shared<Item>(*InvItems[index]);
                slot->quantity = halfQuantity;
                return;
            }
        }

        InvItems[index]->quantity += halfQuantity;
    }
}

void Inventory::handleMouseDrag(sf::Vector2i mousePos, bool isMousePressed, sf::Vector2f playerPosition) {
    if (isMousePressed) {
        if (!isDragging && selectedItemIndex != -1 && InvItems[selectedItemIndex] && !InvItems[selectedItemIndex]->isEmpty()) {
            isDragging = true;
            draggedItem = InvItems[selectedItemIndex]->sprite;
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
                if (targetIndex != selectedItemIndex) {
                    if (!InvItems[targetIndex] || InvItems[targetIndex]->isEmpty()) {
                        InvItems[targetIndex] = InvItems[selectedItemIndex];
                        InvItems[selectedItemIndex] = std::make_shared<Item>();
                    } else if (InvItems[targetIndex]->canStackWith(*InvItems[selectedItemIndex])) {
                        InvItems[targetIndex]->quantity += InvItems[selectedItemIndex]->quantity;
                        InvItems[selectedItemIndex] = std::make_shared<Item>();
                    } else {
                        std::swap(InvItems[selectedItemIndex], InvItems[targetIndex]);
                    }
                }
            } else {
                dropItem(selectedItemIndex, playerPosition);
            }

            selectedItemIndex = -1;
            isDragging = false;
        }
    }
}

bool Inventory::dropItem(int index, sf::Vector2f playerPosition) {
    if (index < InvItems.size() && InvItems[index] && !InvItems[index]->isEmpty()) {
        InvItems[index]->sprite.setPosition(playerPosition);
        InvItems[index]->pickedUp = false;
        if (game) {
            game->MapItems.emplace_back(*InvItems[index]);
        }
        InvItems[index] = std::make_shared<Item>();
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

void Inventory::setBackpackAvailable(bool available) {
    isBackpackAv = available;
    if (available) {
        InvItems.resize(hotbarSize + backpackSize);
    } else {
        InvItems.resize(hotbarSize);
        backpackOpen = false;
    }
}

void Inventory::toggleBackpack() {
    if (isBackpackAv) {
        backpackOpen = !backpackOpen;
    }
}

bool Inventory::isBackpackOpen() const {
    return backpackOpen;
}

bool Inventory::isBackpackAvailable() const {
    return isBackpackAv;
}

void Inventory::holdItem(int index) {
    if (index >= 0 && index < hotbarSize && index < InvItems.size() && InvItems[index] && !InvItems[index]->isEmpty()) {
        heldItemIndex = index;
        heldItem = InvItems[heldItemIndex];
    } else {
        heldItemIndex = -1;
        heldItem.reset();
    }
}

Item* Inventory::getHeldItem() {
    return heldItem.get();
}

int Inventory::getTotalQuantity(const std::string& itemName) const {
    int totalQuantity = 0;
    for (const auto& itemPtr : InvItems) {
        if (itemPtr && itemPtr->name == itemName) {
            totalQuantity += itemPtr->quantity;
        }
    }
    return totalQuantity;
}

bool Inventory::decreaseQuantity(const std::string& itemName) {
    for (auto& itemPtr : InvItems) {
        if (itemPtr && itemPtr->name == itemName) {
            itemPtr->quantity -= 1;
            if (itemPtr->quantity <= 0) {
                itemPtr = std::make_shared<Item>();
            }
            return true;
        }
    }
    return false;
}

void Inventory::removeAllItems() {
    for (auto& item : InvItems) {
        item = std::make_shared<Item>();
    }
}

void Inventory::saveInventory(const std::string& filePath, int& currentLevel, bool& gameWon, std::vector<Item>& Items) {
    std::ofstream outFile(filePath);
    if (outFile.is_open()) {

        outFile << currentLevel << "\n";
        outFile << gameWon << "\n";
        outFile << isBackpackAv << "\n";

        for (const auto& itemPtr : InvItems) {
            if (itemPtr) {
                outFile << itemPtr->name << "\n";
                outFile << itemPtr->quantity << "\n";
            } else {
                outFile << "empty\n";
            }
        }
        outFile.close();
    } else {
        std::cerr << "Unable to open file for saving inventory\n";
    }
}

void Inventory::loadInventory(const std::string& filePath, int& currentLevel, bool& gameWon, std::vector<Item>& Items) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        std::ofstream outFile(filePath);
        if (outFile.is_open()) {
            currentLevel = 0;
            gameWon = false;
            isBackpackAv = false;

            outFile << currentLevel << "\n";
            outFile << gameWon << "\n";
            outFile << isBackpackAv << "\n";

            for (size_t i = 0; i < InvItems.size(); ++i) {
                outFile << "empty\n";
            }

            outFile.close();
        }
        return;
    } else {
        std::string line;

        std::getline(inFile, line);
        currentLevel = std::stoi(line);
        std::getline(inFile, line);
        gameWon = static_cast<bool>(std::stoi(line));
        std::getline(inFile, line);
        isBackpackAv = static_cast<bool>(std::stoi(line));
        if (isBackpackAv) InvItems.resize(hotbarSize + backpackSize);

        int index = 0;
        while (std::getline(inFile, line) && index < InvItems.size()) {
            if (line == "empty") {
                InvItems[index] = std::make_shared<Item>();
            } else {
                std::string itemName = line;
                std::getline(inFile, line);
                int quantity = std::stoi(line);

                auto it = std::find_if(Items.begin(), Items.end(), [&itemName](const Item& item) {
                    return item.name == itemName;
                });

                if (it != Items.end()) {
                    auto itemPtr = std::make_shared<Item>(*it);
                    itemPtr->quantity = quantity;
                    InvItems[index] = itemPtr;
                } else {
                    InvItems[index] = std::make_shared<Item>();
                }
            }
            ++index;
        }
        inFile.close();
    }
}