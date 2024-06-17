#ifndef DOOR_HPP
#define DOOR_HPP

#include <SFML/Graphics.hpp>

class Door {
public:
    Door(const sf::Vector2f& pos, const sf::Texture& tex, const std::string& dir, short id);

    void update(const sf::FloatRect& playerBounds, sf::Time deltaTime);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    sf::FloatRect boundary;
    short id;

private:
    sf::Sprite sprite;
    sf::Vector2f position;
    float angle;
    float rotationSpeed;
    std::string direction;

    sf::Vector2f collisionSize;

    void setInitialRotation(const std::string& dir);
    void updateBoundary();
};

#endif
