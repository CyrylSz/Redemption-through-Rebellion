#ifndef BULLET_HPP
#define BULLET_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cmath>

const float BULLET_SPEED = 1500.0f;
const float BULLET_LIFETIME = 3.0f;

class Bullet {
public:
    Bullet(sf::Texture& texture, sf::Vector2f position, sf::Vector2f direction)
        : velocity(direction * BULLET_SPEED), lifetime(sf::seconds(BULLET_LIFETIME)) {

            sprite.setTexture(texture);
            sprite.setPosition(position);
            sprite.setScale(10,10);
            double angle = std::atan2(direction.y, direction.x) * 180 / 3.14159265;
            sprite.setRotation((float)angle - 90);
    }

    void update(sf::Time deltaTime) {
        sprite.move(velocity * deltaTime.asSeconds());
        lifetime -= deltaTime;
    }
    bool isExpired() {
        return lifetime <= sf::Time::Zero;
    }
    sf::Sprite& getSprite() {
        return sprite;
    }

private:
    sf::Sprite sprite;
    sf::Vector2f velocity;
    sf::Time lifetime;
};

#endif

