#ifndef LEV_HPP
#define LEV_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Tile {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Vector2f position;
    bool isInteractive;
    std::vector<sf::RectangleShape> boundaries;

    Tile(const sf::Vector2f& pos, const std::string& textureFile, bool interactive = false);
    void addBoundary(const sf::FloatRect& rect);
};

class Level {
public:
    std::vector<std::vector<Tile*>> tiles;
    int rows;
    int cols;

    Level(int r, int c);
    ~Level();

    void setTile(int row, int col, Tile* tile);
    void draw(sf::RenderWindow& window);
    bool checkCollision(const sf::FloatRect& playerBounds);
};

#endif // LEV_HPP