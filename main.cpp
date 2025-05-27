#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "characters.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Character & NPC Demo");
    window.setFramerateLimit(60);

    // Gracz
    Character player("Characters\\Character 9.png");

    // NPC (chodzi automatycznie od x = 100 do x = 700)
    NPC npc("Characters\\Mushroom.png", false, {100.f, 700.f});
    npc.setPosition(300.f, 500.f);

    // Podłoga
    sf::RectangleShape ground(sf::Vector2f(800, 50));
    ground.setFillColor(sf::Color::Green);
    ground.setPosition(0, 550);

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float dt = clock.restart().asSeconds();

        // Sterowanie graczem
        int horizontal = 0;
        bool jump = false;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            horizontal = -1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            horizontal = 1;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            jump = true;

        // Kolizja gracza z podłożem
        if (player.getPosition().y + player.getGlobalBounds().height >= ground.getPosition().y) {
            player.setPosition(player.getPosition().x, ground.getPosition().y - player.getGlobalBounds().height);
            player.setGroundContact(true);
        } else {
            player.setGroundContact(false);
        }

        // NPC nie skacze, ale też ograniczamy go do podłogi
        if (npc.getPosition().y + npc.getGlobalBounds().height >= ground.getPosition().y) {
            npc.setPosition(npc.getPosition().x, ground.getPosition().y - npc.getGlobalBounds().height);
            npc.setGroundContact(true);
        } else {
            npc.setGroundContact(false);
        }

        // Aktualizacja
        player.update(horizontal, jump, dt);
        npc.move(dt);  // NPC chodzi samodzielnie
        npc.animate(dt);

        // Rysowanie
        window.clear(sf::Color::Cyan);
        window.draw(ground);
        window.draw(player);
        window.draw(npc);
        window.display();
    }

    return 0;
}
