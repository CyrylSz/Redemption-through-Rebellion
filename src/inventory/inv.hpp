#ifndef INV_HPP
#define INV_HPP

#include <SFML/Graphics.hpp>
#include <utility>
#include <vector>

struct Item {
    sf::Sprite sprite;
    bool pickedUp{};
    std::string name;
    std::string description;

    Item() : pickedUp(false) {}

    Item(sf::Sprite  spr, bool picked, std::string  n, std::string  desc)
            : sprite(std::move(spr)), pickedUp(picked), name(std::move(n)), description(std::move(desc)) {}

    bool isEmpty() const {
        return sprite.getTexture() == nullptr;
    }
};

class Game;

class Inventory {
public:
    explicit Inventory(sf::Font& font, Game* gameInstance);
    void draw(sf::RenderWindow& window);
    void toggleBackpack();
    bool addItem(const sf::Sprite& item, const std::string& name, const std::string& description);
    bool removeItem(int index, sf::Vector2f playerPosition);
    void handleMouseClick(sf::Vector2i mousePos);
    void handleMouseDrag(sf::Vector2i mousePos, bool isMousePressed, sf::Vector2f playerPosition);
    void clearSelection();
    bool isBackpackOpen() const;
    void holdItem(int index);
    void handleMouseWheel(float delta);

private:
    sf::Font& font;
    std::vector<Item> InvItems;
    bool backpackOpen;
    static const int hotbarSize = 3;
    static const int backpackSize = 9;
    sf::RectangleShape hotbar[hotbarSize];
    sf::RectangleShape backpack[backpackSize];

    int selectedItemIndex;
    int heldItemIndex;
    sf::RectangleShape selectedItemBorder;
    sf::RectangleShape heldItemBorder;
    sf::Text itemNameText;
    sf::Text itemDescriptionText;
    sf::RectangleShape descriptionBackground;

    bool isDragging;
    sf::Sprite draggedItem;
    sf::Vector2i dragOffset;

    Game* game;
};

#endif
