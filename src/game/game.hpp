#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../level/lev.hpp"
#include "../inventory/inv.hpp"
#include "../menu/menu.hpp"

enum class GameState {
    Menu,
    Playing
};

const int TILE_SIZE = 128 * 5;
const int NUM_ITEMS = 3;

sf::Vector2f normalize(const sf::Vector2f &source);

class Game {

public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    sf::View hudView;
    std::vector<Level> levels;
    int currentLevel;
    sf::Font font;
    sf::Text winText;
    bool gameWon;
    Inventory inventory;
    std::vector<Item> items;
    std::vector<sf::Texture> itemTextures;

    sf::Texture playerTexture;
    sf::Sprite playerSprite;
    sf::CircleShape playerHitbox;
    sf::CircleShape pickupRadius;
    float playerSpeed;

    GameState gameState;
    Menu menu;

    void startNewGame();
    void loadGame();
    void exitGame();

    void generateItems(const std::vector<sf::Texture>& textures, int numItems);
    void loadTextures();

    void initializeTestLevel();

    void processEvents();
    void update(sf::Time deltaTime);
    void render();
};

#endif
