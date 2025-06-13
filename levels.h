#ifndef LEVELS_H
#define LEVELS_H

#include <SFML/Graphics.hpp>
#include <vector>

struct LevelMap {
    sf::RectangleShape ground;
    std::vector<sf::RectangleShape> platforms;
};

LevelMap generateLevel(int levelNumber, const sf::RenderWindow& window);
bool runLevel1(sf::RenderWindow& window, sf::Font& font, const std::string& playerName);
bool runLevel2(sf::RenderWindow& window, sf::Font& font, const std::string& playerName);
bool runLevel3(sf::RenderWindow& window, sf::Font& font, const std::string& playerName);
//bool runLevelGeneric(sf::RenderWindow& window, sf::Font& font, const sf::Texture& backgroundTexture);


#endif
