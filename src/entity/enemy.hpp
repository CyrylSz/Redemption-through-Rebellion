#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <utility>
#include "../inventory/item.hpp"

class Enemy {
public:
    sf::Sprite sprite;
    sf::CircleShape visionRadius;
    sf::CircleShape attackRange;
    sf::CircleShape hitbox;
    sf::CircleShape boundary;

    std::string type;
    float visionRadiusSize;
    float attackRangeSize;
    bool isAlive;
    Item dropItem;
    sf::Clock attackClock;
    bool playerInAttackRange{};

    Enemy(sf::Sprite spr, std::string type, float visionRadius, float attackRange, float hitbox, const Item& item)
            : sprite(std::move(spr)), type(std::move(type)), visionRadiusSize(visionRadius), attackRangeSize(attackRange), dropItem(item), isAlive(true) {

        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2, bounds.height / 2);

        this->visionRadius.setRadius(visionRadius);
        this->visionRadius.setFillColor(sf::Color(0, 0, 255, 100));
        this->visionRadius.setOrigin(visionRadius, visionRadius);
        this->visionRadius.setPosition(sprite.getPosition());

        this->attackRange.setRadius(attackRange);
        this->attackRange.setFillColor(sf::Color(255, 0, 0, 100));
        this->attackRange.setOrigin(attackRange, attackRange);
        this->attackRange.setPosition(sprite.getPosition());

        this->hitbox.setRadius(hitbox);
        this->hitbox.setFillColor(sf::Color(0, 255, 0, 100));
        this->hitbox.setOrigin(hitbox, hitbox);
        this->hitbox.setPosition(sprite.getPosition());

        this->boundary.setRadius(attackRange / 2);
        this->boundary.setFillColor(sf::Color(0, 255, 255, 100));
        this->boundary.setOrigin(attackRange / 3, attackRange / 3);
        this->boundary.setPosition(sprite.getPosition());
    }

    void updatePosition(const sf::Vector2f& newPosition) {
        sprite.setPosition(newPosition);
        visionRadius.setPosition(newPosition);
        attackRange.setPosition(newPosition);
        hitbox.setPosition(newPosition);
        boundary.setPosition(newPosition);
    }
};

#endif
