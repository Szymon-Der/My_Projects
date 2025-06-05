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

bool runLevelGeneric(sf::RenderWindow& window, sf::Font& font, const sf::Texture& bgTexture) {
    Character player("Characters/Character 9.png");
    NPC npc("Characters/Mushroom.png", true, false);

    float groundY = 550.f;
    player.setPosition(100.f, groundY - player.getGlobalBounds().height);
    npc.setPosition(300.f, groundY - npc.getGlobalBounds().height);

    sf::RectangleShape ground(sf::Vector2f(static_cast<float>(window.getSize().x), 170.f));
    ground.setFillColor(sf::Color(100, 250, 50));
    ground.setPosition(0.f, groundY);

    sf::RectangleShape sky(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
    sky.setFillColor(sf::Color(135, 206, 235)); // Jasnoniebieskie niebo

    sf::Sprite background;
    if (bgTexture.getSize().x > 0 && bgTexture.getSize().y > 0) {
        background.setTexture(bgTexture);
        float scaleX = window.getSize().x / static_cast<float>(bgTexture.getSize().x);
        float scaleY = window.getSize().y / static_cast<float>(bgTexture.getSize().y);
        background.setScale(scaleX, scaleY);
    } else {
        std::cerr << "Tło ma zerowy rozmiar!" << std::endl;
    }

    Timer timer(20); // 20 sekund

    // Dźwięk beep
    sf::SoundBuffer beepBuffer;
    beepBuffer.loadFromFile("Sounds/beep.wav");
    sf::Sound beepSound(beepBuffer);

    // Dźwięk game over
    sf::SoundBuffer gameOverBuffer;
    gameOverBuffer.loadFromFile("Sounds/gameover.wav");
    sf::Sound gameOverSound(gameOverBuffer);

    int lastSecond = timer.getFormattedTime() == "00:00" ? 0 : std::stoi(timer.getFormattedTime().substr(3));
    bool gameOverPlayed = false;

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(80);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("GAME OVER");
    sf::FloatRect textBounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin(textBounds.width / 2, textBounds.height / 2);
    gameOverText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);

    sf::Text exitHintText;
    exitHintText.setFont(font);
    exitHintText.setCharacterSize(28);
    exitHintText.setFillColor(sf::Color::White);
    exitHintText.setString("Press ESC to return to levels");
    sf::FloatRect hintBounds = exitHintText.getLocalBounds();
    exitHintText.setOrigin(hintBounds.width / 2, hintBounds.height / 2);
    exitHintText.setPosition(window.getSize().x / 2.f, window.getSize().y - 80.f);


    sf::Clock clock;
    bool backToMenu = false;
    bool paused = false;
    int pauseOption = 0;

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

    while (window.isOpen() && !backToMenu) {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                if (timer.getFormattedTime() == "00:00") {
                    backToMenu = true; // jeśli czas się skończył → wyjście do menu
                } else if (!paused) {
                    paused = true;
                    timer.pause();
                }
            }


            if (paused && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down)
                    pauseOption = 1 - pauseOption;
                if (event.key.code == sf::Keyboard::Enter) {
                    if (pauseOption == 0) {
                        paused = false;
                        timer.resume();
                    } else if (pauseOption == 1) backToMenu = true;
                }
            }
        }

        int remainingSeconds = 0;
        try {
            remainingSeconds = std::stoi(timer.getFormattedTime().substr(3));
        } catch (...) {}

        if (remainingSeconds != lastSecond) {
            if (remainingSeconds <= 10 && remainingSeconds > 0) {
                beepSound.play();
            } else if (remainingSeconds == 0 && !gameOverPlayed) {
                gameOverSound.play();
                gameOverPlayed = true;
            }
            lastSecond = remainingSeconds;
        }

        if (paused) {
            window.clear();
            window.draw(background); // Tło z bg.jpg podczas pauzy
            window.draw(pauseBox);
            resumeText.setFillColor(pauseOption == 0 ? sf::Color::Yellow : sf::Color::White);
            exitText.setFillColor(pauseOption == 1 ? sf::Color::Yellow : sf::Color::White);
            window.draw(resumeText);
            window.draw(exitText);
            window.display();
            continue;
        }

        // Sterowanie i fizyka
        int horizontal = 0;
        bool jump = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) horizontal = -1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) horizontal = 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) jump = true;

        if (player.getPosition().y + player.getGlobalBounds().height >= groundY) {
            player.setPosition(player.getPosition().x, groundY - player.getGlobalBounds().height);
            player.setGroundContact(true);
        } else player.setGroundContact(false);

        if (npc.getPosition().y + npc.getGlobalBounds().height >= groundY) {
            npc.setPosition(npc.getPosition().x, groundY - npc.getGlobalBounds().height);
            npc.setGroundContact(true);
        } else npc.setGroundContact(false);

        player.update(horizontal, jump, dt);
        npc.move(dt);
        npc.animate(dt);

        // Strzały NPC
        static float shootTimer = 0.f;
        shootTimer += dt;
        if (shootTimer > 3.f && npc.getBullets().size() < 10) {
            Bullet newBullet(100.f, npc.getPosition()); // zawsze w prawo
            npc.getBullets().push_back(newBullet);
            shootTimer = 0.f;
        }

        auto& bullets = npc.getBullets();
        for (auto& bullet : bullets) {
            bullet.update(dt);
            if (bullet.getPosition().x > window.getSize().x || bullet.getPosition().x < 0)
                bullet.deactive();
        }

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                     [](const Bullet& b) { return !b.getActive(); }),
                      bullets.end());

        window.clear();
        window.draw(sky);      // niebo w czasie gry
        window.draw(ground);   // trawa
        window.draw(player);
        window.draw(npc);

        for (const Bullet& bullet : npc.getBullets()) {
            if (bullet.getActive()) window.draw(bullet);
        }

        // Zegar
        sf::Text timerText;
        timerText.setFont(font);
        timerText.setCharacterSize(24);
        timerText.setFillColor(sf::Color::White);
        timerText.setString(timer.getFormattedTime());
        timerText.setPosition(window.getSize().x - 140.f, 10.f);
        window.draw(timerText);

        if (remainingSeconds == 0) {
            window.draw(gameOverText);
            window.draw(exitHintText);
        }

        window.display();
    }

    return !backToMenu;
}





bool runLevel1(sf::RenderWindow& window, sf::Font& font, const sf::Texture& bg) {
    return runLevelGeneric(window, font, bg);
}

bool runLevel2(sf::RenderWindow& window, sf::Font& font, const sf::Texture& bg) {
    return runLevelGeneric(window, font, bg);
}

bool runLevel3(sf::RenderWindow& window, sf::Font& font, const sf::Texture& bg) {
    return runLevelGeneric(window, font, bg);
}


