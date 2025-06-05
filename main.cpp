#include <SFML/Graphics.hpp>
#include "menu.h"
#include "characters.h"
#include "levels.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Dual Power", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("Fonts/Gatrich.otf")) {
        return -1;
    }

    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("Images/bg.jpg")) {
        return -1;
    }

    while (window.isOpen()) {
        Menu menu(window, font);
        if (!menu.run()) break; // użytkownik wybrał Exit lub zamknął okno

        bool returnToMainMenu = false;

        while (window.isOpen()) {
            int selectedLevel = menu.selectLevel();
            if (selectedLevel == -1) break; // użytkownik kliknął ESC w wyborze levelu

            bool exitToMenu = false;
            switch (selectedLevel) {
            case 1: exitToMenu = runLevel1(window, font, bgTexture); break;
            case 2: exitToMenu = runLevel2(window, font, bgTexture); break;
            case 3: exitToMenu = runLevel3(window, font, bgTexture); break;
            }

            if (exitToMenu) {
                continue; // wracamy do wyboru levelu
            }
        }

    }

    return 0;
}
