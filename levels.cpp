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
    // Generowanie postaci
    Character player("Characters/Character 9.png");
    Character player2("Characters/Character 1.png");
    NPC npc("Characters/Mushroom.png", true, std::make_pair(100.f, 300.f));
    NPC npc2("Characters/Mushroom.png", false, std::make_pair(400.f, 600.f));
    NPC npc3("Characters/Mushroom.png", false, std::make_pair(200.f, 350.f));
    NPC npc4("Characters/Mushroom.png", false, std::make_pair(800.f, 1200.f));

    // Ustawienie początkowych pozycji postaci
    player.setPosition(100.f, 549.f);
    player2.setPosition(150.f, 549.f);
    npc.setPosition(150.f, 449.f);
    npc2.setPosition(550.f, 300.f);
    npc3.setPosition(250.f, 200.f);
    npc4.setPosition(1000.f, 300.f);

    sf::Clock clock;
    Timer levelTimer(50);  // 3 minuty = 180 sekund

    bool backToMenu = false;
    bool paused = false;
    bool gameOver = false; // Nowa flaga dla stanu GAME OVER
    int pauseOption = 0;
    int gameOverOption = 0; // Nowa opcja dla menu GAME OVER


    sf::Text timerText;
    timerText.setFont(font);  // Używa tej samej czcionki co reszta
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(20, 20);  // Dowolna pozycja w rogu

    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("Sounds/music.mp3")) {
        std::cerr << "Nie można załadować muzyki z Sounds/music.mp3\n";
    } else {
        backgroundMusic.setLoop(true);
        backgroundMusic.play();
    }

    // Sekwencja wczytania tła dla aktualnego poziomu
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

    // Wczytywanie tekstur i sprite'ów platform
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
    for (int y = 0; y < 6; y++) {
        for (int x = 0; x < 10; x++) {
            sf::Sprite tile = baseTile; // Kopiujemy bazowy sprite
            tile.setTextureRect(sf::IntRect(x * 32, y * 32, 32, 32));
            tiles[counter++] = tile;
        }
    }

    const int tileSize = 32;
    const int mapWidth = 40;  // Kolumny
    const int mapHeight = 23;   // Wymiary mapy, ile kafelków się zmieści na mapie


    // Wektor zapisanania poszczególnych kafelków, tymczasowo tutaj. Można przenieść do pliku txt
    std::vector<std::vector<int>> tileMap = {
        //   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39
        {13, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8,11}, // 0
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 1
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 2
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32,33,45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 3
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 33, 33, 34, 0,0,0,0,37,33,33,33,33,33,33,33,33,33,33,34, 0, 0, 0, 0,32,33,33,17}, // 4
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 5
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 33, 34, 0, 0,11}, // 6
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 7
        {27,33,33,33,33,33,33,33,33,33,33,33,34, 0, 0, 0, 0,32,33,33,52,33,33,34, 0, 0, 0, 0,32,33,33,33,33,33,33,33,33,33,33,17}, // 8
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 9
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 10
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 11
        {27,33,33,34, 0, 0, 0, 0,32,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,17}, // 12
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 13
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 14
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 15
        {27,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,34, 0, 0, 0, 0, 0,11}, // 16
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 17
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,17}, // 18
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 19
        {24, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,26}, // 20 - ziemia
        {12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12}, // 21
        {12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12}  // 22
    };

    // Na podstawie wektora tileMap utworzono wektor solidTiles, który będzie do sprawdzania kontaktu z kafelkami (0-brak kolizji, !0 kolizja)
    std::vector<sf::FloatRect> solidTiles;
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (tileMap[y][x] != 0) {
                solidTiles.emplace_back(x * tileSize, y * tileSize, tileSize, tileSize);
            }
        }
    }

    //obsluga ukonczenia poziomu
    sf::Vector2i targetTileCoordsPlayer1(21, 3); // Column, Row
    sf::Vector2i targetTileCoordsPlayer2(19, 2); // Column, Row

    // Create sf::RectangleShape for visual representation
    sf::RectangleShape targetRectPlayer1Visual(sf::Vector2f(tileSize, tileSize));
    targetRectPlayer1Visual.setPosition(targetTileCoordsPlayer1.x * tileSize, targetTileCoordsPlayer1.y * tileSize);
    targetRectPlayer1Visual.setFillColor(sf::Color(255, 165, 0, 100));

    sf::RectangleShape targetRectPlayer2Visual(sf::Vector2f(tileSize, tileSize));
    targetRectPlayer2Visual.setPosition(targetTileCoordsPlayer2.x * tileSize, targetTileCoordsPlayer2.y * tileSize);
    targetRectPlayer2Visual.setFillColor(sf::Color(0, 0, 255, 100));

    // Create sf::FloatRect for collision detection (same as before)
    sf::FloatRect targetRectPlayer1Collision(targetTileCoordsPlayer1.x * tileSize, targetTileCoordsPlayer1.y * tileSize, tileSize, tileSize);
    sf::FloatRect targetRectPlayer2Collision(targetTileCoordsPlayer2.x * tileSize, targetTileCoordsPlayer2.y * tileSize, tileSize, tileSize);

    bool player1OnTarget = false;
    bool player2OnTarget = false;
    bool levelCompleted = false;

    // Generowanie okna pauzowania
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

    // Teksty dla ekranu GAME OVER
    sf::Text gameOverText("GAME OVER", font, 50);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setOutlineColor(sf::Color::Black);
    gameOverText.setOutlineThickness(2);
    sf::FloatRect goBounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin(goBounds.width / 2.f, goBounds.height / 2.f);
    gameOverText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 80);

    sf::Text restartText("RESTART", font, 36);
    restartText.setFillColor(sf::Color::White);
    sf::FloatRect restartBounds = restartText.getLocalBounds();
    restartText.setOrigin(restartBounds.width / 2.f, restartBounds.height / 2.f);
    restartText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f + 20);

    sf::Text backToMenuText("BACK TO MENU", font, 36);
    backToMenuText.setFillColor(sf::Color::White);
    sf::FloatRect backToMenuBounds = backToMenuText.getLocalBounds();
    backToMenuText.setOrigin(backToMenuBounds.width / 2.f, backToMenuBounds.height / 2.f);
    backToMenuText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f + 80);

    // Wczytanie dźwięków
    sf::SoundBuffer beepBuf, gameoverBuf;
    if (!beepBuf.loadFromFile("Sounds/beep.wav")) std::cerr << "Nie można załadować beep.wav\n";
    if (!gameoverBuf.loadFromFile("Sounds/gameover.wav")) std::cerr << "Nie można załadować gameover.wav\n";
    sf::Sound beepSound(beepBuf), gameoverSound(gameoverBuf);

    int lastBeepSecond = -1;
    bool gameoverPlayed = false;


    //============================================================
    //=======================Pętla poziomu 1======================
    //============================================================
    while (window.isOpen() && !backToMenu) {
        float dt = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                if (!gameOver) { // Tylko jeśli gra nie jest w stanie GAME OVER
                    paused = !paused;
                    if (paused) {
                        backgroundMusic.pause();
                        levelTimer.pause();
                    } else {
                        backgroundMusic.play();
                        levelTimer.resume();
                    }
                }
            }

            if (gameOver && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) gameOverOption = (gameOverOption + 1) % 2;
                if (event.key.code == sf::Keyboard::Down) gameOverOption = (gameOverOption + 1) % 2;

                if (event.key.code == sf::Keyboard::Enter) {
                    if (gameOverOption == 0) { // RESTART
                        backgroundMusic.stop();
                        return true; // Wskazuje na restart poziomu
                    } else if (gameOverOption == 1) { // BACK TO MENU
                        backgroundMusic.stop();
                        return false; // Wskazuje na powrót do menu
                    }
                }
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

        if (gameOver) {
            window.clear();
            if (hasBackground) window.draw(background);
            window.draw(gameOverText);

            //restartText.setFillColor(gameOverOption == 0 ? sf::Color::Red : sf::Color::White);
            backToMenuText.setFillColor(gameOverOption == 1 ? sf::Color::Red : sf::Color::White);

            //window.draw(restartText);
            window.draw(backToMenuText);
            window.display();
            continue;
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

        int rem = levelTimer.getRemainingSeconds();
        // co sekundę przez ostatnie 10 sekund
        if (rem <= 10 && rem > 0 && rem != lastBeepSecond) {
            beepSound.play();
            lastBeepSecond = rem;
        }
        // na zero raz – dźwięk końca gry
        if (rem == 0 && !gameoverPlayed) {
            gameoverSound.play();
            gameoverPlayed = true;
        }

        // Sterowanie dla gracza 1
        int horizontalPlayer1 = 0;
        bool jumpPlayer1 = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) horizontalPlayer1 = -1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) horizontalPlayer1 = 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) jumpPlayer1 = true;

        // Sterowanie dla gracza 2
        int horizontalPlayer2 = 0;
        bool jumpPlayer2 = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) horizontalPlayer2 = -1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) horizontalPlayer2 = 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) jumpPlayer2 = true;

        // Aktualizacja stanu graczy
        player.update(horizontalPlayer1, jumpPlayer1, dt);
        player2.update(horizontalPlayer2, jumpPlayer2, dt);

        // Aktualizacja stanu NPC
        npc.update(dt);
        npc2.update(dt);
        npc3.update(dt);
        npc4.update(dt);

        // Interakcje ze ścianami dla graczy, pocisków i NPC
        resolveCollisions(player, solidTiles);
        resolveCollisions(player2, solidTiles);
        resolveCollisions(npc, solidTiles);
        resolveCollisions(npc2, solidTiles);
        resolveCollisions(npc3, solidTiles);
        resolveCollisions(npc4, solidTiles);

        // Kolizje pocisków z NPC
        for (auto& bullet : npc.getBullets()) {
            bullet.update(dt);
            resolveCollisions(bullet, solidTiles);
            // Sprawdzenie kolizji pocisku z graczem 1
            if (bullet.getGlobalBounds().intersects(player.getGlobalBounds())) {
                bullet.deactive();
                gameOver = true; // Ustaw flagę GAME OVER
                levelTimer.pause();
            }
            // Sprawdzenie kolizji pocisku z graczem 2
            if (bullet.getGlobalBounds().intersects(player2.getGlobalBounds())) {
                bullet.deactive();
                gameOver = true; // Ustaw flagę GAME OVER
                levelTimer.pause();
            }
        }

        //sprawdzanie kolizcji gracze npc
        sf::FloatRect player1Bounds = player.getGlobalBounds();
        sf::FloatRect npcBounds = npc.getGlobalBounds();
        sf::FloatRect npc2Bounds = npc2.getGlobalBounds();
        sf::FloatRect npc3Bounds = npc3.getGlobalBounds();
        sf::FloatRect npc4Bounds = npc4.getGlobalBounds();

        if (player1Bounds.intersects(npcBounds)) {
            float player1Bottom = player1Bounds.top + player1Bounds.height;
            float overlapY = player1Bottom - npcBounds.top;

            if (player.getVelocity().y > 0 && overlapY < 20.f) {
                player.setVelocity(player.getVelocity().x, -250.f);
                player.setGroundContact(false);
            } else {
                gameOver = true;
                levelTimer.pause();
            }
        }

        if (player1Bounds.intersects(npc2Bounds)) {
            float player1Bottom = player1Bounds.top + player1Bounds.height;
            float overlapY = player1Bottom - npc2Bounds.top;

            if (player.getVelocity().y > 0 && overlapY < 20.f) {
                player.setVelocity(player.getVelocity().x, -250.f);
                player.setGroundContact(false);
            } else {
                gameOver = true;
                levelTimer.pause();
            }
        }

        if (player1Bounds.intersects(npc3Bounds)) {
            float player1Bottom = player1Bounds.top + player1Bounds.height;
            float overlapY = player1Bottom - npc3Bounds.top;

            if (player.getVelocity().y > 0 && overlapY < 20.f) {
                player.setVelocity(player.getVelocity().x, -250.f);
                player.setGroundContact(false);
            } else {
                gameOver = true;
                levelTimer.pause();
            }
        }

        if (player1Bounds.intersects(npc4Bounds)) {
            float player1Bottom = player1Bounds.top + player1Bounds.height;
            float overlapY = player1Bottom - npc4Bounds.top;

            if (player.getVelocity().y > 0 && overlapY < 20.f) {
                player.setVelocity(player.getVelocity().x, -250.f);
                player.setGroundContact(false);
            } else {
                gameOver = true;
                levelTimer.pause();
            }
        }

        sf::FloatRect player2Bounds = player2.getGlobalBounds();

        if (player2Bounds.intersects(npcBounds)) {
            float player2Bottom = player2Bounds.top + player2Bounds.height;
            float overlapY = player2Bottom - npcBounds.top;

            if (player2.getVelocity().y > 0 && overlapY < 20.f) {
                player2.setVelocity(player2.getVelocity().x, -250.f);
                player2.setGroundContact(false);
            } else {
                gameOver = true;
                levelTimer.pause();
            }
        }

        if (player2Bounds.intersects(npc2Bounds)) {
            float player2Bottom = player2Bounds.top + player2Bounds.height;
            float overlapY = player2Bottom - npc2Bounds.top;

            if (player2.getVelocity().y > 0 && overlapY < 20.f) {
                player2.setVelocity(player2.getVelocity().x, -250.f);
                player2.setGroundContact(false);
            } else {
                gameOver = true;
                levelTimer.pause();
            }
        }

        if (player2Bounds.intersects(npc3Bounds)) {
            float player2Bottom = player2Bounds.top + player2Bounds.height;
            float overlapY = player2Bottom - npc3Bounds.top;

            if (player2.getVelocity().y > 0 && overlapY < 20.f) {
                player2.setVelocity(player2.getVelocity().x, -250.f);
                player2.setGroundContact(false);
            } else {
                gameOver = true;
                levelTimer.pause();
            }
        }

        if (player2Bounds.intersects(npc4Bounds)) {
            float player2Bottom = player2Bounds.top + player2Bounds.height;
            float overlapY = player2Bottom - npc4Bounds.top;

            if (player2.getVelocity().y > 0 && overlapY < 20.f) {
                player2.setVelocity(player2.getVelocity().x, -250.f);
                player2.setGroundContact(false);
            } else {
                gameOver = true;
                levelTimer.pause();
            }
        }


        //sprawdzanie warunkow ukonczenia poziomu. czy obie postacie sa na miejscach konczowcyh
        player1OnTarget = player.getGlobalBounds().intersects(targetRectPlayer1Collision);
        player2OnTarget = player2.getGlobalBounds().intersects(targetRectPlayer2Collision);
        if (player1OnTarget && player2OnTarget) {
            levelCompleted = true;
            levelTimer.pause();
        }


        // Aktualizacja stanu dla pocisków strzelanych przez NPC
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
        window.draw(npc2);
        window.draw(npc3);
        window.draw(npc4);
        for (auto& bullet : bullets) {
            window.draw(bullet);
        }

        window.draw(targetRectPlayer1Visual);
        window.draw(targetRectPlayer2Visual);

        timerText.setString("Time left: " + levelTimer.getFormattedTime());
        window.draw(timerText);


        window.display();
    }

    return true;
}

