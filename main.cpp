#include <SFML/Graphics.hpp>
#include "menu.h"
#include "levels.h"
#include "playtime.h" // <-- dodane, żeby działało updatePlayTime
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Dual Power");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("Fonts/Gatrich.otf")) {
        return -1;
    }

    Menu menu(window, font);
    bool firstRun = true;

    std::string playerName;
    sf::Clock playClock;          // zegar mierzący czas gry
    bool clockStarted = false;    // flaga: czy zegar został uruchomiony

    while (window.isOpen()) {
        // pokazuje menu, przy pierwszym uruchomieniu umożliwia wpisanie nazwy gracza
        if (!menu.run(firstRun)) break;
        firstRun = false;

        playerName = menu.getPlayerName();

        // uruchom zegar dopiero po wpisaniu nazwy gracza (tylko raz!)
        if (!clockStarted && !playerName.empty()) {
            playClock.restart();
            clockStarted = true;
        }

        while (window.isOpen()) {
            int selectedLevel = menu.selectLevel();
            if (selectedLevel == -1) break; // kliknięto ESC – wróć do menu

            sf::Texture bg;
            if (!bg.loadFromFile("Images/bg.jpg")) {
                std::cerr << "Nie udało się załadować tła\n";
                return -1;
            }

            bool goBackToLevelSelect = false;

            switch (selectedLevel) {
            case 1: goBackToLevelSelect = runLevel1(window, font, playerName); break;
            case 2: goBackToLevelSelect = runLevel2(window, font, playerName); break;
            case 3: goBackToLevelSelect = runLevel3(window, font, playerName); break;
            default: break;
            }

            // Jeśli ESC z poziomu poziomu – wróć do MENU, nie level select
            if (!goBackToLevelSelect) break;
        }
    }

    // po zamknięciu programu – zapisz czas gry (jeśli był liczony)
    if (clockStarted && !playerName.empty()) {
        int playTime = static_cast<int>(playClock.getElapsedTime().asSeconds());
        updatePlayTime(playerName, playTime);
    }

    return 0;
}
