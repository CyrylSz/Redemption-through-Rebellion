#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <filesystem>
#include <memory>
#include "../level/lev.hpp"
#include "../level/door.hpp"
#include "../menu/menu.hpp"
#include "../animation/anim.hpp"
#include "../inventory/inv.hpp"
#include "../inventory/bullet.hpp"
#include "../entity/enemy.hpp"

enum class GameState {
    Menu,
    Playing
};

const std::string ROOT = std::filesystem::current_path().parent_path().string();
const int TILE_SIZE = 128 * 5;
static sf::Font font;

class Game {

public:
    Game();
    void run();
    std::vector<Item> Items;
    std::vector<Item> MapItems;

private:
    sf::RenderWindow window;
    Menu menu;
    GameState gameState;
    Inventory inventory;
    int currentLevel;
    float playerSpeed;
    bool useFirstSegment;
    bool animationFinished;
    bool gameWon;
    bool gameOver;

    sf::Text winText;
    sf::Text ripText;

    sf::Texture playerPrisonIdle;
    sf::Texture walkingTexture;
    sf::Texture playerPrisonIdleWithBackpack;
    sf::Texture walkingTextureWithBackpack;
    sf::CircleShape playerBoundary;
    sf::CircleShape pickupRadius;
    Animation walkingAnimation;
    Animation idleAnimation;
    Animation idleAnimationWithBackpack;
    Animation walkingAnimationWithBackpack;
    sf::Sprite playerSprite;
    sf::Sprite playerItemSprite;
    sf::Clock animationClock;
    sf::View hudView;

    std::vector<Level> levels;
    sf::Sprite prisonLevelSprite;
    sf::Sprite surfaceLevelSprite;
    sf::Texture prisonLevelTexture;
    sf::Texture surfaceLevelTexture;
    sf::Texture weaponAnimTextures;
    sf::Texture weaponIdleTexture;
    sf::Texture bulletTexture;
    std::vector<sf::Texture> PrisonLevelItemsTextures;
    std::vector<sf::Texture> doorTexture;
    std::vector<sf::Texture> PrisonLevelEnemiesTextures;
    std::vector<sf::Texture> PrisonLevelDeadEnemiesTextures;
    std::vector<Door> doors;
    std::vector<Enemy> enemies;

    std::vector<Bullet> bullets;
    sf::Clock BulletClock;
    sf::Vector2f direction;
    std::vector<sf::Texture> itemsIdleTextures;
    Animation weaponAnimation;
    std::vector<sf::Sprite> enemySprites;

    sf::RectangleShape Elevator;
    sf::RectangleShape Escape;

    void init_Menu();
    void init_Items();
    void init_Player();
    void init_PrisonLevel();
    void init_SurfaceLevel();

    void processEvents();
    void update(sf::Time deltaTime);
    void render();

    void loadProgress();
    void saveProgress();
    void continueGame();
    void startNewGame();
    void exitGame();
};

#endif
