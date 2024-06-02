#ifndef DOOR_HPP
#define DOOR_HPP

#include <SFML/Graphics.hpp>

class Door {
public:
    Door(const sf::Vector2f& pos, const sf::Texture& tex, const std::string& dir);

    void update(const sf::FloatRect& playerBounds, sf::Time deltaTime);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    sf::FloatRect boundary;

private:
    sf::Sprite sprite;
    sf::Vector2f position;
    float angle;
    float rotationSpeed;
    std::string direction;

    sf::Vector2f collisionSize;

    void setInitialRotation(const std::string& dir);
    void updateBoundary();
    sf::Vector2f getDirectionVector(float angle) const;
};

#endif
