#include "lev.hpp"
#include <fmt/core.h>

Tile::Tile(const sf::Vector2f& pos)
        : position(pos) {}

void Tile::addBoundary(const sf::FloatRect& rect) {
    sf::RectangleShape boundary;
    boundary.setPosition(rect.left*5, rect.top*5);
    boundary.setSize(sf::Vector2f(rect.width*5, rect.height*5));
    boundary.setFillColor(sf::Color::Transparent);
    boundaries.push_back(boundary);
}

Level::Level(int r, int c, sf::Sprite& textureSprite)
        : rows(r), cols(c) {
    tiles.resize(rows, std::vector<Tile*>(cols, nullptr));
    sprite = textureSprite;

}

void Level::setTile(int row, int col, Tile* tile) {
    if (row < rows && col < cols) {
        tiles[row][col] = tile;
    }
}

void Level::draw(sf::RenderWindow& window) {
    window.draw(sprite);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (tiles[i][j] != nullptr) {
                for (const auto& boundary : tiles[i][j]->boundaries) {
                    window.draw(boundary);
                }
            }
        }
    }
}

bool Level::checkCollision(const sf::FloatRect& playerBounds) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (tiles[i][j] != nullptr) {
                for (const auto& boundary : tiles[i][j]->boundaries) {
                    if (boundary.getGlobalBounds().intersects(playerBounds)) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}
