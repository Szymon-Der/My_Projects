#include <SFML/Graphics.hpp>
#include "menu.h"
#include "levels.h"
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

    while (window.isOpen()) {
        // uruchamiamy menu – tylko raz pobieramy nazwę gracza
        if (!menu.run(firstRun)) break;
        firstRun = false;

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
            case 1: goBackToLevelSelect = runLevel1(window, font, bg); break;
            case 2: goBackToLevelSelect = runLevel2(window, font, bg); break;
            case 3: goBackToLevelSelect = runLevel3(window, font, bg); break;
            default: break;
            }

            // Jeśli ESC z poziomu — wróć do MENU, nie level select
            if (!goBackToLevelSelect) break;
        }
    }

    return 0;
}
