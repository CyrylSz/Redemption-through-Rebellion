#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../level/lev.hpp"
#include "../level/door.hpp"
#include "../inventory/inv.hpp"
#include "../menu/menu.hpp"
#include "../animation/anim.hpp"

enum class GameState {
    Menu,
    Playing
};

const int TILE_SIZE = 128 * 5;

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
    std::vector<sf::Texture> PrisonLevelItemsTextures;

    sf::Texture playerPrisonIdle;
    sf::Texture walkingTexture;
    sf::CircleShape playerHitbox;
    sf::CircleShape pickupRadius;
    float playerSpeed;

    GameState gameState;
    Menu menu;

    void startNewGame();
    void loadGame();
    void exitGame();

    void initializePrisonLevel();
    sf::Texture prisonLevelTexture;
    sf::Sprite prisonLevelSprite;

    void processEvents();
    void update(sf::Time deltaTime);
    void render();

    //animation
    Animation walkingAnimation;
    Animation idleAnimation;
    sf::Sprite animatedSprite;
    sf::Clock animationClock;

    bool useFirstSegment;
    bool animationFinished;

    void loadAnimations();

    std::vector<sf::Texture> doorTexture;
    std::vector<Door> doors;
};

#endif
