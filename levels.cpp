#include "levels.h"
#include "characters.h"
#include "timer.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>


LevelMap generateLevel(int levelNumber, const sf::RenderWindow& window) {
    LevelMap level;
    float groundY = 550.f;

    if (levelNumber == 1) {
        level.ground.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), 170.f));
        level.ground.setFillColor(sf::Color(100, 250, 50));
        level.ground.setPosition(0.f, groundY);
    } else if (levelNumber == 2) {
        level.ground.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), 150.f));
        level.ground.setFillColor(sf::Color(80, 180, 250));
        level.ground.setPosition(0.f, groundY + 20.f);

        sf::RectangleShape platform(sf::Vector2f(200.f, 20.f));
        platform.setPosition(400.f, 500.f);
        platform.setFillColor(sf::Color::Red);
        level.platforms.push_back(platform);
    } else if (levelNumber == 3) {
        level.ground.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), 120.f));
        level.ground.setFillColor(sf::Color(180, 100, 250));
        level.ground.setPosition(0.f, groundY + 50.f);

        sf::RectangleShape platform(sf::Vector2f(250.f, 20.f));
        platform.setPosition(600.f, 420.f);
        platform.setFillColor(sf::Color::Cyan);
        level.platforms.push_back(platform);
    }

    return level;
}

bool runLevel1(sf::RenderWindow& window, sf::Font& font) {
    //generowanie postaci
    Character player("Characters/Character 9.png");
    Character player2("Characters/Character 1.png");
    NPC npc("Characters/Mushroom.png", true, std::make_pair(300.f, 800.f));

    //ustawienie tymczasowej platformy
    float groundY = 550.f;
    player.setPosition(100.f, groundY - player.getGlobalBounds().height);
    player2.setPosition(100.f, groundY - player2.getGlobalBounds().height);
    npc.setPosition(300.f, groundY - npc.getGlobalBounds().height);

    sf::RectangleShape ground(sf::Vector2f(static_cast<float>(window.getSize().x), 170.f));
    ground.setFillColor(sf::Color(100, 250, 50,128));
    ground.setPosition(0.f, groundY);

    sf::Clock clock;
    bool backToMenu = false;
    bool paused = false;
    int pauseOption = 0;


    //sekwencja wczytania tla dla akutalnego lvl
    sf::Texture backgroundTexture;
    sf::Sprite background;
    bool hasBackground = false;

    if (backgroundTexture.loadFromFile("Levels_textures\\Background.png")) {
        hasBackground = true;
        background.setTexture(backgroundTexture);
        float w = background.getLocalBounds().width;
        float h = background.getLocalBounds().height;
        if (w > 0 && h > 0) {
            background.setScale(
                window.getSize().x / w,
                window.getSize().y / h
                );
        } else {
            hasBackground = false;
            std::cerr << "Tło ma zerowe wymiary." << std::endl;
        }
    } else {
        std::cerr << "Nie można załadować Images/bg.jpg" << std::endl;
    }

    sf::RectangleShape pauseBox(sf::Vector2f(300, 150));
    pauseBox.setFillColor(sf::Color(0, 0, 0, 200));
    pauseBox.setOutlineColor(sf::Color::White);
    pauseBox.setOutlineThickness(3);
    pauseBox.setPosition(window.getSize().x / 2.f - 150, window.getSize().y / 2.f - 75);

    sf::Font gatrich;
    gatrich.loadFromFile("Fonts/Gatrich.otf");

    sf::Text resumeText("Resume", gatrich, 28);
    sf::Text exitText("Exit", gatrich, 28);
    resumeText.setPosition(pauseBox.getPosition().x + 90, pauseBox.getPosition().y + 30);
    exitText.setPosition(pauseBox.getPosition().x + 110, pauseBox.getPosition().y + 80);


    //petla lvl1
    while (window.isOpen() && !backToMenu) {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) paused = !paused;

            if (paused && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down)
                    pauseOption = 1 - pauseOption;
                if (event.key.code == sf::Keyboard::Enter) {
                    if (pauseOption == 0) paused = false;
                    else if (pauseOption == 1) return false;
                }
            }
        }

        if (paused) {
            window.clear();
            if (backgroundTexture.getSize().x > 0) {
                window.draw(background);
            }

            window.draw(pauseBox);
            resumeText.setFillColor(pauseOption == 0 ? sf::Color::Yellow : sf::Color::White);
            exitText.setFillColor(pauseOption == 1 ? sf::Color::Yellow : sf::Color::White);
            window.draw(resumeText);
            window.draw(exitText);
            window.display();
            continue;
        }


        //contorlsy dla player
        int horizontalPlayer1 = 0;
        bool jumpPlayer1 = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) horizontalPlayer1 = -1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) horizontalPlayer1 = 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) jumpPlayer1 = true;

        //controlsy dla player2
        int horizontalPlayer2 = 0;
        bool jumpPlayer2 = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) horizontalPlayer2 = -1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) horizontalPlayer2 = 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) jumpPlayer2 = true;

        //aktulizacja stany player1
        player.update(horizontalPlayer1, jumpPlayer1, dt);

        //atulaizacja stany player2
        player2.update(horizontalPlayer2, jumpPlayer2, dt);

        //kontakt z podlozem dla player 1
        if (player.getPosition().y + player.getGlobalBounds().height >= groundY) {
            player.setPosition(player.getPosition().x, groundY - player.getGlobalBounds().height);
            player.setGroundContact(true);
        } else player.setGroundContact(false);

        if (npc.getPosition().y + npc.getGlobalBounds().height >= groundY) {
            npc.setPosition(npc.getPosition().x, groundY - npc.getGlobalBounds().height);
            npc.setGroundContact(true);
        } else npc.setGroundContact(false);

        // kontakt z podłożem dla player 2
        if (player2.getPosition().y + player2.getGlobalBounds().height >= groundY) {
            player2.setPosition(player2.getPosition().x, groundY - player2.getGlobalBounds().height);
            player2.setGroundContact(true);
        }
        else {
            player2.setGroundContact(false);
        }


        //aktualizacja stanu NPC
        npc.move(dt);
        npc.animate(dt);
        npc.shoot(dt);

        auto& bullets = npc.getBullets();
        for (auto& bullet : bullets) {
            bullet.update(dt);
            if (bullet.getPosition().x > window.getSize().x || bullet.getPosition().x < 0) {
                bullet.deactive();
            }
        }

        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) {
                return !b.getActive();
            }),
            bullets.end()
            );

        window.clear(sf::Color(50, 50, 50));
        if (hasBackground) window.draw(background);
        window.draw(ground);
        window.draw(player);
        window.draw(player2);
        window.draw(npc);
        for (auto& bullet : bullets) {
            window.draw(bullet);
        }
        window.display();
    }

    return true;
}

bool runLevel2(sf::RenderWindow& window, sf::Font& font) {
    return runLevel1(window, font); // tymczasowo
}

bool runLevel3(sf::RenderWindow& window, sf::Font& font) {
    return runLevel1(window, font); // tymczasowo
}


