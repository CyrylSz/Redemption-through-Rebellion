#include "game.hpp"
#include <cmath>
#include <fmt/core.h>

Game::Game()
        : window(sf::VideoMode::getDesktopMode(), "Redemption-through-Rebellion", sf::Style::Fullscreen),
          menu(window, font, ROOT+"/res/menus/background.png"),
          gameState(GameState::Menu),
          inventory(font, this),
          currentLevel(0),
          playerSpeed(800.0f),
          useFirstSegment(true),
          animationFinished(false),
          gameWon(false),
          gameOver(false) {

    init_Items();
    loadProgress();
    init_Menu();
}

void Game::init_Menu() {

    if (!font.loadFromFile(ROOT+"/res/fonts/Monocraft-nerd-fonts-patched.ttf")) {
        fmt::println("Failed to load pixel font");
        exit(-1);
    }

    menu.addTitle("Redemption", 108, 0.0f);
    menu.addTitle("through", 72, 100.0f);
    menu.addTitle("Rebellion", 108, 200.0f);

    menu.setButtonSpacing(60.0f);

    menu.addButton("Continue", [this]() { continueGame(); });
    menu.addButton("New Game", [this]() { startNewGame(); });
    menu.addButton("Exit", [this]() { exitGame(); });

    if(currentLevel==0)menu.buttons[0].changeButtonMode(false);

    winText = sf::Text("You Escaped", font, 100);
    winText.setFillColor(sf::Color::Green);
    winText.setPosition(static_cast<float>(window.getSize().x) / 2.0f, static_cast<float>(window.getSize().y) / 2.0f);
    winText.setOrigin(winText.getLocalBounds().width / 2.0f, winText.getLocalBounds().height / 2.0f);

    ripText = sf::Text("RIP", font, 100);
    ripText.setFillColor(sf::Color::Red);
    ripText.setPosition(static_cast<float>(window.getSize().x) / 2.0f, static_cast<float>(window.getSize().y) / 2.0f);
    ripText.setOrigin(ripText.getLocalBounds().width / 2.0f, ripText.getLocalBounds().height / 2.0f);
}
void Game::saveProgress() {
    inventory.saveInventory(ROOT+"/progressFile.txt", currentLevel, gameWon, Items);
}

void Game::loadProgress() {
    inventory.loadInventory(ROOT+"/progressFile.txt", currentLevel, gameWon, Items);
}

void Game::continueGame() {
    if(currentLevel>0) {
        init_Player();
        init_PrisonLevel();
        init_SurfaceLevel();
        loadProgress();
        Elevator.setPosition(-100,-100);
        playerSprite.setPosition(TILE_SIZE*10+63*5, 5*78);
        playerItemSprite.setPosition(-100,-100);
        gameState = GameState::Playing;
    }
}

void Game::startNewGame() {
    gameWon = false;
    currentLevel = 0;
    inventory.removeAllItems();
    inventory.setBackpackAvailable(false);
    levels.clear();
    init_Player();
    init_PrisonLevel();
    playerSprite.setPosition(TILE_SIZE*4+-260, TILE_SIZE*2-400);
    playerItemSprite.setPosition(-100,-100);
    menu.buttons[0].changeButtonMode(false);
    saveProgress();
    loadProgress();
    gameState = GameState::Playing;
}

void Game::exitGame() {
    window.close();
}

