#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>

class Button {
public:
    Button(const sf::Vector2f& position, const std::string& text, sf::Font& font);

    void draw(sf::RenderWindow& window);
    bool isHovered(const sf::Vector2f& mousePos) const;
    void setCallback(const std::function<void()>& callback);
    void onClick() const;
    static void centerText(sf::Text& text);
    void changeButtonMode(bool active);

private:
    sf::Text text;
    sf::RectangleShape shape;
    std::function<void()> callback;
};

class Menu {
public:
    Menu(sf::RenderWindow& window, sf::Font& font, const std::string& backgroundPath);

    void addButton(const std::string& text, const std::function<void()>& callback);
    void addTitle(const std::string& text, int characterSize, float yOffset);
    void draw();
    void handleEvent(const sf::Event& event);

    void setButtonSpacing(float spacing);
    std::vector<Button> buttons;

private:
    sf::RenderWindow& window;
    sf::Font& font;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    std::vector<sf::Text> titles;
    std::vector<sf::RectangleShape> titleShapes;
    float buttonYPosition;
    float buttonSpacing;
    float verticalOffset;

    void centerTitles();
    static void centerText(sf::Text& text);
};

#endif
