#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

    sf::Texture texture;
    if (!texture.loadFromFile("grass.png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }
    texture.setRepeated(true);
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(0, 0, 800, 600));

    sf::Texture texture_guy;
    if (!texture_guy.loadFromFile("guy.png")) {
        return 1;
    }
    sf::Sprite guy;
    guy.setTexture(texture_guy);
    guy.setPosition(100.0f, 100.0f);

    sf::Texture texture_wall;
    if (!texture_wall.loadFromFile("wall.png")) {
        return 1;
    }
    texture_wall.setRepeated(true);

    sf::Sprite wall;
    wall.setTexture(texture_wall);
    wall.setTextureRect(sf::IntRect(0, 0, 300, 50));
    wall.setPosition(0.0f, 300.0f);

    sf::Sprite wall2;
    wall2.setTexture(texture_wall);
    wall2.setTextureRect(sf::IntRect(0, 0, 300, 50));
    wall2.setPosition(400.0f, 300.0f);

    sf::Clock clock;
    sf::Vector2f velocity(0.0f, 0.0f);
    const float gravity = 800.0f;
    const float moveSpeed = 200.0f;
    const float jumpStrength = -400.0f;
    bool isOnGround = false;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            velocity.x = -moveSpeed;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            velocity.x = moveSpeed;
        } else {
            velocity.x = 0;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && isOnGround) {
            velocity.y = jumpStrength;
            isOnGround = false;
        }

        velocity.y += gravity * dt;

        guy.move(velocity.x * dt, velocity.y * dt);

        sf::FloatRect guyBounds = guy.getGlobalBounds();
        sf::FloatRect wallBounds = wall.getGlobalBounds();
        sf::FloatRect wall2Bounds = wall2.getGlobalBounds();


        if (guyBounds.intersects(wallBounds)) {
            guy.setPosition(guy.getPosition().x, wall.getPosition().y - guyBounds.height);
            velocity.y = 0;
            isOnGround = true;
        } else if (guyBounds.intersects(wall2Bounds)) {
            guy.setPosition(guy.getPosition().x, wall2.getPosition().y - guyBounds.height);
            velocity.y = 0;
            isOnGround = true;
        }

        guyBounds = guy.getGlobalBounds();
        const float windowWidth = window.getSize().x;
        const float windowHeight = window.getSize().y;

        if (guyBounds.left < 0 || guyBounds.left + guyBounds.width > windowWidth ||
            guyBounds.top < 0 || guyBounds.top + guyBounds.height > windowHeight) {
            guy.setPosition(100.0f, 100.0f);
            velocity = sf::Vector2f(0.0f, 0.0f);
            isOnGround = false;
        }

        window.clear();
        window.draw(sprite);
        window.draw(wall);
        window.draw(wall2);
        window.draw(guy);
        window.display();
    }

    return 0;
}