void Game::init_Player() {
    if (inventory.isBackpackAvailable()) {
        if (!playerPrisonIdle.loadFromFile(ROOT + "/res/animations/player-prison-idle-backpack.png")) {
            fmt::println("Failed to load idle texture with backpack");
            exit(-1);
        }
        if (!walkingTexture.loadFromFile(ROOT + "/res/animations/player-prison-walking-backpack.png")) {
            fmt::println("Failed to load walking texture with backpack");
            exit(-1);
        }
        sf::Vector2f currentPosition = playerSprite.getPosition();
        playerSprite.setPosition(currentPosition);
    } else {
        if (!playerPrisonIdle.loadFromFile(ROOT + "/res/animations/player-prison-idle.png")) {
            fmt::println("Failed to load idle texture");
            exit(-1);
        }
        if (!walkingTexture.loadFromFile(ROOT + "/res/animations/player-prison-walking.png")) {
            fmt::println("Failed to load walking texture");
            exit(-1);
        }
        playerSprite.setPosition(TILE_SIZE*4+-260, TILE_SIZE*2-400);
    }

    int frameWidth = 32;
    int frameHeight = 32;

    idleAnimation.setSpriteSheet(playerPrisonIdle);
    idleAnimation.setFrameTime(sf::seconds(0.1f));
    idleAnimation.addFrame(sf::IntRect(0, 0, frameWidth, frameHeight));
    idleAnimation.addFrame(sf::IntRect(8 * frameWidth, 0, frameWidth, frameHeight));

    walkingAnimation.setSpriteSheet(walkingTexture);
    for (int i = 0; i < 16; ++i) {
        walkingAnimation.addFrame(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    playerSprite.setTexture(playerPrisonIdle);
    playerSprite.setScale(5.0f, 5.0f);
    sf::FloatRect playerBounds = playerSprite.getLocalBounds();
    playerSprite.setOrigin(playerBounds.width / 2.0f, playerBounds.height / 2.0f);

    playerBoundary.setRadius(playerBounds.width * 1.3f);
    playerBoundary.setOrigin(playerBoundary.getRadius(), playerBoundary.getRadius());
    playerBoundary.setPosition(playerSprite.getPosition());
    playerBoundary.setFillColor(sf::Color(0, 255, 0, 100));

    pickupRadius.setRadius(playerBounds.width * 2.5f);
    pickupRadius.setOrigin(pickupRadius.getRadius(), pickupRadius.getRadius());
    pickupRadius.setPosition(playerSprite.getPosition());
    pickupRadius.setFillColor(sf::Color(255, 0, 0, 100));

    hudView.setSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
    hudView.setCenter(static_cast<float>(window.getSize().x) / 2.0f, static_cast<float>(window.getSize().y) / 2.0f);
}

void Game::init_Items() {
    const std::filesystem::path path{ROOT+"/res/inventories/items"};
    int numOf_Items = 0;
    for ([[maybe_unused]] const auto& entry : std::filesystem::directory_iterator(path))++numOf_Items;

    PrisonLevelItemsTextures.resize(numOf_Items);
    itemsIdleTextures.resize(numOf_Items);
    for (int i = 0; i<numOf_Items; ++i) {
        if (!PrisonLevelItemsTextures[i].loadFromFile(ROOT+fmt::format("/res/inventories/items/item{}.png", i))) {
            fmt::println("Failed to load item {} on prison levels: ", i);
            exit(-1);
        }
        sf::Sprite tempSprite;
        tempSprite.setTexture(PrisonLevelItemsTextures[i]);
        tempSprite.setScale(5.0f, 5.0f);
        switch (i) {
            case 0:
                Items.emplace_back(tempSprite, "Widelec", "LPM to attack");
                break;
            case 1:
                Items.emplace_back(tempSprite, "But", "useless");
                break;
            case 2:
                Items.emplace_back(tempSprite, "Keycard", "Otwiera metalowe drzwi");
                break;
            case 3:
                Items.emplace_back(tempSprite, "Ammo 9mm", "Ammo to Glock-45", 5);
                break;
            case 4:
                Items.emplace_back(tempSprite, "Glock-45", "LMB to shoot");
                break;
            case 5:
                Items.emplace_back(tempSprite, "Backpack", "+9 slots");
                break;
            case 6:
                Items.emplace_back(tempSprite, "Knife","LPM to attack");
                break;
            default: {}
        }
        if (!itemsIdleTextures[i].loadFromFile(ROOT + fmt::format("/res/animations/player-prison-idle-item{}.png", i))) {
            fmt::println("Failed to load weapon sprite sheet");
            exit(-1);
        }
    }

    if (!weaponAnimTextures.loadFromFile(ROOT + "/res/animations/player-prison-glock-anim.png")) {
        fmt::println("Failed to load weapon sprite sheet");
        exit(-1);
    }
    if (!bulletTexture.loadFromFile(ROOT+"/res/inventories/gun/bullet1.png")) {
        fmt::println("Failed to load bullet texture");
        exit(-1);
    }
}

void Game::init_PrisonLevel() {
    MapItems.clear();
    doors.clear();
    enemies.clear();
    //================================================
    if (!prisonLevelTexture.loadFromFile(ROOT+"/res/levels/level-prison.png")) {
        fmt::println("Failed to load prison levels texture");
        exit(-1);
    }
    prisonLevelSprite.setTexture(prisonLevelTexture);
    prisonLevelSprite.setPosition(0, 0);
    prisonLevelSprite.setScale(5.0f, 5.0f);
    //================================================
    short numOf_Enemies = 5;
    PrisonLevelEnemiesTextures.resize(numOf_Enemies);
    enemySprites.resize(numOf_Enemies);
    for (int i = 0; i<numOf_Enemies; ++i) {
        if (!PrisonLevelEnemiesTextures[i].loadFromFile(ROOT+fmt::format("/res/entities/enemy{}.png", i))) {
            fmt::println("Failed to load enemy {} on prison levels: ", i);
            exit(-1);
        }
        enemySprites[i].setTexture(PrisonLevelEnemiesTextures[i]);
        enemySprites[i].setScale(5.0f, 5.0f);
    }
    short numOf_deadEnemies = 2;
    PrisonLevelDeadEnemiesTextures.resize(numOf_deadEnemies);
    for (int i = 0; i<numOf_deadEnemies; ++i) {
        if (!PrisonLevelDeadEnemiesTextures[i].loadFromFile(ROOT+fmt::format("/res/entities/deadEnemy{}.png", i))) {
            fmt::println("Failed to load dead enemy {} on prison levels: ", i);
            exit(-1);
        }
    }
    //================================================
    Elevator.setSize(sf::Vector2f(5*76.0f, 5*76.0f));
    Elevator.setFillColor(sf::Color::Transparent);
    Elevator.setPosition(5*128*10.0f+26*5, 5*50.0f);
    //================================================
    short numOf_DoorTypes = 4;
    doorTexture.resize(numOf_DoorTypes);
    for (int i=0; i < numOf_DoorTypes; ++i)
        if (!doorTexture[i].loadFromFile(ROOT+fmt::format("/res/levels/doors/door{}.png", i))) {
            fmt::println("Failed to load door texture: ", i);
            exit(-1);
        }
    //================================================
    short numOf_Columns = 14;
    short numOf_Rows = 11;
    Level level(numOf_Rows, numOf_Columns, prisonLevelSprite);

    for (int col = 0; col < numOf_Columns; ++col) {
        for (int row = 0; row < numOf_Rows; ++row) {
            sf::Vector2f pos(static_cast<float>(col*128), static_cast<float>(row*128));
            Tile* tile = new Tile(pos);

            switch (row) {
                case 0:
                    if(col == 10) {
                        tile->addBoundary(sf::FloatRect(pos.x+23, pos.y+47, 82, 4));
                        tile->addBoundary(sf::FloatRect(pos.x+23, pos.y+51, 4, 74));
                        tile->addBoundary(sf::FloatRect(pos.x+101, pos.y+51, 4, 74));
                        tile->addBoundary(sf::FloatRect(pos.x+22, pos.y+125, 18, 10));
                        tile->addBoundary(sf::FloatRect(pos.x+88, pos.y+125, 18, 10));
                        tile->addBoundary(sf::FloatRect(pos.x-8, pos.y+120, 30, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+106, pos.y+120, 406, 8));
                    }
                    break;
                case 1:
                    if(col == 0){
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+24, 128, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+32, 8, 200));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+32, 8, 56));
                        tile->addBoundary(sf::FloatRect(pos.x+16, pos.y+88, 85, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+95, pos.y+60, 6, 28));
                        Items[0].sprite.setPosition(5 * (pos.x + 77), 5 * (pos.y + 70));
                        MapItems.emplace_back(Items[0]);
                        enemySprites[2].setPosition(5 * (pos.x + 30), 5 * (pos.y + 150));
                        enemies.emplace_back(enemySprites[2], "prisoner-but", 60*5, 20*5, 60*5, Items[1]);
                    } else if (col == 1){
                        tile->addBoundary(sf::FloatRect(pos.x+0, pos.y+24, 8, 72));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+88, 152, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+96, 8, 16));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+72), 5*(pos.y+114)), doorTexture[1], "S", 1);
                    } else if (col == 2){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 8, 88));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y, 888, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+64, pos.y+88, 96, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+34), 5*(pos.y+92)), doorTexture[0], "E", 0);
                    } else if (col == 3 || col == 4 || col == 5 || col == 6 || col == 7){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+8, 8, 80));
                        tile->addBoundary(sf::FloatRect(pos.x+64, pos.y+88, 96, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+34), 5*(pos.y+92)), doorTexture[0], "E", 0);
                    } else if (col == 8){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+8, 8, 80));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+8, 8, 80));
                        tile->addBoundary(sf::FloatRect(pos.x+64, pos.y+88, 556, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+34), 5*(pos.y+92)), doorTexture[0], "E", 0);
                        Items[1].sprite.setPosition(5 * (pos.x + 18), 5 * (pos.y + 25));
                        MapItems.emplace_back(Items[1]);
                    } else if (col == 9){
                        tile->addBoundary(sf::FloatRect(pos.x+20, pos.y+96, 8, 15));
                        tile->addBoundary(sf::FloatRect(pos.x+100, pos.y+96, 8, 15));
                    } else if (col == 10){
                        tile->addBoundary(sf::FloatRect(pos.x-8, pos.y, 8, 88));
                        tile->addBoundary(sf::FloatRect(pos.x+84, pos.y+96, 8, 112));
                    } else if (col == 13){
                        tile->addBoundary(sf::FloatRect(pos.x-28, pos.y+96, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y, 8, 264));
                        doors.emplace_back(sf::Vector2f(5*(pos.x-24), 5*(pos.y+114)), doorTexture[3], "S", 3);
                    }
                    break;
                case 2:
                    if (col == 0){
                        tile->addBoundary(sf::FloatRect(pos.x+16, pos.y+96, 120, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+40, 8, 56));
                        tile->addBoundary(sf::FloatRect(pos.x+49, pos.y+32, 271, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+43, pos.y+32, 6, 36));
                    } else if (col == 1){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+40, 8, 56));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+16, 8, 16));
                    } else if (col == 2){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+40, 8, 88));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+120, 368, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y+32, 96, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+40, 8, 80));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+94), 5*(pos.y+36)), doorTexture[0], "W", 0);
                    } else if (col == 3 || col == 7){
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y+32, 96, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+40, 8, 80));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+94), 5*(pos.y+36)), doorTexture[0], "W", 0);
                    } else if (col == 4){
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y+32, 32, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+40, 8, 208));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+94), 5*(pos.y+36)), doorTexture[0], "W", 0);
                    } else if (col == 6){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+32, 8, 216));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+32, 56, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y+32, 96, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+120, 376, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+40, 8, 80));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+94), 5*(pos.y+36)), doorTexture[0], "W", 0);
                    } else if (col == 8){
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y+32, 160, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+40, 8, 80));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+94), 5*(pos.y+36)), doorTexture[0], "W", 0);
                    } else if (col == 9){
                        tile->addBoundary(sf::FloatRect(pos.x+20, pos.y+16, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+100, pos.y+16, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+40, 8, 224));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+24), 5*(pos.y+14)), doorTexture[2], "N", 2);
                        doors.emplace_back(sf::Vector2f(5*(pos.x+104), 5*(pos.y+14)), doorTexture[2], "N", 2);
                    } else if (col == 10){
                        tile->addBoundary(sf::FloatRect(pos.x+84, pos.y+112, 8, 24));
                        tile->addBoundary(sf::FloatRect(pos.x+92, pos.y+128, 420, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+88), 5*(pos.y+82)), doorTexture[3], "S", 3);
                    } else if (col == 12){
                        tile->addBoundary(sf::FloatRect(pos.x+100, pos.y+16, 8, 112));
                        tile->addBoundary(sf::FloatRect(pos.x+128, pos.y+136, 8, 128));
                        enemySprites[4].setPosition(5 * (pos.x), 5 * (pos.y));
                        enemies.emplace_back(enemySprites[4], "guard-glock", 150*5, 30*5, 50*5, Items[4]);
                        Items[3].sprite.setPosition(5 * (pos.x+35), 5 * (pos.y + 75));
                        MapItems.emplace_back(Items[3]);
                        Items[3].sprite.setPosition(5 * (pos.x+145), 5 * (pos.y + 90));
                        MapItems.emplace_back(Items[3]);
                        Items[3].sprite.setPosition(5 * (pos.x+212), 5 * (pos.y + 75));
                        MapItems.emplace_back(Items[3]);
                    }
                    break;
                case 3:
                    if (col == 5){
                        enemySprites[1].setPosition(5 * (pos.x + 20), 5 * (pos.y + 50));
                        enemies.emplace_back(enemySprites[1], "prisoner-widelec", 150*5, 30*5, 45*5, Items[0]);
                        enemySprites[1].setPosition(5 * (pos.x + 30), 5 * (pos.y + 400));
                        enemies.emplace_back(enemySprites[1], "prisoner-widelec", 150*5, 30*5, 45*5, Items[0]);
                    }
                    break;
                case 4:
                    if (col == 3){
                        tile->addBoundary(sf::FloatRect(pos.x-8, pos.y+120, 184, 8));
                        tile->addBoundary(sf::FloatRect(pos.x-8, pos.y+128, 8, 160));
                    } else if (col == 4){
                        tile->addBoundary(sf::FloatRect(pos.x-8, pos.y-8, 8, 128));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y-8, 160, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+80, pos.y+120, 80, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+78), 5*(pos.y+124)), doorTexture[1], "W", 1);
                    } else if (col == 5){
                        tile->addBoundary(sf::FloatRect(pos.x+24, pos.y, 8, 120));
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y, 8, 120));
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y-8, 168, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y+120, 80, 8));
                    } else if (col == 6) {
                        doors.emplace_back(sf::Vector2f(5*(pos.x+50), 5*(pos.y+124)), doorTexture[1], "E", 1);
                    } else if (col == 7){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 8, 120));
                        tile->addBoundary(sf::FloatRect(pos.x-48, pos.y+120, 184, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+128, pos.y+128, 8, 672));
                    } else if (col == 10){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 288, 8));
                    } else if (col == 12){
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y, 32, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+24, pos.y+8, 8, 128));
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y+8, 8, 128));
                        tile->addBoundary(sf::FloatRect(pos.x+32, pos.y+67, 16, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+80, pos.y+67, 16, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+50), 5*(pos.y+71)), doorTexture[3], "E", 3);
                    }
                    break;
                case 5:
                    if (col == 11){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 152, 8));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+8, 8, 384));
                    } else if (col == 12){
                        tile->addBoundary(sf::FloatRect(pos.x+104, pos.y, 152, 8));
                        enemySprites[0].setPosition(5 * (pos.x + 92), 5 * (pos.y + 50));
                        enemies.emplace_back(enemySprites[0], "guard-knife", 150*5, 30*5, 50*5, Items[6]);
                        enemySprites[0].setPosition(5 * (pos.x - 100), 5 * (pos.y + 300));
                        enemies.emplace_back(enemySprites[0], "guard-knife", 150*5, 30*5, 50*5, Items[6]);
                    } else if (col == 13){
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+8, 8, 384));
                        enemySprites[0].setPosition(5 * (pos.x + 92), 5 * (pos.y + 350));
                        enemies.emplace_back(enemySprites[0], "guard-knife", 150*5, 30*5, 50*5, Items[6]);
                    }
                    break;
                case 6:
                    if (col == 0){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y-8, 264, 8));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 8, 264));
                        enemySprites[0].setPosition(5 * (pos.x + 92), 5 * (pos.y + 50));
                        enemies.emplace_back(enemySprites[0], "guard-knife", 150*5, 30*5, 50*5, Items[6]);
                    } else if (col == 2){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 8, 32));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+96, 8, 32));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+24, 112, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+96, 112, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+32, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+80, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+96, 8, 296));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+72), 5*(pos.y+78)), doorTexture[1], "N", 1);
                    }
                    break;
                case 7:
                    if (col == 1){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 136, 8));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+8, 8, 120));
                        Items[2].sprite.setPosition(5 * (pos.x - 60), 5 * (pos.y + 80));
                        MapItems.emplace_back(Items[2]);
                    } else if (col == 9){
                        tile->addBoundary(sf::FloatRect(pos.x-8, pos.y-8, 8, 424));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y-8, 128, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+128, pos.y-8, 8, 264));
                        Items[5].sprite.setPosition(5 * (pos.x + 60), 5 * (pos.y + 50));
                        MapItems.emplace_back(Items[5]);
                    }
                    break;
                case 8:
                    if (col == 0){
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y, 128, 8));
                    } else if (col == 3){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 48, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+80, pos.y, 192, 8));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+128, 512, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+78), 5*(pos.y+4)), doorTexture[1], "W", 1);
                    } else if (col == 4){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+8, 8, 120));
                    } else if (col == 5){
                        tile->addBoundary(sf::FloatRect(pos.x+64, pos.y, 240, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+62), 5*(pos.y+4)), doorTexture[1], "W", 1);
                        doors.emplace_back(sf::Vector2f(5*(pos.x+2), 5*(pos.y+4)), doorTexture[1], "E", 1);
                    } else if (col == 6){
                        tile->addBoundary(sf::FloatRect(pos.x+84, pos.y+8, 8, 48));
                        tile->addBoundary(sf::FloatRect(pos.x+84, pos.y+88, 8, 40));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+88), 5*(pos.y+58)), doorTexture[3], "S", 3);
                    } else if (col == 7){
                        tile->addBoundary(sf::FloatRect(pos.x+80, pos.y, 48, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+50), 5*(pos.y+4)), doorTexture[3], "E", 3);
                    } else if (col == 9){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+99, 48, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+80, pos.y+99, 48, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+50), 5*(pos.y+103)), doorTexture[3], "E", 3);
                    } else if (col == 11){
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y, 120, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+8, 8, 120));
                        tile->addBoundary(sf::FloatRect(pos.x-120, pos.y+120, 240, 8));
                    } else if (col == 12){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+36, 48, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+80, pos.y+36, 48, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+50), 5*(pos.y+40)), doorTexture[3], "E", 3);
                    } else if (col == 13){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 120, 8));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+8, 8, 120));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+120, 120, 8));
                    }
                    break;
                case 9:
                    if (col == 7) {
                        tile->addBoundary(sf::FloatRect(pos.x-8, pos.y+8, 8, 120));
                    } else if (col == 10){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+56, 8, 192));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+56, 136, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+116, pos.y+64, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+116, pos.y+112, 8, 32));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+120, 108, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+120), 5*(pos.y+82)), doorTexture[1], "S", 1);
                    } else if (col == 11){
                        tile->addBoundary(sf::FloatRect(pos.x+48, pos.y+56, 144, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+64, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+112, 8, 32));
                        tile->addBoundary(sf::FloatRect(pos.x+76, pos.y+120, 108, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+72), 5*(pos.y+82)), doorTexture[1], "S", 1);
                        doors.emplace_back(sf::Vector2f(5*(pos.x+18), 5*(pos.y+60)), doorTexture[3], "E", 3);
                    } else if (col == 12){
                        tile->addBoundary(sf::FloatRect(pos.x+56, pos.y+64, 8, 184));
                    } else if (col == 13){
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y, 8, 248));
                    }
                    break;
                case 10:
                    if (col == 5) {
                        tile->addBoundary(sf::FloatRect(pos.x-8, pos.y-8, 256, 8));
                        tile->addBoundary(sf::FloatRect(pos.x-8, pos.y, 8, 128));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+120, 392, 8));
                        Items[3].sprite.setPosition(5 * (pos.x + 18), 5 * (pos.y + 100));
                        MapItems.emplace_back(Items[3]);
                        Items[3].sprite.setPosition(5 * (pos.x + 30), 5 * (pos.y + 60));
                        MapItems.emplace_back(Items[3]);
                        Items[3].sprite.setPosition(5 * (pos.x + 10), 5 * (pos.y + 30));
                        MapItems.emplace_back(Items[3]);
                        Items[3].sprite.setPosition(5 * (pos.x + 80), 5 * (pos.y + 75));
                        MapItems.emplace_back(Items[3]);
                    } else if (col == 6){
                        tile->addBoundary(sf::FloatRect(pos.x+4, pos.y, 8, 64));
                        tile->addBoundary(sf::FloatRect(pos.x+4, pos.y+96, 8, 24));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+8), 5*(pos.y+94)), doorTexture[3], "N", 3);
                    } else if (col == 8){
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+24, 112, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+96, 112, 8));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+96, 8, 24));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+96, 8, 24));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+120, 648, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+52, pos.y+32, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+52, pos.y+80, 8, 16));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+56), 5*(pos.y+50)), doorTexture[3], "S", 3);
                    } else if (col == 10){
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+56, 108, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+116, pos.y+48, 8, 32));
                        tile->addBoundary(sf::FloatRect(pos.x+116, pos.y+112, 8, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+120), 5*(pos.y+18)), doorTexture[1], "S", 1);
                        doors.emplace_back(sf::Vector2f(5*(pos.x+120), 5*(pos.y+82)), doorTexture[1], "S", 1);
                    } else if (col == 11){
                        tile->addBoundary(sf::FloatRect(pos.x+74, pos.y+56, 108, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+48, 8, 32));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+112, 8, 8));
                        Items[4].sprite.setPosition(5 * (pos.x + 100), 5 * (pos.y + 93));
                        MapItems.emplace_back(Items[4]);
                        doors.emplace_back(sf::Vector2f(5*(pos.x+72), 5*(pos.y+18)), doorTexture[1], "S", 1);
                        doors.emplace_back(sf::Vector2f(5*(pos.x+72), 5*(pos.y+82)), doorTexture[1], "S", 1);
                    }
                    break;
                default: {}
            }
            level.setTile(row, col, tile);
        }
    }
    levels.push_back(level);
}

