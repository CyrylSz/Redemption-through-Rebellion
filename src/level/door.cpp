#include "door.hpp"
#include <cmath>

Door::Door(const sf::Vector2f& pos, const sf::Texture& tex, const std::string& dir)
        : position(pos), sprite(tex), angle(0.f), rotationSpeed(280.f), direction(dir), collisionSize(28.f * 5.f, 4.f * 5.f) { // Zmodyfikowane

    sprite.setPosition(position);
    sprite.setOrigin(3.f, 5.f);
    sprite.setScale(5.0f, 5.0f);

    setInitialRotation(dir);
    updateBoundary();
}

void Door::setInitialRotation(const std::string& dir) {
    if (dir == "S") {
        sprite.setRotation(90);
    } else if (dir == "W") {
        sprite.setRotation(180);
    } else if (dir == "N") {
        sprite.setRotation(270);
    } else {
        sprite.setRotation(0);
    }
}

void Door::updateBoundary() {
    float halfWidth = collisionSize.x / 2.f;
    float halfHeight = collisionSize.y / 2.f;

    sf::Vector2f corner = sprite.getPosition() - sf::Vector2f(0, halfHeight);
    sf::FloatRect rotatedBoundary = sf::FloatRect(corner.x, corner.y, collisionSize.x, collisionSize.y);
    boundary = rotatedBoundary;

    sf::Transform transform;
    transform.rotate(sprite.getRotation(), sprite.getPosition());
    boundary = transform.transformRect(boundary);
}

sf::Vector2f Door::getDirectionVector(float angle) const {
    float rad = angle * 3.14159265f / 180.f;
    return sf::Vector2f(std::cos(rad), std::sin(rad));
}

void Door::update(const sf::FloatRect& playerBounds, sf::Time deltaTime) {
    if (boundary.intersects(playerBounds)) {
        sf::Vector2f playerCenter(playerBounds.left + playerBounds.width / 2.f, playerBounds.top + playerBounds.height / 2.f);
        sf::Vector2f doorCenter = sprite.getPosition();
        sf::Vector2f directionVector = playerCenter - doorCenter;

        float angleToPlayer = atan2(directionVector.y, directionVector.x) * 180.f / 3.14159265f;
        float doorAngle = sprite.getRotation();

        float rotationChange = 0.f;
        float angleDifference = angleToPlayer - doorAngle;
        if (angleDifference < 0) {
            angleDifference += 360.f;
        }

        if (angleDifference < 180.f) {
            rotationChange = -rotationSpeed * deltaTime.asSeconds();
        } else {
            rotationChange = rotationSpeed * deltaTime.asSeconds();
        }

        angle += rotationChange;

        if (angle > 90.f) angle = 90.f;
        if (angle < -90.f) angle = -90.f;

        float initialRotation = 0.f;
        if (direction == "S") {
            initialRotation = 90.f;
        } else if (direction == "W") {
            initialRotation = 180.f;
        } else if (direction == "N") {
            initialRotation = 270.f;
        }

        sprite.setRotation(initialRotation + angle);
        updateBoundary();
    }
}

void Door::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Door::getBounds() const {
    return boundary;
}
