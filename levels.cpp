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
    Timer levelTimer(180);  // 3 minuty = 180 sekund

    bool backToMenu = false;
    bool paused = false;
    int pauseOption = 0;

    sf::Text timerText;
    timerText.setFont(font);  // używa tej samej czcionki co reszta
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(20, 20);  // dowolna pozycja w rogu



    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("Sounds/music.mp3")) {
        std::cerr << "Nie można załadować muzyki z Sounds/music.mp3\n";
    } else {
        backgroundMusic.setLoop(true);
        backgroundMusic.play();
    }


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

    //wczystywanie tesktur i spirte'ow platform
    sf::Texture tilesSet;
    if(tilesSet.loadFromFile("Levels_textures\\Tileset.png")){
    }
    else{
        std::cerr << "Blad ladowania kafelkow" << std::endl;
    }

    std::map<int, sf::Sprite> tiles;
    sf::Sprite baseTile;
    baseTile.setTexture(tilesSet);

    int counter = 1;
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 6; x++) {
            sf::Sprite tile = baseTile; // kopiujemy bazowy sprite
            tile.setTextureRect(sf::IntRect(x * 32, y * 32, 32, 32));
            //tile.setScale(2.f,2.f);
            tiles[counter++] = tile;
        }
    }

    const int tileSize = 32;
    const int mapWidth = 40;  // kolumny
    const int mapHeight = 23;   //wymairy mapy, ile kafelkow sie zmiesci na mapie


    //wektor zpaisanai poszczegolnych kafelkow, tymoczasowo tutaj. mozna przeniesc to pliku txt
    std::vector<std::vector<int>> tileMap = {
        {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // 1
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 2
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 3
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 4
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 5
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 6
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 7
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 8
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 9
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 10
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 11
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 12
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 13
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 14
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 15
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 16
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 17
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 18
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 19
        {1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,4,1,1,1,0,0,1}, // 20 - ziemia
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},  // 21 - gleba
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
    };



    //generowanie okna pauzowania
    sf::RectangleShape pauseBox(sf::Vector2f(300, 200));
    pauseBox.setFillColor(sf::Color(0, 0, 0, 200));
    pauseBox.setOutlineColor(sf::Color::White);
    pauseBox.setOutlineThickness(3);
    pauseBox.setPosition(window.getSize().x / 2.f - 150, window.getSize().y / 2.f - 100);

    sf::Text resumeText("Resume", font, 28);
    sf::Text volumeText("Volume", font, 28);
    sf::Text exitText("Exit", font, 28);
    resumeText.setPosition(pauseBox.getPosition().x + 90, pauseBox.getPosition().y + 20);
    volumeText.setPosition(pauseBox.getPosition().x + 90, pauseBox.getPosition().y + 70);
    exitText.setPosition(pauseBox.getPosition().x + 110, pauseBox.getPosition().y + 130);

    sf::RectangleShape volumeBar(sf::Vector2f(150, 10));
    volumeBar.setFillColor(sf::Color::White);
    volumeBar.setPosition(pauseBox.getPosition().x + pauseBox.getSize().x / 2.f - volumeBar.getSize().x / 2.f,
                          volumeText.getPosition().y + 35);

    sf::CircleShape volumeKnob(7);
    volumeKnob.setFillColor(sf::Color::Red);

    const int pauseOptionsCount = 3;
    float volume = 50.f;  // Domyślna głośność
    backgroundMusic.setVolume(volume);

    //============================================================
    //=======================petla lvl1===========================
    //============================================================
    while (window.isOpen() && !backToMenu) {
        float dt = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape && !paused) {
                paused = true;
                backgroundMusic.pause();
                levelTimer.pause();
            }

            if (paused && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) pauseOption = (pauseOption + pauseOptionsCount - 1) % pauseOptionsCount;
                if (event.key.code == sf::Keyboard::Down) pauseOption = (pauseOption + 1) % pauseOptionsCount;

                if (pauseOption == 1) { // Volume
                    if (event.key.code == sf::Keyboard::Left && volume > 0.f) {
                        volume = std::max(0.f, volume - 5.f);
                        backgroundMusic.setVolume(volume);
                    }
                    if (event.key.code == sf::Keyboard::Right && volume < 100.f) {
                        volume = std::min(100.f, volume + 5.f);
                        backgroundMusic.setVolume(volume);
                    }
                }

                if (event.key.code == sf::Keyboard::Enter) {
                    if (pauseOption == 0) {
                        paused = false;
                        backgroundMusic.play();
                        levelTimer.resume();
                    }
                    else if (pauseOption == 2) {
                        backgroundMusic.stop();
                        return false;
                    }
                }
            }
        }

        if (paused) {
            window.clear();
            if (hasBackground) window.draw(background);
            window.draw(pauseBox);

            resumeText.setFillColor(pauseOption == 0 ? sf::Color::Yellow : sf::Color::White);
            volumeText.setFillColor(pauseOption == 1 ? sf::Color::Yellow : sf::Color::White);
            exitText.setFillColor(pauseOption == 2 ? sf::Color::Yellow : sf::Color::White);

            window.draw(resumeText);
            window.draw(volumeText);

            window.draw(volumeBar);
            volumeKnob.setPosition(
                volumeBar.getPosition().x + (volume / 100.f) * volumeBar.getSize().x - volumeKnob.getRadius(),
                volumeBar.getPosition().y + volumeBar.getSize().y / 2.f - volumeKnob.getRadius()
                );

            window.draw(volumeKnob);

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

        //aktualizacja stanu NPC
        npc.update(dt);

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

        //aktualizacja stanu dla pocskow strzelanych przez NPC
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

        for (int y = 0; y < mapHeight; y++) {
            for (int x = 0; x < mapWidth; x++) {
                int tileNumber = tileMap[y][x];
                if (tileNumber != 0) {
                    if (tiles.count(tileNumber)) {
                        sf::Sprite& sprite = tiles[tileNumber];
                        sprite.setPosition(x * tileSize, y * tileSize);
                        window.draw(sprite);
                    }
                }
            }
        }
        window.draw(player);
        window.draw(player2);
        window.draw(npc);
        //window.draw(tile1);
        for (auto& bullet : bullets) {
            window.draw(bullet);
        }
        timerText.setString("Time left: " + levelTimer.getFormattedTime());
        window.draw(timerText);


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


