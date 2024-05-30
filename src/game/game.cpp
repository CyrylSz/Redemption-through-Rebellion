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
          currentLevel(0), gameWon(false), playerSpeed(800.0f), inventory(font),
          gameState(GameState::Menu), menu(window, font, "C:\\Users\\Akwar\\Documents\\Procesy\\Redemption-through-Rebellion\\res\\background.png") {

    std::srand(static_cast<unsigned>(std::time(0)));

    if (!playerTexture.loadFromFile("C:\\Users\\Akwar\\Documents\\Procesy\\Redemption-through-Rebellion\\res\\player-prison.png")) {
        fmt::print("Failed to load player texture\n");
    }
    playerSprite.setTexture(playerTexture);
    playerSprite.setScale(5.0f, 5.0f);
    sf::FloatRect playerBounds = playerSprite.getLocalBounds();
    playerSprite.setOrigin(playerBounds.width / 2.0f, playerBounds.height / 2.0f);
    playerSprite.setPosition(window.getSize().x / 2, window.getSize().y / 2);

    playerHitbox.setRadius(playerBounds.width * 1.4f);
    playerHitbox.setOrigin(playerHitbox.getRadius(), playerHitbox.getRadius());
    playerHitbox.setPosition(playerSprite.getPosition());
    playerHitbox.setFillColor(sf::Color::Transparent);
    playerHitbox.setOutlineColor(sf::Color::Red);
    playerHitbox.setOutlineThickness(1.0f);

    pickupRadius.setRadius(playerBounds.width * 2.5f);
    pickupRadius.setOrigin(pickupRadius.getRadius(), pickupRadius.getRadius());
    pickupRadius.setPosition(playerSprite.getPosition());
    pickupRadius.setFillColor(sf::Color::Transparent);
    pickupRadius.setOutlineColor(sf::Color::Green);
    pickupRadius.setOutlineThickness(1.0f);

    loadTextures();

    if (!font.loadFromFile("C:\\Users\\Akwar\\Documents\\Procesy\\Redemption-through-Rebellion\\res\\Monocraft-nerd-fonts-patched.ttf")) {
        fmt::print("Failed to load font\n");
        exit(-1);
    }

    menu.addButton("Start", [this]() { loadGame(); });
    menu.addButton("New Game", [this]() { startNewGame(); });
    menu.addButton("Exit", [this]() { exitGame(); });

    winText = sf::Text("You won!", font, 100);
    winText.setFillColor(sf::Color::Green);
    winText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
    winText.setOrigin(winText.getLocalBounds().width / 2.0f, winText.getLocalBounds().height / 2.0f);

    hudView.setSize(window.getSize().x, window.getSize().y);
    hudView.setCenter(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    generateItems(itemTextures, 20);

    initializeTestLevel();
}

void Game::loadTextures() {
    itemTextures.resize(NUM_ITEMS);
    for (int i = 0; i < NUM_ITEMS; i++) {
        if (!itemTextures[i].loadFromFile(fmt::format("C:\\Users\\Akwar\\Documents\\Procesy\\Redemption-through-Rebellion\\res\\item{}.png", i+1))) {
            fmt::println("Failed to load item {}", i+1);
            exit(-1);
        } else {
            fmt::println("Successfully item texture {}",i+1);
        }
    }
}

void Game::generateItems(const std::vector<sf::Texture>& textures, int numItems) {
    items.clear();
    for (int i = 0; i < numItems; i++) {
        sf::Sprite itemSprite;
        itemSprite.setTexture(textures[std::rand() % textures.size()]);
        itemSprite.setPosition(static_cast<float>(std::rand() % window.getSize().x), static_cast<float>(std::rand() % window.getSize().y));
        itemSprite.setScale(5.0f, 5.0f);
        items.push_back({ itemSprite, false });
    }
}

void Game::initializeTestLevel() {

    Level level(5, 5);

    std::vector<std::string> textureFiles;
    for (int i=0; i<9; i++)
        textureFiles.push_back("C:\\Users\\Akwar\\Documents\\Procesy\\Redemption-through-Rebellion\\res\\tile"+std::to_string(i+1)+".png");

    int textureIndex = 0;
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 5; ++col) {
            if (row == 2 || col == 2) {
                sf::Vector2f pos(col * TILE_SIZE, row * TILE_SIZE);
                Tile* tile = new Tile(pos, textureFiles[textureIndex], true);
                short thickness = 15;

                switch (row) {
                    case 0:
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y, TILE_SIZE, thickness));
                        tile->addBoundary(sf::FloatRect(pos.x, pos.y+15, thickness, TILE_SIZE));
                        tile->addBoundary(sf::FloatRect(pos.x+TILE_SIZE-15, pos.y, thickness, TILE_SIZE));
                }


                level.setTile(row, col, tile);
                textureIndex = (textureIndex + 1) % textureFiles.size();
            }
        }
    }
    levels.push_back(level);
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        processEvents();
        sf::Time deltaTime = clock.restart();
        if (!gameWon) {
            update(deltaTime);
        }
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
        sf::Vector2f originalPosition = playerSprite.getPosition();

        playerSprite.move(normalizedMovement);
        playerHitbox.setPosition(playerSprite.getPosition());
        pickupRadius.setPosition(playerSprite.getPosition());

        if (levels[currentLevel].checkCollision(playerHitbox.getGlobalBounds())) {
            playerSprite.setPosition(originalPosition);
            playerHitbox.setPosition(originalPosition);
            pickupRadius.setPosition(originalPosition);

            sf::Vector2f slideMovementX(normalizedMovement.x * 0.85f, 0.0f);
            playerSprite.move(slideMovementX);
            playerHitbox.setPosition(playerSprite.getPosition());
            pickupRadius.setPosition(playerSprite.getPosition());
            if (levels[currentLevel].checkCollision(playerHitbox.getGlobalBounds())) {
                playerSprite.move(-slideMovementX);
                playerHitbox.setPosition(playerSprite.getPosition());
                pickupRadius.setPosition(playerSprite.getPosition());
            }

            sf::Vector2f slideMovementY(0.0f, normalizedMovement.y * 0.85f);
            playerSprite.move(slideMovementY);
            playerHitbox.setPosition(playerSprite.getPosition());
            pickupRadius.setPosition(playerSprite.getPosition());
            if (levels[currentLevel].checkCollision(playerHitbox.getGlobalBounds())) {
                playerSprite.move(-slideMovementY);
                playerHitbox.setPosition(playerSprite.getPosition());
                pickupRadius.setPosition(playerSprite.getPosition());
            }
        }

        sf::View view = window.getView();
        view.setCenter(playerSprite.getPosition());
        window.setView(view);

        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePosition);
        sf::Vector2f playerPos = playerSprite.getPosition();
        sf::Vector2f direction = worldPos - playerPos;
        float angle = std::atan2(direction.y, direction.x) * 180 / 3.14159265;
        playerSprite.setRotation(angle-90);
    }
}

void Game::render() {
    window.clear();
    if (gameState == GameState::Menu) {
        menu.draw();
    } else if (gameState == GameState::Playing) {
        if (!gameWon) {
            levels[currentLevel].draw(window);
            window.draw(playerSprite);
            //window.draw(playerHitbox);
            //window.draw(pickupRadius);
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