void Game::init_SurfaceLevel() {
    MapItems.clear();
    doors.clear();
    enemies.clear();

    if (!surfaceLevelTexture.loadFromFile(ROOT + "/res/levels/level-surface.png")) {
        fmt::println("Failed to load prison levels texture");
        exit(-1);
    }
    surfaceLevelSprite.setTexture(surfaceLevelTexture);
    surfaceLevelSprite.setPosition(0, 0);
    surfaceLevelSprite.setScale(5.0f, 5.0f);

    Escape.setSize(sf::Vector2f(5*76.0f, 5*76.0f));
    Escape.setPosition(5*128*10, 5*128*5+60*5);

    menu.buttons[0].changeButtonMode(true);

    short numOf_Columns = 14;
    short numOf_Rows = 11;
    Level level(numOf_Rows, numOf_Columns, surfaceLevelSprite);

    for (int col = 0; col < numOf_Columns; ++col) {
        for (int row = 0; row < numOf_Rows; ++row) {
            sf::Vector2f pos(static_cast<float>(col*128), static_cast<float>(row*128));
            Tile* tile = new Tile(pos);

            switch (row) {
                case 0:
                    if(col == 10) {
                        tile->addBoundary(sf::FloatRect(pos.x+23, pos.y+47, 82, 4));
                        tile->addBoundary(sf::FloatRect(pos.x+23, pos.y+51, 4, 74));
                        tile->addBoundary(sf::FloatRect(pos.x+101, pos.y+51, 4, 74));
                        tile->addBoundary(sf::FloatRect(pos.x+22, pos.y+125, 18, 10));
                        tile->addBoundary(sf::FloatRect(pos.x+88, pos.y+125, 18, 10));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+120, 22, 8));
                        tile->addBoundary(sf::FloatRect(pos.x-8, pos.y+120, 8, 648));
                        tile->addBoundary(sf::FloatRect(pos.x+106, pos.y+120, 22, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+128, pos.y+120, 8, 648));
                    }
                    break;
                case 4:
                    if(col == 10) {
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+120, 48, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+80, pos.y+120, 48, 8));
                        enemySprites[0].setPosition(5 * (pos.x + 60), 5 * (pos.y - 100));
                        enemies.emplace_back(enemySprites[0], "guard-knife", 150*5, 30*5, 50*5, Items[6]);
                        enemySprites[4].setPosition(5 * (pos.x + 25), 5 * (pos.y + 50));
                        enemies.emplace_back(enemySprites[4], "guard-glock", 150*5, 30*5, 50*5, Items[4]);
                        enemySprites[4].setPosition(5 * (pos.x + 92), 5 * (pos.y + 50));
                        enemies.emplace_back(enemySprites[4], "guard-glock", 150*5, 30*5, 50*5, Items[4]);
                        doors.emplace_back(sf::Vector2f(5*(pos.x+78), 5*(pos.y+124)), doorTexture[2], "W", 2);
                    }
                default: {}
            }
            level.setTile(row, col, tile);
        }
    }
    levels.push_back(level);
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        processEvents();
        sf::Time deltaTime = clock.restart();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event{};
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (gameState == GameState::Menu) {
            menu.handleEvent(event);
        } else if (gameState == GameState::Playing) {
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::E) {
                    if (inventory.isBackpackAvailable()) {
                        inventory.toggleBackpack();
                        if (!inventory.isBackpackOpen()) {
                            inventory.clearSelection();
                        }
                    }
                }
                if (event.key.code == sf::Keyboard::F) {
                    for (auto& item : MapItems) {
                        if (!item.pickedUp && pickupRadius.getGlobalBounds().intersects(item.sprite.getGlobalBounds())) {
                            if (item.name == "Backpack") {
                                inventory.setBackpackAvailable(true);
                                init_Player();
                                item = Item();
                            } else if (inventory.addItem(item)) {
                                item.pickedUp = true;
                            }
                        }
                    }
                }
                if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num3) {
                    int hotbarIndex = event.key.code - sf::Keyboard::Num1;
                    inventory.holdItem(hotbarIndex);
                } else if (event.key.code >= sf::Keyboard::Num4 && event.key.code <= sf::Keyboard::Num9) {
                    inventory.holdItem(-1);
                }
                if (event.key.code == sf::Keyboard::Escape) {
                    gameState = GameState::Menu;
                }
                if (event.key.code == sf::Keyboard::Space) {
                    if (Elevator.getGlobalBounds().contains(playerSprite.getPosition())) {
                        Elevator.setPosition(-100,-100);
                        currentLevel++;
                        saveProgress();
                        init_SurfaceLevel();
                    }
                }
            }
            if (event.type == sf::Event::MouseWheelScrolled) {
                inventory.handleMouseWheel(-event.mouseWheelScroll.delta);
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::FloatRect playerBounds = playerSprite.getGlobalBounds();
                    Item* heldItem = inventory.getHeldItem();
                    if (heldItem && (heldItem->name=="Widelec" || heldItem->name=="Knife")) {
                        for (auto &enemy: enemies) {
                            if (enemy.isAlive && enemy.hitbox.getGlobalBounds().intersects(playerBounds)) {
                                enemy.isAlive = false;
                                if (enemy.type == "guard-glock" || enemy.type == "guard-knife")
                                    enemy.sprite.setTexture(PrisonLevelDeadEnemiesTextures[0]);
                                else enemy.sprite.setTexture(PrisonLevelDeadEnemiesTextures[1]);
                                sf::Vector2f dropPosition = enemy.sprite.getPosition() + sf::Vector2f(100.0f, 50.0f);
                                enemy.dropItem.sprite.setPosition(dropPosition);
                                MapItems.emplace_back(enemy.dropItem);
                            }
                        }
                    }
                    if (heldItem && heldItem->name=="Glock-45" && inventory.getTotalQuantity("Ammo 9mm")>0) {
                        inventory.decreaseQuantity("Ammo 9mm");
                        sf::Vector2f bulletOffset(-5*5, 20*5);
                        sf::Transform rotation;
                        rotation.rotate(playerSprite.getRotation());
                        sf::Vector2f bulletStartPos = playerSprite.getPosition() + rotation.transformPoint(bulletOffset);
                        bullets.emplace_back(bulletTexture, bulletStartPos, direction);
                    }
                    inventory.handleLeftMouseClick(sf::Mouse::getPosition(window));
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    inventory.handleRightMouseClick(sf::Mouse::getPosition(window));
                }
            }
            if (event.type == sf::Event::MouseMoved) {
                inventory.handleMouseDrag(sf::Mouse::getPosition(window),
                                          sf::Mouse::isButtonPressed(sf::Mouse::Left), playerSprite.getPosition());
            }
        }
    }
}


