#include "lev.hpp"
#include <fmt/core.h>
#include <iostream>

Tile::Tile(const sf::Vector2f& pos, const std::string& textureFile, bool interactive)
        : position(pos), isInteractive(interactive) {
    if (texture.loadFromFile(textureFile)) {
        sprite.setTexture(texture);
        sprite.setPosition(position);
        sprite.setScale(5.0f, 5.0f);
    } else {
        fmt::println("Failed to load texture: ");
    }
}

void Tile::addBoundary(const sf::FloatRect& rect) {
    sf::RectangleShape boundary;
    boundary.setPosition(rect.left, rect.top);
    boundary.setSize(sf::Vector2f(rect.width, rect.height));
    boundary.setFillColor(sf::Color::Red);
    boundaries.push_back(boundary);
}

Level::Level(int r, int c)
        : rows(r), cols(c) {
    tiles.resize(rows, std::vector<Tile*>(cols, nullptr));
}

Level::~Level() = default;

void Level::setTile(int row, int col, Tile* tile) {
    if (row < rows && col < cols) {
        tiles[row][col] = tile;
    }
}

void Level::draw(sf::RenderWindow& window) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (tiles[i][j] != nullptr) {
                window.draw(tiles[i][j]->sprite);
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