//============================================================
//===========================LVL2=============================
//============================================================

bool runLevel2(sf::RenderWindow& window, sf::Font& font) {
    // Generowanie postaci
    Character player("Characters/Character 9.png");
    Character player2("Characters/Character 1.png");
    NPC npc("Characters/Mushroom.png", true,true);

    // Ustawienie początkowych pozycji postaci
    player.setPosition(1200.f, 575.f);
    player2.setPosition(100.f, 590.f);
    npc.setPosition(65.f, 450.f);

    sf::Clock clock;
    Timer levelTimer(180);  // 3 minuty = 180 sekund

    bool backToMenu = false;
    bool paused = false;
    bool gameOver = false; // Nowa flaga dla stanu GAME OVER
    int pauseOption = 0;
    int gameOverOption = 0; // Nowa opcja dla menu GAME OVER


    sf::Text timerText;
    timerText.setFont(font);  // Używa tej samej czcionki co reszta
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(20, 20);  // Dowolna pozycja w rogu

    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("Sounds/music.mp3")) {
        std::cerr << "Nie można załadować muzyki z Sounds/music.mp3\n";
    } else {
        backgroundMusic.setLoop(true);
        backgroundMusic.play();
    }

    // Sekwencja wczytania tła dla aktualnego poziomu
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

    // Wczytywanie tekstur i sprite'ów platform
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
    for (int y = 0; y < 6; y++) {
        for (int x = 0; x < 10; x++) {
            sf::Sprite tile = baseTile; // Kopiujemy bazowy sprite
            tile.setTextureRect(sf::IntRect(x * 32, y * 32, 32, 32));
            tiles[counter++] = tile;
        }
    }

    const int tileSize = 32;
    const int mapWidth = 40;  // Kolumny
    const int mapHeight = 23;   // Wymiary mapy, ile kafelków się zmieści na mapie


    // Wektor zapisanania poszczególnych kafelków, tymczasowo tutaj. Można przenieść do pliku txt
    std::vector<std::vector<int>> tileMap = {
        //   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39
        {13, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8,11}, // 0
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 1
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 2
        {13, 0, 0, 0, 0, 0,32,33,33,33,33,33,33,33,33,33,33,33,33,33,35,33,33,33,33,33,33,33,33,33,33,33,33,33,34, 0, 0, 0, 0,11}, // 3
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 4
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,33,11}, // 5
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,31, 0, 0,11}, // 6
        {27,33,33,33,33,33,33,33,33,33,33,33,34, 0, 0, 0, 0,32,33,33,45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 7
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,55, 0, 0, 0, 0,32,33,33,33,33,33,33,33,33,33,33,33,33,33,17}, // 8
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 9
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 10
        {13, 0, 0, 0,32,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 11
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,37,33,33,33,33,33,33,33,33,33,33,33,33,34, 0, 0, 0,32,33,17}, // 12
        {27,33,34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 13
        {13, 0, 0,31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 14
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 15
        {27,33,33,33,33,33,33,33,33,33,33,33,33,33,33,34, 0, 0, 0, 0,55, 0, 0, 0, 0,32,33,33,33,33,33,33,33,33,33,33,33,33,33,17}, // 16
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 17
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,33,35,33,34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 18
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 19
        {24, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,28, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,26}, // 20 - ziemia
        {12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12}, // 21
        {12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12}  // 22
    };

    // Na podstawie wektora tileMap utworzono wektor solidTiles, który będzie do sprawdzania kontaktu z kafelkami (0-brak kolizji, !0 kolizja)
    std::vector<sf::FloatRect> solidTiles;
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (tileMap[y][x] != 0) {
                solidTiles.emplace_back(x * tileSize, y * tileSize, tileSize, tileSize);
            }
        }
    }

    //obsluga ukonczenia poziomu
    sf::Vector2i targetTileCoordsPlayer1(21, 2);
    sf::Vector2i targetTileCoordsPlayer2(19, 2);

    sf::RectangleShape targetRectPlayer1Visual(sf::Vector2f(tileSize, tileSize));
    targetRectPlayer1Visual.setPosition(targetTileCoordsPlayer1.x * tileSize, targetTileCoordsPlayer1.y * tileSize);
    targetRectPlayer1Visual.setFillColor(sf::Color(255, 165, 0, 100));

    sf::RectangleShape targetRectPlayer2Visual(sf::Vector2f(tileSize, tileSize));
    targetRectPlayer2Visual.setPosition(targetTileCoordsPlayer2.x * tileSize, targetTileCoordsPlayer2.y * tileSize);
    targetRectPlayer2Visual.setFillColor(sf::Color(0, 0, 255, 100));

    sf::FloatRect targetRectPlayer1Collision(targetTileCoordsPlayer1.x * tileSize, targetTileCoordsPlayer1.y * tileSize, tileSize, tileSize);
    sf::FloatRect targetRectPlayer2Collision(targetTileCoordsPlayer2.x * tileSize, targetTileCoordsPlayer2.y * tileSize, tileSize, tileSize);

    bool player1OnTarget = false;
    bool player2OnTarget = false;
    bool levelCompleted = false;

    // Generowanie okna pauzowania
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

    // Teksty dla ekranu GAME OVER
    sf::Text gameOverText("GAME OVER", font, 50);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setOutlineColor(sf::Color::Black);
    gameOverText.setOutlineThickness(2);
    sf::FloatRect goBounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin(goBounds.width / 2.f, goBounds.height / 2.f);
    gameOverText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 80);

    sf::Text restartText("RESTART", font, 36);
    restartText.setFillColor(sf::Color::White);
    sf::FloatRect restartBounds = restartText.getLocalBounds();
    restartText.setOrigin(restartBounds.width / 2.f, restartBounds.height / 2.f);
    restartText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f + 20);

    sf::Text backToMenuText("BACK TO MENU", font, 36);
    backToMenuText.setFillColor(sf::Color::White);
    sf::FloatRect backToMenuBounds = backToMenuText.getLocalBounds();
    backToMenuText.setOrigin(backToMenuBounds.width / 2.f, backToMenuBounds.height / 2.f);
    backToMenuText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f + 80);


    //============================================================
    //=======================Pętla poziomu 2======================
    //============================================================
    while (window.isOpen() && !backToMenu) {
        float dt = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                if (!gameOver) { // Tylko jeśli gra nie jest w stanie GAME OVER
                    paused = !paused;
                    if (paused) {
                        backgroundMusic.pause();
                        levelTimer.pause();
                    } else {
                        backgroundMusic.play();
                        levelTimer.resume();
                    }
                }
            }

            if (gameOver && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) gameOverOption = (gameOverOption + 1) % 2;
                if (event.key.code == sf::Keyboard::Down) gameOverOption = (gameOverOption + 1) % 2;

                if (event.key.code == sf::Keyboard::Enter) {
                    if (gameOverOption == 0) { // RESTART
                        backgroundMusic.stop();
                        return true; // Wskazuje na restart poziomu
                    } else if (gameOverOption == 1) { // BACK TO MENU
                        backgroundMusic.stop();
                        return false; // Wskazuje na powrót do menu
                    }
                }
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

        if (gameOver) {
            window.clear();
            if (hasBackground) window.draw(background);
            window.draw(gameOverText);

            //restartText.setFillColor(gameOverOption == 0 ? sf::Color::Red : sf::Color::White);
            backToMenuText.setFillColor(gameOverOption == 1 ? sf::Color::Red : sf::Color::White);

            //window.draw(restartText);
            window.draw(backToMenuText);
            window.display();
            continue;
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


        // Sterowanie dla gracza 1
        int horizontalPlayer1 = 0;
        bool jumpPlayer1 = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) horizontalPlayer1 = -1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) horizontalPlayer1 = 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) jumpPlayer1 = true;

        // Sterowanie dla gracza 2
        int horizontalPlayer2 = 0;
        bool jumpPlayer2 = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) horizontalPlayer2 = -1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) horizontalPlayer2 = 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) jumpPlayer2 = true;

        // Aktualizacja stanu graczy
        player.update(horizontalPlayer1, jumpPlayer1, dt);
        player2.update(horizontalPlayer2, jumpPlayer2, dt);

        // Aktualizacja stanu NPC
        npc.update(dt);

        // Interakcje ze ścianami dla graczy, pocisków i NPC
        resolveCollisions(player, solidTiles);
        resolveCollisions(player2, solidTiles);
        resolveCollisions(npc, solidTiles);

        // Kolizje pocisków z NPC
        for (auto& bullet : npc.getBullets()) {
            bullet.update(dt);
            resolveCollisions(bullet, solidTiles);
            // Sprawdzenie kolizji pocisku z graczem 1
            if (bullet.getGlobalBounds().intersects(player.getGlobalBounds())) {
                bullet.deactive();
                gameOver = true; // Ustaw flagę GAME OVER
                levelTimer.pause();
            }
            // Sprawdzenie kolizji pocisku z graczem 2
            if (bullet.getGlobalBounds().intersects(player2.getGlobalBounds())) {
                bullet.deactive();
                gameOver = true; // Ustaw flagę GAME OVER
                levelTimer.pause();
            }
        }

        //sprawdzanie kolizcji gracze npc
        sf::FloatRect player1Bounds = player.getGlobalBounds();
        sf::FloatRect npcBounds = npc.getGlobalBounds();

        if (player1Bounds.intersects(npcBounds)) {
            float player1Bottom = player1Bounds.top + player1Bounds.height;
            float overlapY = player1Bottom - npcBounds.top;

            if (player.getVelocity().y > 0 && overlapY < 20.f) {
                player.setVelocity(player.getVelocity().x, -250.f);
                player.setGroundContact(false);
            } else {
                gameOver = true;
                levelTimer.pause();
            }
        }

        sf::FloatRect player2Bounds = player2.getGlobalBounds();

        if (player2Bounds.intersects(npcBounds)) {
            float player2Bottom = player2Bounds.top + player2Bounds.height;
            float overlapY = player2Bottom - npcBounds.top;

            if (player2.getVelocity().y > 0 && overlapY < 20.f) {
                player2.setVelocity(player2.getVelocity().x, -250.f);
                player2.setGroundContact(false);
            } else {
                gameOver = true;
                levelTimer.pause();
            }
        }


        //sprawdzanie warunkow ukonczenia poziomu. czy obie postacie sa na miejscach konczowcyh
        player1OnTarget = player.getGlobalBounds().intersects(targetRectPlayer1Collision);
        player2OnTarget = player2.getGlobalBounds().intersects(targetRectPlayer2Collision);
        if (player1OnTarget && player2OnTarget) {
            levelCompleted = true;
            levelTimer.pause();
        }


        // Aktualizacja stanu dla pocisków strzelanych przez NPC
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
        for (auto& bullet : bullets) {
            window.draw(bullet);
        }

        window.draw(targetRectPlayer1Visual);
        window.draw(targetRectPlayer2Visual);

        timerText.setString("Time left: " + levelTimer.getFormattedTime());
        window.draw(timerText);


        window.display();
    }

    return true;
}



