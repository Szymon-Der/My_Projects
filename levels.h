#ifndef LEVELS_H
#define LEVELS_H

#include <SFML/Graphics.hpp>
#include <vector>

struct LevelMap {
    sf::RectangleShape ground;
    std::vector<sf::RectangleShape> platforms;
};

LevelMap generateLevel(int levelNumber, const sf::RenderWindow& window);

#endif
