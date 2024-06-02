#include "game.hpp"
#include <cmath>
#include <ctime>
#include <iostream>
#include <fmt/core.h>

sf::Vector2f normalize(const sf::Vector2f &source) {
    float length = std::sqrt(source.x * source.x + source.y * source.y);
    if (length != 0)
        return {source.x / length, source.y / length};
    else
        return source;
}

Game::Game()
        : window(sf::VideoMode::getDesktopMode(), "Redemption-through-Rebellion", sf::Style::Fullscreen),
          currentLevel(0), gameWon(false), playerSpeed(700.0f), inventory(font),
          gameState(GameState::Menu), menu(window, font, "C:\\Users\\Akwar\\Documents\\Procesy\\Redemption-through-Rebellion\\res\\background.png"),
          useFirstSegment(true), animationFinished(false) {

    std::srand(static_cast<unsigned>(std::time(0)));

    if (!playerPrisonIdle.loadFromFile("C:\\Users\\Akwar\\Documents\\Procesy\\Redemption-through-Rebellion\\res\\player-prison-idle.png")) {
        fmt::print("Failed to load player texture\n");
        exit(-1);
    }
    if (!walkingTexture.loadFromFile("C:\\Users\\Akwar\\Documents\\Procesy\\Redemption-through-Rebellion\\res\\player-prison-walking.png")) {
        fmt::print("Failed to load walking texture\n");
        exit(-1);
    }
    loadAnimations();
    animatedSprite.setTexture(playerPrisonIdle);
    animatedSprite.setScale(5.0f, 5.0f);
    sf::FloatRect playerBounds = animatedSprite.getLocalBounds();
    animatedSprite.setOrigin(playerBounds.width / 2.0f, playerBounds.height / 2.0f);
    animatedSprite.setPosition(TILE_SIZE*4+-260, TILE_SIZE*2-400);

    playerHitbox.setRadius(playerBounds.width * 1.3f);
    playerHitbox.setOrigin(playerHitbox.getRadius(), playerHitbox.getRadius());
    playerHitbox.setPosition(animatedSprite.getPosition());
    playerHitbox.setFillColor(sf::Color::Transparent);
    playerHitbox.setOutlineColor(sf::Color::Red);
    playerHitbox.setOutlineThickness(1.0f);

    pickupRadius.setRadius(playerBounds.width * 2.5f);
    pickupRadius.setOrigin(pickupRadius.getRadius(), pickupRadius.getRadius());
    pickupRadius.setPosition(animatedSprite.getPosition());
    pickupRadius.setFillColor(sf::Color::Transparent);
    pickupRadius.setOutlineColor(sf::Color::Green);
    pickupRadius.setOutlineThickness(1.0f);

    if (!font.loadFromFile("C:\\Users\\Akwar\\Documents\\Procesy\\Redemption-through-Rebellion\\res\\Monocraft-nerd-fonts-patched.ttf")) {
        fmt::print("Failed to load font\n");
        exit(-1);
    }

    menu.addButton("Start", [this]() { loadGame(); });
    menu.addButton("New Game", [this]() { startNewGame(); });
    menu.addButton("Exit", [this]() { exitGame(); });

    winText = sf::Text("Demo Completed", font, 100);
    winText.setFillColor(sf::Color::Green);
    winText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
    winText.setOrigin(winText.getLocalBounds().width / 2.0f, winText.getLocalBounds().height / 2.0f);

    hudView.setSize(window.getSize().x, window.getSize().y);
    hudView.setCenter(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    initializePrisonLevel();
}

void Game::loadAnimations() {
    walkingAnimation.setSpriteSheet(walkingTexture);
    walkingAnimation.setFrameTime(sf::seconds(0.1f));

    int frameWidth = 32;
    int frameHeight = 32;
    for (int i = 0; i < 16; ++i) {
        walkingAnimation.addFrame(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    walkingAnimation.setSegment(0, 7);

    idleAnimation.setSpriteSheet(walkingTexture);
    idleAnimation.setFrameTime(sf::seconds(0.1f));
    idleAnimation.addFrame(sf::IntRect(0, 0, frameWidth, frameHeight));
    idleAnimation.addFrame(sf::IntRect(8 * frameWidth, 0, frameWidth, frameHeight));
}

void Game::initializePrisonLevel() {

    if (!prisonLevelTexture.loadFromFile("C:\\Users\\Akwar\\Documents\\Procesy\\Redemption-through-Rebellion\\res\\level-prison.png")) {
        fmt::print("Failed to load prison level texture\n");
    }
    prisonLevelSprite.setTexture(prisonLevelTexture);
    prisonLevelSprite.setPosition(0, 0);
    prisonLevelSprite.setScale(5.0f, 5.0f);;
    short columns = 14;
    short rows = 11;
    Level level(rows, columns, prisonLevelSprite);

    short numOf_Doors = 4;
    doorTexture.resize(numOf_Doors);
    for (int i=0; i<numOf_Doors; i++)
        if (!doorTexture[i].loadFromFile(fmt::format("C:\\Users\\Akwar\\Documents\\Procesy\\Redemption-through-Rebellion\\res\\door{}.png", i))) {
            fmt::println("Failed to load door texture: ", i);
        }

    items.clear();
    short numOf_Items = 3;
    PrisonLevelItemsTextures.resize(numOf_Items);
    std::vector<sf::Sprite> ITEM;
    for (int i = 0; i<numOf_Items; i++) {
        if (!PrisonLevelItemsTextures[i].loadFromFile(fmt::format("C:\\Users\\Akwar\\Documents\\Procesy\\Redemption-through-Rebellion\\res\\item{}.png", i))) {
            fmt::println("Failed to load item {} on prison level", i);
        }
        sf::Sprite tempSprite;
        tempSprite.setTexture(PrisonLevelItemsTextures[i]);
        tempSprite.setScale(5.0f, 5.0f);
        ITEM.push_back(tempSprite);
    }

    for (int col = 0; col < columns; ++col) {
        for (int row = 0; row < rows; ++row) {
            sf::Vector2f pos(col*128, row*128);
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
                        ITEM[0].setPosition(5*(pos.x+77),5*(pos.y+70));
                        items.emplace_back(ITEM[0], false);
                    } else if (col == 1){
                        tile->addBoundary(sf::FloatRect(pos.x+0, pos.y+24, 8, 72));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+88, 152, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+96, 8, 16));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+72), 5*(pos.y+114)), doorTexture[1], "S");
                    } else if (col == 2){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 8, 88));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y, 888, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+64, pos.y+88, 96, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+34), 5*(pos.y+92)), doorTexture[0], "E");
                    } else if (col == 3 || col == 4 || col == 5 || col == 6 || col == 7){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+8, 8, 80));
                        tile->addBoundary(sf::FloatRect(pos.x+64, pos.y+88, 96, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+34), 5*(pos.y+92)), doorTexture[0], "E");
                    } else if (col == 8){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+8, 8, 80));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+8, 8, 80));
                        tile->addBoundary(sf::FloatRect(pos.x+64, pos.y+88, 556, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+34), 5*(pos.y+92)), doorTexture[0], "E");
                        ITEM[1].setPosition(5*(pos.x+18),5*(pos.y+25));
                        items.emplace_back(ITEM[1], false);
                    } else if (col == 9){
                        tile->addBoundary(sf::FloatRect(pos.x+20, pos.y+96, 8, 15));
                        tile->addBoundary(sf::FloatRect(pos.x+100, pos.y+96, 8, 15));
                    } else if (col == 10){
                        tile->addBoundary(sf::FloatRect(pos.x-8, pos.y, 8, 88));
                        tile->addBoundary(sf::FloatRect(pos.x+84, pos.y+96, 8, 112));
                    } else if (col == 13){
                        tile->addBoundary(sf::FloatRect(pos.x-28, pos.y+96, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y, 8, 264));
                        doors.emplace_back(sf::Vector2f(5*(pos.x-24), 5*(pos.y+114)), doorTexture[3], "S");
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
                        doors.emplace_back(sf::Vector2f(5*(pos.x+94), 5*(pos.y+36)), doorTexture[0], "W");
                    } else if (col == 3){
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y+32, 96, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+40, 8, 80));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+94), 5*(pos.y+36)), doorTexture[0], "W");
                    } else if (col == 4){
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y+32, 32, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+40, 8, 208));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+94), 5*(pos.y+36)), doorTexture[0], "W");
                    } else if (col == 6){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+32, 8, 216));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+32, 56, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y+32, 96, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+120, 376, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+40, 8, 80));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+94), 5*(pos.y+36)), doorTexture[0], "W");
                    } else if (col == 7){
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y+32, 96, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+40, 8, 80));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+94), 5*(pos.y+36)), doorTexture[0], "W");
                    } else if (col == 8){
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y+32, 160, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+40, 8, 80));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+94), 5*(pos.y+36)), doorTexture[0], "W");
                    } else if (col == 9){
                        tile->addBoundary(sf::FloatRect(pos.x+20, pos.y+16, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+100, pos.y+16, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+40, 8, 224));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+24), 5*(pos.y+14)), doorTexture[2], "N");
                        doors.emplace_back(sf::Vector2f(5*(pos.x+104), 5*(pos.y+14)), doorTexture[2], "N");
                    } else if (col == 10){
                        tile->addBoundary(sf::FloatRect(pos.x+84, pos.y+112, 8, 24));
                        tile->addBoundary(sf::FloatRect(pos.x+92, pos.y+128, 420, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+88), 5*(pos.y+82)), doorTexture[3], "S");
                    } else if (col == 12){
                        tile->addBoundary(sf::FloatRect(pos.x+100, pos.y+16, 8, 112));
                        tile->addBoundary(sf::FloatRect(pos.x+128, pos.y+136, 8, 128));
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
                        doors.emplace_back(sf::Vector2f(5*(pos.x+78), 5*(pos.y+124)), doorTexture[1], "W");
                    } else if (col == 5){
                        tile->addBoundary(sf::FloatRect(pos.x+24, pos.y, 8, 120));
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y, 8, 120));
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y-8, 168, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+96, pos.y+120, 80, 8));
                    } else if (col == 6) {
                        doors.emplace_back(sf::Vector2f(5*(pos.x+50), 5*(pos.y+124)), doorTexture[1], "E");
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
                        doors.emplace_back(sf::Vector2f(5*(pos.x+50), 5*(pos.y+71)), doorTexture[3], "E");
                    }
                    break;
                case 5:
                    if (col == 11){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 152, 8));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+8, 8, 384));
                    } else if (col == 12){
                        tile->addBoundary(sf::FloatRect(pos.x+104, pos.y, 152, 8));
                    } else if (col == 13){
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+8, 8, 384));
                    }
                    break;
                case 6:
                    if (col == 0){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y-8, 264, 8));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 8, 264));
                    } else if (col == 2){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 8, 32));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+96, 8, 32));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+24, 112, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+96, 112, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+32, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+80, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+96, 8, 296));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+72), 5*(pos.y+78)), doorTexture[1], "N");
                    }
                    break;
                case 7:
                    if (col == 1){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 136, 8));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+8, 8, 120));
                    } else if (col == 9){
                        tile->addBoundary(sf::FloatRect(pos.x-8, pos.y-8, 8, 424));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y-8, 128, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+128, pos.y-8, 8, 264));
                        ITEM[2].setPosition(5*(pos.x+10),5*(pos.y+6));
                        items.emplace_back(ITEM[2], false);
                    }
                    break;
                case 8:
                    if (col == 0){
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y, 128, 8));
                    } else if (col == 3){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, 48, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+80, pos.y, 192, 8));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+128, 512, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+78), 5*(pos.y+4)), doorTexture[1], "W");
                    } else if (col == 4){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+8, 8, 120));
                    } else if (col == 5){
                        tile->addBoundary(sf::FloatRect(pos.x+64, pos.y, 240, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+62), 5*(pos.y+4)), doorTexture[1], "W");
                        doors.emplace_back(sf::Vector2f(5*(pos.x+2), 5*(pos.y+4)), doorTexture[1], "E");
                    } else if (col == 6){
                        tile->addBoundary(sf::FloatRect(pos.x+84, pos.y+8, 8, 48));
                        tile->addBoundary(sf::FloatRect(pos.x+84, pos.y+88, 8, 40));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+88), 5*(pos.y+58)), doorTexture[3], "S");
                    } else if (col == 7){
                        tile->addBoundary(sf::FloatRect(pos.x+80, pos.y, 48, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+50), 5*(pos.y+4)), doorTexture[3], "E");
                    } else if (col == 9){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+99, 48, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+80, pos.y+99, 48, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+50), 5*(pos.y+103)), doorTexture[3], "E");
                    } else if (col == 11){
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y, 120, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+8, 8, 120));
                        tile->addBoundary(sf::FloatRect(pos.x-120, pos.y+120, 240, 8));
                    } else if (col == 12){
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+36, 48, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+80, pos.y+36, 48, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+50), 5*(pos.y+40)), doorTexture[3], "E");
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
                        doors.emplace_back(sf::Vector2f(5*(pos.x+120), 5*(pos.y+82)), doorTexture[1], "S");
                    } else if (col == 11){
                        tile->addBoundary(sf::FloatRect(pos.x+48, pos.y+56, 144, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+64, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+112, 8, 32));
                        tile->addBoundary(sf::FloatRect(pos.x+76, pos.y+120, 108, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+72), 5*(pos.y+82)), doorTexture[1], "S");
                        doors.emplace_back(sf::Vector2f(5*(pos.x+18), 5*(pos.y+60)), doorTexture[3], "E");
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
                    } else if (col == 6){
                        tile->addBoundary(sf::FloatRect(pos.x+4, pos.y, 8, 64));
                        tile->addBoundary(sf::FloatRect(pos.x+4, pos.y+96, 8, 24));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+8), 5*(pos.y+94)), doorTexture[3], "N");
                    } else if (col == 8){
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+24, 112, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+96, 112, 8));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+96, 8, 24));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+96, 8, 24));
                        tile->addBoundary(sf::FloatRect(pos.x+120, pos.y+120, 648, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+52, pos.y+32, 8, 16));
                        tile->addBoundary(sf::FloatRect(pos.x+52, pos.y+80, 8, 16));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+56), 5*(pos.y+78)), doorTexture[3], "N");
                    } else if (col == 10){
                        tile->addBoundary(sf::FloatRect(pos.x+8, pos.y+56, 108, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+116, pos.y+48, 8, 32));
                        tile->addBoundary(sf::FloatRect(pos.x+116, pos.y+112, 8, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+120), 5*(pos.y+18)), doorTexture[1], "S");
                        doors.emplace_back(sf::Vector2f(5*(pos.x+120), 5*(pos.y+82)), doorTexture[1], "S");
                    } else if (col == 11){
                        tile->addBoundary(sf::FloatRect(pos.x+74, pos.y+56, 108, 8));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+48, 8, 32));
                        tile->addBoundary(sf::FloatRect(pos.x+68, pos.y+112, 8, 8));
                        doors.emplace_back(sf::Vector2f(5*(pos.x+72), 5*(pos.y+18)), doorTexture[1], "S");
                        doors.emplace_back(sf::Vector2f(5*(pos.x+72), 5*(pos.y+82)), doorTexture[1], "S");
                    }
                    break;
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
                    inventory.toggleInventory();
                }
                if (event.key.code == sf::Keyboard::F) {
                    for (auto& item : items) {
                        if (!item.pickedUp && pickupRadius.getGlobalBounds().intersects(item.sprite.getGlobalBounds())) {
                            if (inventory.addItem(item.sprite)) {
                                item.pickedUp = true;
                            }
                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Right) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
                    inventory.removeItem(0, worldPos); //change later
                }
            }
        }
    }
}