void Game::update(sf::Time deltaTime) {
    if (gameState == GameState::Playing && !gameWon && !gameOver) {
//===================[ PLAYER MOVEMENT ]===========================
        sf::Vector2f movement(0.0f, 0.0f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            movement.y -= playerSpeed * deltaTime.asSeconds();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            movement.y += playerSpeed * deltaTime.asSeconds();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            movement.x -= playerSpeed * deltaTime.asSeconds();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            movement.x += playerSpeed * deltaTime.asSeconds();
        }

        auto normalize = [](const sf::Vector2f &source) {
            float length = std::sqrt(source.x * source.x + source.y * source.y);
            if (length != 0)
                return sf::Vector2f{source.x / length, source.y / length};
            else
                return source;
        };

        sf::Vector2f normalizedMovement = normalize(movement) * playerSpeed * deltaTime.asSeconds();
        sf::Vector2f originalPosition = playerSprite.getPosition();

        playerSprite.move(normalizedMovement);
        playerBoundary.setPosition(playerSprite.getPosition());
        pickupRadius.setPosition(playerSprite.getPosition());

        if (levels[currentLevel].checkCollision(playerBoundary.getGlobalBounds())) {
            playerSprite.setPosition(originalPosition);
            playerBoundary.setPosition(originalPosition);
            pickupRadius.setPosition(originalPosition);

            sf::Vector2f slideMovementX(normalizedMovement.x * 0.85f, 0.0f);
            playerSprite.move(slideMovementX);
            playerBoundary.setPosition(playerSprite.getPosition());
            pickupRadius.setPosition(playerSprite.getPosition());
            if (levels[currentLevel].checkCollision(playerBoundary.getGlobalBounds())) {
                playerSprite.move(-slideMovementX);
                playerBoundary.setPosition(playerSprite.getPosition());
                pickupRadius.setPosition(playerSprite.getPosition());
            }

            sf::Vector2f slideMovementY(0.0f, normalizedMovement.y * 0.85f);
            playerSprite.move(slideMovementY);
            playerBoundary.setPosition(playerSprite.getPosition());
            pickupRadius.setPosition(playerSprite.getPosition());
            if (levels[currentLevel].checkCollision(playerBoundary.getGlobalBounds())) {
                playerSprite.move(-slideMovementY);
                playerBoundary.setPosition(playerSprite.getPosition());
                pickupRadius.setPosition(playerSprite.getPosition());
            }
        }
//===================[ PLAYER INTERACTIONS WITH DOORS ]===========================
        Item* heldItem = inventory.getHeldItem();
        for (auto &door: doors) {

            if (door.id == 2 || door.id == 3) {
                if (heldItem && heldItem->name == "Keycard")door.update(playerBoundary.getGlobalBounds(), deltaTime);
            } else {
                door.update(playerBoundary.getGlobalBounds(), deltaTime);
            }
            if (door.getBounds().intersects(playerBoundary.getGlobalBounds())) {
                playerSprite.setPosition(originalPosition);
                playerBoundary.setPosition(originalPosition);
                pickupRadius.setPosition(originalPosition);

                sf::Vector2f slideMovementX(normalizedMovement.x * 1.f, 0.0f);
                playerSprite.move(slideMovementX);
                playerBoundary.setPosition(playerSprite.getPosition());
                pickupRadius.setPosition(playerSprite.getPosition());
                if (door.getBounds().intersects(playerBoundary.getGlobalBounds())) {
                    playerSprite.move(-slideMovementX);
                    playerBoundary.setPosition(playerSprite.getPosition());
                    pickupRadius.setPosition(playerSprite.getPosition());
                }

                sf::Vector2f slideMovementY(0.0f, normalizedMovement.y * 1.f);
                playerSprite.move(slideMovementY);
                playerBoundary.setPosition(playerSprite.getPosition());
                pickupRadius.setPosition(playerSprite.getPosition());
                if (door.getBounds().intersects(playerBoundary.getGlobalBounds())) {
                    playerSprite.move(-slideMovementY);
                    playerBoundary.setPosition(playerSprite.getPosition());
                    pickupRadius.setPosition(playerSprite.getPosition());
                }
            }
        }
//===================[ PLAYER ROTATION ]===========================
        sf::View view = window.getView();
        view.setCenter(playerSprite.getPosition());
        window.setView(view);

        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePosition);
        sf::Vector2f playerPos = playerSprite.getPosition();
        direction = normalize(worldPos - playerPos);
        auto angle = static_cast<float>(std::atan2(direction.y, direction.x) * 180 / 3.14159265);
        playerSprite.setRotation(angle - 90);
//===================[ PLAYER WALKING ]===========================
        if (movement.x != 0 || movement.y != 0) {
            if (!walkingAnimation.isPlaying()) {
                if (useFirstSegment) {
                    walkingAnimation.setSegment(0, 7);
                } else {
                    walkingAnimation.setSegment(8, 15);
                }
                walkingAnimation.play();
                useFirstSegment = !useFirstSegment;
                animationFinished = false;
            }
            walkingAnimation.update(deltaTime);
            playerSprite.setTexture(walkingTexture);
            playerSprite.setTextureRect(walkingAnimation.getFrame());
        } else {
            if (walkingAnimation.isPlaying()) {
                walkingAnimation.update(deltaTime);
                playerSprite.setTexture(walkingTexture);
                playerSprite.setTextureRect(walkingAnimation.getFrame());
                if (walkingAnimation.getCurrentFrameIndex() == walkingAnimation.getEndFrame()) {
                    animationFinished = true;
                    useFirstSegment = !useFirstSegment;
                    walkingAnimation.stop();
                }
            } else {
                if (animationFinished) {
                    if (useFirstSegment) {
                        playerSprite.setTexture(walkingTexture);
                        playerSprite.setTextureRect(walkingAnimation.getFrame(0));
                    } else {
                        playerSprite.setTexture(walkingTexture);
                        playerSprite.setTextureRect(walkingAnimation.getFrame(8));
                    }
                } else {
                    idleAnimation.update(deltaTime);
                    playerSprite.setTexture(walkingTexture);
                    playerSprite.setTextureRect(idleAnimation.getFrame());
                }
            }
        }
//===================[ HOLDING ITEMS ]===========================
        playerItemSprite.setScale(5.0f, 5.0f);
        sf::FloatRect playerBounds = playerSprite.getLocalBounds();
        playerItemSprite.setOrigin(playerBounds.width / 2.0f, (playerBounds.height + 32) / 2.0f);
        playerItemSprite.setRotation(playerSprite.getRotation());

        if (heldItem) {
            if (heldItem->name=="Widelec")playerItemSprite.setTexture(itemsIdleTextures[0]);
            else if (heldItem->name=="But")playerItemSprite.setTexture(itemsIdleTextures[1]);
            else if (heldItem->name=="Keycard")playerItemSprite.setTexture(itemsIdleTextures[2]);
            else if (heldItem->name=="Ammo 9mm")playerItemSprite.setTexture(itemsIdleTextures[3]);
            else if (heldItem->name=="Glock-45")playerItemSprite.setTexture(itemsIdleTextures[4]);
            else if (heldItem->name=="Backpack")playerItemSprite.setTexture(itemsIdleTextures[5]);
            else if (heldItem->name=="Knife")playerItemSprite.setTexture(itemsIdleTextures[6]);
            playerItemSprite.setPosition(playerSprite.getPosition());
        } else {
            playerItemSprite.setPosition(-100,-100);
        }

        for (auto& bullet : bullets) {
            bullet.update(deltaTime);
            for (auto& enemy : enemies) {
                if (enemy.isAlive && bullet.getSprite().getGlobalBounds().intersects(enemy.hitbox.getGlobalBounds())) {
                    enemy.isAlive = false;

                    if (enemy.type == "guard-glock" || enemy.type == "guard-knife")
                        enemy.sprite.setTexture(PrisonLevelDeadEnemiesTextures[0]);
                    else enemy.sprite.setTexture(PrisonLevelDeadEnemiesTextures[1]);

                    sf::Vector2f dropPosition = enemy.sprite.getPosition() + sf::Vector2f(100.0f, 50.0f);
                    enemy.dropItem.sprite.setPosition(dropPosition);
                    MapItems.emplace_back(enemy.dropItem);
                }
            }
        }
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet& bullet) {
            return bullet.isExpired();
        }), bullets.end());
