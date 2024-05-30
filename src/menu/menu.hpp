#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>

class Button {
public:
    explicit Button(const sf::Vector2f& position);

    void draw(sf::RenderWindow& window);
    bool isHovered(const sf::Vector2f& mousePos) const;
    void setCallback(const std::function<void()>& callback);
    void onClick() const;

private:
    sf::Text text;
    sf::RectangleShape shape;
    std::function<void()> callback;
};

class Menu {
public:
    Menu(sf::RenderWindow& window, const sf::Font& font, const std::string& backgroundPath);

    void addButton(const std::string& text, const std::function<void()>& callback);
    void draw();
    void handleEvent(const sf::Event& event);

private:
    sf::RenderWindow& window;
    sf::Text title1;
    sf::Text title2;
    sf::Text title3;
    sf::RectangleShape shape1;
    sf::RectangleShape shape2;
    sf::RectangleShape shape3;
    sf::Text title4;
    sf::Text title5;
    sf::Text title6;
    std::vector<Button> buttons;
    sf::Font font;
    float buttonYPosition;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
};

#endif // MENU_HPP
