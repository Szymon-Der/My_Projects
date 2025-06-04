// main.cpp
#include <SFML/Graphics.hpp>
#include "menu.h"
#include "characters.h"

int main() {
    // 1) Utworzenie okna
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Dual Power", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // 2) Załadowanie czcionki
    sf::Font font;
    if (!font.loadFromFile("Fonts/Gatrich.otf")) {
        return -1;
    }

    // Główna pętla aplikacji: za każdym razem pokazujemy menu, a po zakończeniu gry wracamy do menu
    while (window.isOpen()) {
        // 3) Uruchamiamy menu
    levelSelect:
        Menu menu(window, font);
        bool startGame = menu.run();
        if (!startGame) {
            break; // użytkownik wybrał Exit albo zamknął okno
        }

        int selectedLevel = -1;
        while (selectedLevel == -1 && window.isOpen()) {
            selectedLevel = menu.selectLevel(); // lub chooseLevel(), w zależności którą wersję chcesz
        }
        if (!startGame) {
            // Gracz zamknął okno lub wybrał Exit – kończymy program
            break;
        }

        // 4) Jeśli tutaj dotarliśmy => wybrano Play i wpisano nazwę gracza
        //    Tworzymy dwie postacie: player (gracz) oraz npc (NPC z własnym ruchem)

        // Ścieżki do grafik (dostosuj do swojego projektu):
        // - player: "Characters/Character 9.png"
        // - npc:    "Characters/Mushroom.png"
        Character player("Characters/Character 9.png");
        NPC      npc("Characters/Mushroom.png", false, {120,300});

        // 5) Ustawienie pozycji początkowych obu postaci oraz poziomu podłoża
        float groundY = 550.f;
        player.setPosition(100.f, groundY - player.getGlobalBounds().height);
        npc.setPosition(300.f, groundY - npc.getGlobalBounds().height);

        // 6) Kształt reprezentujący „ziemię”
        sf::RectangleShape ground(sf::Vector2f(static_cast<float>(window.getSize().x), 170.f));
        ground.setFillColor(sf::Color(100, 250, 50));
        ground.setPosition(0.f, groundY);

        // 7) Zegar do wyliczania deltaTime
        sf::Clock clock;

        // Zmienna kontrolująca wyjście z pętli gry i powrót do menu
        bool backToMenu = false;

        // 8) Pętla gry
        while (window.isOpen() && !backToMenu) {
            float dt = clock.restart().asSeconds();

            sf::Event event;
            while (window.pollEvent(event)) {
                // Zamknięcie okna
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                // Klawisz Esc: wychodzimy z pętli gry i wracamy do menu
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    backToMenu = true;
                    goto levelSelect;
                }
            }

            // Jeżeli wracamy do menu, przerywamy dalsze aktualizacje i rysowanie
            if (backToMenu) {
                break;
            }

            // 9) STEROWANIE GRACZEM
            int horizontal = 0;
            bool jump = false;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                horizontal = -1;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                horizontal = 1;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                jump = true;
            }

            // 10) KOLIZJA GRACZA Z PODŁOŻEM
            if (player.getPosition().y + player.getGlobalBounds().height >= groundY) {
                player.setPosition(player.getPosition().x, groundY - player.getGlobalBounds().height);
                player.setGroundContact(true);
            } else {
                player.setGroundContact(false);
            }

            // 11) KOLIZJA NPC Z PODŁOŻEM
            if (npc.getPosition().y + npc.getGlobalBounds().height >= groundY) {
                npc.setPosition(npc.getPosition().x, groundY - npc.getGlobalBounds().height);
                npc.setGroundContact(true);
            } else {
                npc.setGroundContact(false);
            }

            // 12) AKTUALIZACJA STANU POSTACI
            player.update(horizontal, jump, dt);
            npc.move(dt);
            npc.animate(dt);

            // 13) RYSOWANIE CAŁEJ SCENY
            window.clear(sf::Color(50, 50, 50));
            window.draw(ground);
            window.draw(player);
            window.draw(npc);
            window.display();
        }

        // Po wyjściu z pętli gry (backToMenu==true), wrócimy do początku głównej pętli i znów wyświetlimy menu.
        // Jeśli okno zostało zamknięte (window.isOpen() == false), to wyjdziemy z głównej pętli i zakończymy program.
    }

    return 0;
}
