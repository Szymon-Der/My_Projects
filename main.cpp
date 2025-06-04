
// main.cpp
#include <SFML/Graphics.hpp>
#include "menu.h"
#include "characters.h"
#include "levels.h" // Dodano

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Dual Power", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("Fonts/Gatrich.otf")) {
        return -1;
    }

    while (window.isOpen()) {
        Menu menu(window, font);
        if (!menu.run()) break;

        while (window.isOpen()) {
            int selectedLevel = menu.selectLevel();
            if (selectedLevel == -1) break;

            bool exitToMenu = false;
            switch (selectedLevel) {
            case 1: exitToMenu = runLevel1(window, font); break;
            case 2: exitToMenu = runLevel2(window, font); break;
            case 3: exitToMenu = runLevel3(window, font); break;
            default: exitToMenu = true; break; // awaryjnie wróć do selektora
            }

            if (exitToMenu) continue; // wróć do selektora poziomów
        }
    }





    return 0;
}
