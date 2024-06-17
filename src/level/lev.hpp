#ifndef LEV_HPP
#define LEV_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "door.hpp"

class Tile {
public:
    sf::Vector2f position;
    std::vector<sf::RectangleShape> boundaries;

    explicit Tile(const sf::Vector2f& pos);
    void addBoundary(const sf::FloatRect& rect);
};

class Level {
public:
    sf::Sprite sprite;
    std::vector<std::vector<Tile*>> tiles;
    std::vector<Door> doors;
    int rows;
    int cols;

    Level(int r, int c, sf::Sprite& textureFile);

    void setTile(int row, int col, Tile* tile);
    void draw(sf::RenderWindow& window);
    bool checkCollision(const sf::FloatRect& playerBounds);
};

#endif