//===================[ ENEMIES ]===========================
        for (auto& enemy : enemies) {
            if (!enemy.isAlive) continue;

            sf::Vector2f playerPosition = playerSprite.getPosition();
            sf::Vector2f enemyPosition = enemy.sprite.getPosition();

            double distanceToPlayer = std::sqrt(std::pow(playerPosition.x - enemyPosition.x, 2) + std::pow(playerPosition.y - enemyPosition.y, 2));

            if (distanceToPlayer <= enemy.visionRadiusSize) {
                if (distanceToPlayer > enemy.attackRangeSize) {
                    sf::Vector2f direction2 = playerPosition - enemyPosition;
                    float length = std::sqrt(direction2.x * direction2.x + direction2.y * direction2.y);
                    direction2 /= length;
                    enemy.updatePosition(enemyPosition + direction2 * (playerSpeed / 2 + 200) * deltaTime.asSeconds());

                    float angle2 = std::atan2(direction2.y, direction2.x) * 180 / 3.14159f;
                    enemy.sprite.setRotation(angle2 + 270);

                    enemy.attackClock.restart();
                    enemy.playerInAttackRange = false;
                } else {
                    sf::Vector2f direction2 = playerPosition - enemyPosition;
                    float angle2 = std::atan2(direction2.y, direction2.x) * 180 / 3.14159f;
                    enemy.sprite.setRotation(angle2 + 270);

                    if (enemy.attackRange.getGlobalBounds().contains(playerPosition)) {
                        if (!enemy.playerInAttackRange) {
                            enemy.playerInAttackRange = true;
                            enemy.attackClock.restart();
                        } else if (enemy.attackClock.getElapsedTime().asSeconds() >= 0.1f) {
                            gameOver = true;
                            break;
                        }
                    } else {
                        enemy.attackClock.restart();
                        enemy.playerInAttackRange = false;
                    }
                }
            }
        }