void Game::update(sf::Time deltaTime) {
    if (gameState == GameState::Playing) {
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

        sf::Vector2f normalizedMovement = normalize(movement) * playerSpeed * deltaTime.asSeconds();
        sf::Vector2f originalPosition = animatedSprite.getPosition();

        animatedSprite.move(normalizedMovement);
        playerHitbox.setPosition(animatedSprite.getPosition());
        pickupRadius.setPosition(animatedSprite.getPosition());

        if (levels[currentLevel].checkCollision(playerHitbox.getGlobalBounds())) {
            animatedSprite.setPosition(originalPosition);
            playerHitbox.setPosition(originalPosition);
            pickupRadius.setPosition(originalPosition);

            sf::Vector2f slideMovementX(normalizedMovement.x * 0.85f, 0.0f);
            animatedSprite.move(slideMovementX);
            playerHitbox.setPosition(animatedSprite.getPosition());
            pickupRadius.setPosition(animatedSprite.getPosition());
            if (levels[currentLevel].checkCollision(playerHitbox.getGlobalBounds())) {
                animatedSprite.move(-slideMovementX);
                playerHitbox.setPosition(animatedSprite.getPosition());
                pickupRadius.setPosition(animatedSprite.getPosition());
            }

            sf::Vector2f slideMovementY(0.0f, normalizedMovement.y * 0.85f);
            animatedSprite.move(slideMovementY);
            playerHitbox.setPosition(animatedSprite.getPosition());
            pickupRadius.setPosition(animatedSprite.getPosition());
            if (levels[currentLevel].checkCollision(playerHitbox.getGlobalBounds())) {
                animatedSprite.move(-slideMovementY);
                playerHitbox.setPosition(animatedSprite.getPosition());
                pickupRadius.setPosition(animatedSprite.getPosition());
            }
        }

        for (auto& door : doors) {
            door.update(playerHitbox.getGlobalBounds(), deltaTime);

            if (door.getBounds().intersects(playerHitbox.getGlobalBounds())) {
                animatedSprite.setPosition(originalPosition);
                playerHitbox.setPosition(originalPosition);
                pickupRadius.setPosition(originalPosition);

                sf::Vector2f slideMovementX(normalizedMovement.x * 1.f, 0.0f);
                animatedSprite.move(slideMovementX);
                playerHitbox.setPosition(animatedSprite.getPosition());
                pickupRadius.setPosition(animatedSprite.getPosition());
                if (door.getBounds().intersects(playerHitbox.getGlobalBounds())) {
                    animatedSprite.move(-slideMovementX);
                    playerHitbox.setPosition(animatedSprite.getPosition());
                    pickupRadius.setPosition(animatedSprite.getPosition());
                }

                sf::Vector2f slideMovementY(0.0f, normalizedMovement.y * 1.f);
                animatedSprite.move(slideMovementY);
                playerHitbox.setPosition(animatedSprite.getPosition());
                pickupRadius.setPosition(animatedSprite.getPosition());
                if (door.getBounds().intersects(playerHitbox.getGlobalBounds())) {
                    animatedSprite.move(-slideMovementY);
                    playerHitbox.setPosition(animatedSprite.getPosition());
                    pickupRadius.setPosition(animatedSprite.getPosition());
                }
            }
        }

        sf::View view = window.getView();
        view.setCenter(animatedSprite.getPosition());
        window.setView(view);

        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePosition);
        sf::Vector2f playerPos = animatedSprite.getPosition();
        sf::Vector2f direction = worldPos - playerPos;
        float angle = std::atan2(direction.y, direction.x) * 180 / 3.14159265;
        animatedSprite.setRotation(angle-90);

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
            animatedSprite.setTexture(walkingTexture);
            animatedSprite.setTextureRect(walkingAnimation.getFrame());
        } else {
            if (walkingAnimation.isPlaying()) {
                walkingAnimation.update(deltaTime);
                animatedSprite.setTexture(walkingTexture);
                animatedSprite.setTextureRect(walkingAnimation.getFrame());
                if (walkingAnimation.getCurrentFrameIndex() == walkingAnimation.getEndFrame()) {
                    animationFinished = true;
                    useFirstSegment = !useFirstSegment;
                    walkingAnimation.stop();
                }
            } else {
                if (animationFinished) {
                    if (useFirstSegment) {
                        animatedSprite.setTexture(walkingTexture);
                        animatedSprite.setTextureRect(walkingAnimation.getFrame(0));
                    } else {
                        animatedSprite.setTexture(walkingTexture);
                        animatedSprite.setTextureRect(walkingAnimation.getFrame(8));
                    }
                } else {
                    idleAnimation.update(deltaTime);
                    animatedSprite.setTexture(walkingTexture);
                    animatedSprite.setTextureRect(idleAnimation.getFrame());
                }
            }
        }
    }
}

void Game::render() {
    window.clear();
    if (gameState == GameState::Menu) {
        menu.draw();
    } else if (gameState == GameState::Playing) {
        if (!gameWon) {
            levels[currentLevel].draw(window);
            window.draw(animatedSprite);
//            window.draw(playerHitbox);
//            window.draw(pickupRadius);
            for (auto& door : doors) {
                door.draw(window);
            }
            for (const auto& item : items) {
                if (!item.pickedUp) {
                    window.draw(item.sprite);
                }
            }
            window.setView(hudView);
            inventory.draw(window);
        } else {
            window.setView(window.getDefaultView());
            window.draw(winText);
        }
    }
    window.display();
}

void Game::startNewGame() {
    gameState = GameState::Playing;
    // Initialize new game state later
}

void Game::loadGame() {
    gameState = GameState::Playing;
    // Load the game state from a file later
}

void Game::exitGame() {
    window.close();
}
