#include "menu.hpp"
#include <fmt/core.h>

Button::Button(const sf::Vector2f& position, const std::string& textStr, sf::Font& font)
        : callback(nullptr) {
    shape.setSize(sf::Vector2f(300, 75));
    shape.setFillColor(sf::Color::Black);
    shape.setPosition(position.x, position.y);

    text.setFont(font);
    text.setString(textStr);
    text.setCharacterSize(36);
    text.setFillColor(sf::Color::White);
    centerText(text);
    text.setPosition(
            position.x + shape.getSize().x / 2.0f,
            position.y + shape.getSize().y / 2.0f
    );
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
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

void Button::centerText(sf::Text& text) {
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
}

void Button::changeButtonMode(bool active) {
    if (active){
        shape.setFillColor(sf::Color::Black);
        text.setFillColor(sf::Color::White);
    } else {
        shape.setFillColor(sf::Color(0, 0, 0, 100));
        text.setFillColor(sf::Color(164, 164, 164));
    }
}

Menu::Menu(sf::RenderWindow& window, sf::Font& font, const std::string& backgroundPath)
        : window(window), font(font), buttonYPosition(window.getSize().y / 2.0f), buttonSpacing(100.0f), verticalOffset(100.0f) {

    if (!backgroundTexture.loadFromFile(backgroundPath)) {
        fmt::println("Error while loading background");
    }
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(
            window.getSize().x / backgroundSprite.getLocalBounds().width,
            window.getSize().y / backgroundSprite.getLocalBounds().height
    );
}

void Menu::addTitle(const std::string& text, int characterSize, float yOffset) {
    sf::Text title;
    title.setFont(font);
    title.setString(text);
    title.setCharacterSize(characterSize);
    title.setFillColor(sf::Color::White);

    sf::RectangleShape titleShape;
    titleShape.setSize(sf::Vector2f(title.getLocalBounds().width, title.getLocalBounds().height));
    titleShape.setFillColor(sf::Color::Transparent);
    titleShape.setOutlineColor(sf::Color::Transparent);
    titleShape.setOutlineThickness(0);
    titleShape.setPosition(window.getSize().x / 2.0f, window.getSize().y / 6.0f + yOffset);

    centerText(title);
    title.setPosition(
            titleShape.getPosition().x,
            titleShape.getPosition().y
    );

    titles.push_back(title);
    titleShapes.push_back(titleShape);
    centerTitles();
}

void Menu::centerTitles() {
    for (size_t i = 0; i < titles.size(); ++i) {
        sf::Text& title = titles[i];
        sf::RectangleShape& titleShape = titleShapes[i];

        centerText(title);
        title.setPosition(
                window.getSize().x / 2.0f,
                title.getPosition().y
        );

        titleShape.setPosition(
                window.getSize().x / 2.0f - titleShape.getSize().x / 2.0f,
                titleShape.getPosition().y
        );
    }
}

void Menu::centerText(sf::Text& text) {
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
}

void Menu::addButton(const std::string& text, const std::function<void()>& callback) {
    float buttonWidth = 300.0f;
    float buttonHeight = 75.0f;
    float buttonX = window.getSize().x / 2.0f - buttonWidth / 2.0f;
    float buttonY = buttonYPosition + buttons.size() * (buttonHeight + buttonSpacing) + verticalOffset;

    buttons.emplace_back(sf::Vector2f(buttonX, buttonY), text, font);
    buttons.back().setCallback(callback);
}

void Menu::draw() {
    window.draw(backgroundSprite);
    for (auto& title : titles) {
        window.draw(title);
    }
    for (auto& button : buttons) {
        button.draw(window);
    }
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

void Menu::setButtonSpacing(float spacing) {
    buttonSpacing = spacing;
}