//============================================================
//===========================LVL3=============================
//============================================================



bool runLevel3(sf::RenderWindow& window, sf::Font& font) {
    // Generowanie postaci
    Character player("Characters/Character 9.png");
    Character player2("Characters/Character 1.png");
    NPC npc("Characters/Mushroom.png", false, std::make_pair(150.f, 800.f));

    // Ustawienie początkowych pozycji postaci
    player.setPosition(100.f, 550.f);
    player2.setPosition(100.f, 550.f);
    npc.setPosition(300.f, 450.f);

    sf::Clock clock;
    Timer levelTimer(180);  // 3 minuty = 180 sekund

    bool backToMenu = false;
    bool paused = false;
    bool gameOver = false; // Nowa flaga dla stanu GAME OVER
    int pauseOption = 0;
    int gameOverOption = 0; // Nowa opcja dla menu GAME OVER


    sf::Text timerText;
    timerText.setFont(font);  // Używa tej samej czcionki co reszta
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(20, 20);  // Dowolna pozycja w rogu

    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("Sounds/music.mp3")) {
        std::cerr << "Nie można załadować muzyki z Sounds/music.mp3\n";
    } else {
        backgroundMusic.setLoop(true);
        backgroundMusic.play();
    }

    // Sekwencja wczytania tła dla aktualnego poziomu
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

    // Wczytywanie tekstur i sprite'ów platform
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
    for (int y = 0; y < 6; y++) {
        for (int x = 0; x < 10; x++) {
            sf::Sprite tile = baseTile; // Kopiujemy bazowy sprite
            tile.setTextureRect(sf::IntRect(x * 32, y * 32, 32, 32));
            tiles[counter++] = tile;
        }
    }

    const int tileSize = 32;
    const int mapWidth = 40;  // Kolumny
    const int mapHeight = 23;   // Wymiary mapy, ile kafelków się zmieści na mapie


    // Wektor zapisanania poszczególnych kafelków, tymczasowo tutaj. Można przenieść do pliku txt
    std::vector<std::vector<int>> tileMap = {
        //   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39
        {13, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8,11}, // 0
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 1
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 2
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 3
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 4
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 5
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 6
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 7
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 8
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 9
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 10
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 11
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 12
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 13
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 14
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 15
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 16
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 17
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 18
        {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11}, // 19
        {24, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,26}, // 20 - ziemia
        {12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12}, // 21
        {12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12}  // 22
    };

    // Na podstawie wektora tileMap utworzono wektor solidTiles, który będzie do sprawdzania kontaktu z kafelkami (0-brak kolizji, !0 kolizja)
    std::vector<sf::FloatRect> solidTiles;
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (tileMap[y][x] != 0) {
                solidTiles.emplace_back(x * tileSize, y * tileSize, tileSize, tileSize);
            }
        }
    }

    //obsluga ukonczenia poziomu
    sf::Vector2i targetTileCoordsPlayer1(21, 1); // Column, Row
    sf::Vector2i targetTileCoordsPlayer2(19, 1); // Column, Row

    // Create sf::RectangleShape for visual representation
    sf::RectangleShape targetRectPlayer1Visual(sf::Vector2f(tileSize, tileSize));
    targetRectPlayer1Visual.setPosition(targetTileCoordsPlayer1.x * tileSize, targetTileCoordsPlayer1.y * tileSize);
    targetRectPlayer1Visual.setFillColor(sf::Color(255, 165, 0, 100));

    sf::RectangleShape targetRectPlayer2Visual(sf::Vector2f(tileSize, tileSize));
    targetRectPlayer2Visual.setPosition(targetTileCoordsPlayer2.x * tileSize, targetTileCoordsPlayer2.y * tileSize);
    targetRectPlayer2Visual.setFillColor(sf::Color(0, 0, 255, 100));

    // Create sf::FloatRect for collision detection (same as before)
    sf::FloatRect targetRectPlayer1Collision(targetTileCoordsPlayer1.x * tileSize, targetTileCoordsPlayer1.y * tileSize, tileSize, tileSize);
    sf::FloatRect targetRectPlayer2Collision(targetTileCoordsPlayer2.x * tileSize, targetTileCoordsPlayer2.y * tileSize, tileSize, tileSize);

    bool player1OnTarget = false;
    bool player2OnTarget = false;
    bool levelCompleted = false;

    // Generowanie okna pauzowania
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

    // Teksty dla ekranu GAME OVER
    sf::Text gameOverText("GAME OVER", font, 50);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setOutlineColor(sf::Color::Black);
    gameOverText.setOutlineThickness(2);
    sf::FloatRect goBounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin(goBounds.width / 2.f, goBounds.height / 2.f);
    gameOverText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 80);

    sf::Text restartText("RESTART", font, 36);
    restartText.setFillColor(sf::Color::White);
    sf::FloatRect restartBounds = restartText.getLocalBounds();
    restartText.setOrigin(restartBounds.width / 2.f, restartBounds.height / 2.f);
    restartText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f + 20);

    sf::Text backToMenuText("BACK TO MENU", font, 36);
    backToMenuText.setFillColor(sf::Color::White);
    sf::FloatRect backToMenuBounds = backToMenuText.getLocalBounds();
    backToMenuText.setOrigin(backToMenuBounds.width / 2.f, backToMenuBounds.height / 2.f);
    backToMenuText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f + 80);


    //============================================================
    //=======================Pętla poziomu 2======================
    //============================================================
    while (window.isOpen() && !backToMenu) {
        float dt = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                if (!gameOver) { // Tylko jeśli gra nie jest w stanie GAME OVER
                    paused = !paused;
                    if (paused) {
                        backgroundMusic.pause();
                        levelTimer.pause();
                    } else {
                        backgroundMusic.play();
                        levelTimer.resume();
                    }
                }
            }

            if (gameOver && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) gameOverOption = (gameOverOption + 1) % 2;
                if (event.key.code == sf::Keyboard::Down) gameOverOption = (gameOverOption + 1) % 2;

                if (event.key.code == sf::Keyboard::Enter) {
                    if (gameOverOption == 0) { // RESTART
                        backgroundMusic.stop();
                        return true; // Wskazuje na restart poziomu
                    } else if (gameOverOption == 1) { // BACK TO MENU
                        backgroundMusic.stop();
                        return false; // Wskazuje na powrót do menu
                    }
                }
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

        if (gameOver) {
            window.clear();
            if (hasBackground) window.draw(background);
            window.draw(gameOverText);

            //restartText.setFillColor(gameOverOption == 0 ? sf::Color::Red : sf::Color::White);
            backToMenuText.setFillColor(gameOverOption == 1 ? sf::Color::Red : sf::Color::White);

            //window.draw(restartText);
            window.draw(backToMenuText);
            window.display();
            continue;
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


        // Sterowanie dla gracza 1
        int horizontalPlayer1 = 0;
        bool jumpPlayer1 = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) horizontalPlayer1 = -1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) horizontalPlayer1 = 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) jumpPlayer1 = true;

        // Sterowanie dla gracza 2
        int horizontalPlayer2 = 0;
        bool jumpPlayer2 = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) horizontalPlayer2 = -1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) horizontalPlayer2 = 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) jumpPlayer2 = true;

        // Aktualizacja stanu graczy
        player.update(horizontalPlayer1, jumpPlayer1, dt);
        player2.update(horizontalPlayer2, jumpPlayer2, dt);

        // Aktualizacja stanu NPC
        npc.update(dt);

        // Interakcje ze ścianami dla graczy, pocisków i NPC
        resolveCollisions(player, solidTiles);
        resolveCollisions(player2, solidTiles);
        resolveCollisions(npc, solidTiles);

        // Kolizje pocisków z NPC
        for (auto& bullet : npc.getBullets()) {
            bullet.update(dt);
            resolveCollisions(bullet, solidTiles);
            // Sprawdzenie kolizji pocisku z graczem 1
            if (bullet.getGlobalBounds().intersects(player.getGlobalBounds())) {
                bullet.deactive();
                gameOver = true; // Ustaw flagę GAME OVER
                levelTimer.pause();
            }
            // Sprawdzenie kolizji pocisku z graczem 2
            if (bullet.getGlobalBounds().intersects(player2.getGlobalBounds())) {
                bullet.deactive();
                gameOver = true; // Ustaw flagę GAME OVER
                levelTimer.pause();
            }
        }

        //sprawdzanie kolizcji gracze npc
        sf::FloatRect player1Bounds = player.getGlobalBounds();
        sf::FloatRect npcBounds = npc.getGlobalBounds();

        if (player1Bounds.intersects(npcBounds)) {
            float player1Bottom = player1Bounds.top + player1Bounds.height;
            float overlapY = player1Bottom - npcBounds.top;

            if (player.getVelocity().y > 0 && overlapY < 20.f) {
                player.setVelocity(player.getVelocity().x, -250.f);
                player.setGroundContact(false);
            } else {
                gameOver = true;
                levelTimer.pause();
            }
        }

        sf::FloatRect player2Bounds = player2.getGlobalBounds();

        if (player2Bounds.intersects(npcBounds)) {
            float player2Bottom = player2Bounds.top + player2Bounds.height;
            float overlapY = player2Bottom - npcBounds.top;

            if (player2.getVelocity().y > 0 && overlapY < 20.f) {
                player2.setVelocity(player2.getVelocity().x, -250.f);
                player2.setGroundContact(false);
            } else {
                gameOver = true;
                levelTimer.pause();
            }
        }


        //sprawdzanie warunkow ukonczenia poziomu. czy obie postacie sa na miejscach konczowcyh
        player1OnTarget = player.getGlobalBounds().intersects(targetRectPlayer1Collision);
        player2OnTarget = player2.getGlobalBounds().intersects(targetRectPlayer2Collision);
        if (player1OnTarget && player2OnTarget) {
            levelCompleted = true;
            levelTimer.pause();
        }


        // Aktualizacja stanu dla pocisków strzelanych przez NPC
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
        for (auto& bullet : bullets) {
            window.draw(bullet);
        }

        window.draw(targetRectPlayer1Visual);
        window.draw(targetRectPlayer2Visual);

        timerText.setString("Time left: " + levelTimer.getFormattedTime());
        window.draw(timerText);


        window.display();
    }

    return true;
}
