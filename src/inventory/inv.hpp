#ifndef INV_HPP
#define INV_HPP

#include <SFML/Graphics.hpp>
#include <utility>
#include <vector>
#include "item.hpp"

class Game;

class Inventory {
public:
    explicit Inventory(sf::Font& font, Game* gameInstance);
    void draw(sf::RenderWindow& window);
    void toggleBackpack();
    bool addItem(const Item& item);
    bool dropItem(int index, sf::Vector2f playerPosition);
    void handleLeftMouseClick(sf::Vector2i mousePos);
    void handleRightMouseClick(sf::Vector2i mousePos);
    void handleMouseDrag(sf::Vector2i mousePos, bool isMousePressed, sf::Vector2f playerPosition);
    void clearSelection();
    bool isBackpackOpen() const;
    bool isBackpackAvailable() const;
    void holdItem(int index);
    void splitItem(int index);
    void handleMouseWheel(float delta);
    void setBackpackAvailable(bool available);
    Item* getHeldItem();
    int getTotalQuantity(const std::string& itemName) const;
    bool decreaseQuantity(const std::string& itemName);
    void removeAllItems();
    void saveInventory(const std::string& filePath, int& currentLevel, bool& gameWon, std::vector<Item>& Items);
    void loadInventory(const std::string& filePath, int& currentLevel, bool& gameWon, std::vector<Item>& Items);

private:
    sf::Font& font;
    bool backpackOpen;
    bool isBackpackAv;
    static const int hotbarSize = 3;
    static const int backpackSize = 9;
    sf::RectangleShape hotbar[hotbarSize];
    sf::RectangleShape backpack[backpackSize];

    int selectedItemIndex;
    int heldItemIndex;
    sf::RectangleShape selectedItemBorder;
    sf::RectangleShape heldItemBorder;
    static const int margin = 15;
    sf::Text itemNameText;
    sf::Text itemDescriptionText;
    sf::RectangleShape descriptionBackground;

    bool isDragging;
    sf::Sprite draggedItem;
    sf::Vector2i dragOffset;

    std::shared_ptr<Item> heldItem;
    std::vector<std::shared_ptr<Item>> InvItems;

    Game* game;
};

#endif