//===================[ ESCAPE ]===========================
        if (Escape.getGlobalBounds().contains(playerSprite.getPosition())) {
            gameWon = true;
        }
    }
}

void Game::render() {
    window.clear();
    if (gameState == GameState::Menu) {
        menu.draw();
        if (gameOver)gameOver=false;
        switch (currentLevel) {
            case 0:
                playerSprite.setPosition(TILE_SIZE*4+-260, TILE_SIZE*2-400);
                break;
            case 1:
                playerSprite.setPosition(TILE_SIZE*10+60*5, 5*78);
                break;
        }
    } else if (gameState == GameState::Playing) {
        if (gameOver) {
            window.setView(window.getDefaultView());
            window.draw(ripText);
        } else if (!gameWon) {
            levels[currentLevel].draw(window);
            for (auto& door : doors) {
                door.draw(window);
            }
            for (const auto& enemy : enemies) {
                window.draw(enemy.sprite);
            }
            for (auto& bullet : bullets) {
                window.draw(bullet.getSprite());
            }
            for (const auto& item : MapItems) {
                if (!item.pickedUp) {
                    window.draw(item.sprite);
                }
            }
            window.draw(playerSprite);
            window.draw(playerItemSprite);
            window.setView(hudView);
            inventory.draw(window);
        } else {
            window.setView(window.getDefaultView());
            window.draw(winText);
            menu.buttons[0].changeButtonMode(false);
        }
    }
    window.display();
}