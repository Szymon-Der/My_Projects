#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "characters.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Character Test");
    window.setFramerateLimit(60);

    // Tworzenie postaci z pliku tekstury
    Character player("Characters\\Character 9.png");

    // Prostokąt udający podłogę
    sf::RectangleShape ground(sf::Vector2f(800, 50));
    ground.setFillColor(sf::Color::Green);
    ground.setPosition(0, 550);

    // Zegar do pomiaru czasu między klatkami
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Delta time
        float dt = clock.restart().asSeconds();

        // Odczyt sterowania
        int horizontal = 0;
        bool jump = false;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            horizontal = -1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            horizontal = 1;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            jump = true;

        // Detekcja kolizji z podłożem (prosta)
        if (player.getPosition().y + player.getGlobalBounds().height >= ground.getPosition().y) {
            player.setPosition(player.getPosition().x, ground.getPosition().y - player.getGlobalBounds().height);
            player.setGroundContact(true);
        } else {
            player.setGroundContact(false);
        }

        // Aktualizacja postaci
        player.update(horizontal, jump, dt);

        // Rysowanie
        window.clear(sf::Color::Cyan);
        window.draw(ground);
        window.draw(player);
        window.display();
    }

    return 0;
}
