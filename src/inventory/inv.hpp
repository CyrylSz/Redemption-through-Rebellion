#ifndef INV_HPP
#define INV_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class Inventory {

    public:
        Inventory(sf::Font& font);
        void draw(sf::RenderWindow& window);
        void toggleInventory();
        bool addItem(const sf::Sprite& item);
        bool removeItem(int index, sf::Vector2f position);
        void moveItem(int fromIndex, int toIndex);

    private:
        sf::Font& font;
        std::vector<sf::Sprite> items;
        bool inventoryOpen;
        static const int hotbarSize = 3;
        static const int inventorySize = 9;
        sf::RectangleShape hotbar[hotbarSize];
        sf::RectangleShape inventory[inventorySize];
};

struct Item {
    sf::Sprite sprite;
    bool pickedUp{};
};

#endif