#ifndef ITEM_HPP
#define ITEM_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <iostream>


struct Item {
public:
    bool pickedUp{};
    bool stackable{};
    int quantity;
    sf::Sprite sprite;
    std::string name;
    std::string description;

    // Empty
    Item() : quantity(1) {}

    // Non-stackable
    Item(sf::Sprite sprite, std::string name, std::string desc)
            : sprite(std::move(sprite)), name(std::move(name)), description(std::move(desc)), quantity(1) {}
    // Stackable
    Item(sf::Sprite sprite, std::string name, std::string desc, int quantity)
            : sprite(std::move(sprite)), name(std::move(name)), description(std::move(desc)), quantity(quantity), stackable(true) {}

    bool isEmpty() const {
        return sprite.getTexture() == nullptr;
    }
    bool canStackWith(const Item& other) const {
        return stackable && name == other.name;
    }
    virtual ~Item() = default;
};

#endif