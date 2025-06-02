#ifndef CHARACTERS_H
#define CHARACTERS_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <iostream>

enum class CharacterState{
    Static,
    Run,
    Jump
};

struct Animation{
    sf::Texture texture;
    std::vector<sf::IntRect> frames;
};

class Character : public sf::Sprite{
public:
    Character(const std::string& texturePath); //konstruktor ktory przekazuje adres pliku png z textura
    void animate(float dt); //zmiana rodzaju animacji i przuswanie kolejnych klatek
    void move(int horizontalDirection, bool jump, float dt); //metoda ktora bedzie aktualizowala pozycje z uwzgledniem grawitacji gdzie pierwsze przymuje wartosci {-1,0,1}
    void update(int horizontalDirection, bool jump, float dt); // jednoczesne wywolanie move i animate
    void setGroundContact(bool grounding); //ustawienie statusu isOnground
protected:
    //parametry podstawowe
    sf::Texture texture;
    sf::Vector2f velocity; //moveSpeed*deltaTime czyli przeiszczenie

    //parametry fizyczne
    bool isOnGround;
    bool facingRight; //potrzebne zeby zmieniac kierunek chodu

    float moveSpeed; //predkosc poruszania sie tora jest stala
    float jumpStrength;
    float gravity;

    //animacje
    std::map<CharacterState, std::vector<sf::IntRect>> frames;
    CharacterState currentState;
    int frameIndex;
    float animationTimer; //czas ktory uplynal od ostatniej zmiany klatki, zeruje sie jesli przkeorczy rameDuration
    float frameDuration; //regulacja trwania klatki animacji niezaleznie od fps

    //metody
    void loadAnimation(); //przypsianie statnow do konkretnych miejsc w texturze
    void setAnimationState(CharacterState newState); //sprawdzanie czy stan sie zmienil wzgedem aktualnego, jesli tak to zmienia set animacji
};

class NPC : public Character{
public:
    NPC(const std::string& texturePath, bool isShooting, std::pair<float, float> maxPositions); //wywolanie wraz z przypisaniem plikow z animacjami
    NPC(const std::string& texturePath, bool isShooting, bool isFacingRight);    // konstruktor ktory wywoluje npc ktroy sie nie porusza
    void move(float dt);

private:
    std::pair<float, float> maxPositions; //pozycje przy koterych npc bedzie zawracał
    bool isShooting; //nadanie czy npc bedzie strzelal

    void loadAnimation();
};

#endif // CHARACTERS_H
