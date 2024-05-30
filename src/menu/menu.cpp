#include "menu.hpp"
#include <fmt/core.h>

Button::Button(const sf::Vector2f& position)
        : callback(nullptr) {

    shape.setSize(sf::Vector2f(300, 75));
    shape.setFillColor(sf::Color::Black);
    shape.setPosition(position.x, position.y);
}


void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

bool Button::isHovered(const sf::Vector2f& mousePos) const {
    return shape.getGlobalBounds().contains(mousePos);
}

void Button::setCallback(const std::function<void()>& callbackFunc) {
    callback = callbackFunc;
}

void Button::onClick() const {
    if (callback) {
        callback();
    }
}

Menu::Menu(sf::RenderWindow& window, const sf::Font& font, const std::string& backgroundPath)
        : window(window), font(font), buttonYPosition(window.getSize().y / 2.0f + 100.0f) {

    if (!backgroundTexture.loadFromFile(backgroundPath)) {
        fmt::println("Error while loading background");
    }
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(
            window.getSize().x / backgroundSprite.getLocalBounds().width,
            window.getSize().y / backgroundSprite.getLocalBounds().height
    );

    title1.setFont(font);
    title1.setString("Redemption");
    title1.setCharacterSize(100);
    title1.setFillColor(sf::Color::White);
    title1.setPosition(window.getSize().x / 2.0f - 330, window.getSize().y / 6.0f);

    title2.setFont(font);
    title2.setString("through");
    title2.setCharacterSize(60);
    title2.setFillColor(sf::Color::White);
    title2.setPosition(window.getSize().x / 2.0f - 140, window.getSize().y / 6.0f + 125.0f);

    title3.setFont(font);
    title3.setString("Rebellion");
    title3.setCharacterSize(100);
    title3.setFillColor(sf::Color::White);
    title3.setPosition(window.getSize().x / 2.0f - 295, window.getSize().y / 6.0f + 200.0f);

    title4.setFont(font);
    title4.setString("Play");
    title4.setCharacterSize(50);
    title4.setFillColor(sf::Color::White);
    title4.setPosition(window.getSize().x / 2.0f - 60, window.getSize().y / 6.0f + 467);

    title5.setFont(font);
    title5.setString("Restart");
    title5.setCharacterSize(50);
    title5.setFillColor(sf::Color::White);
    title5.setPosition(window.getSize().x / 2.0f - 110, window.getSize().y / 6.0f + 617);

    title6.setFont(font);
    title6.setString("Exit");
    title6.setCharacterSize(50);
    title6.setFillColor(sf::Color::White);
    title6.setPosition(window.getSize().x / 2.0f - 60, window.getSize().y / 6.0f + 767);

}

void Menu::addButton(const std::string& text, const std::function<void()>& callback) {
    float buttonWidth = 300.0f;
    float buttonHeight = 50.0f;
    float buttonX = window.getSize().x / 2.0f - buttonWidth / 2.0f;

    buttons.emplace_back(sf::Vector2f(buttonX, buttonYPosition));
    buttons.back().setCallback(callback);
    buttonYPosition += 150.0f;
}

void Menu::draw() {
    window.draw(backgroundSprite);
    for (auto& button : buttons) {
        button.draw(window);
    }
    window.draw(title1);
    window.draw(title2);
    window.draw(title3);

    window.draw(title4);
    window.draw(title5);
    window.draw(title6);
}

void Menu::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        for (auto& button : buttons) {
            if (button.isHovered(mousePos)) {
                button.onClick();
            }
        }
    }
}