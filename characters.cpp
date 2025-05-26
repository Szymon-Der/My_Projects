#include "characters.h"

//konstrukotr nadajacy wartosci fizyczne oraz przypisujacy plik z textura jako zmienna
Character::Character(const std::string& texturePath) :
    isOnGround(false), facingRight(true), moveSpeed(200.f),
    jumpStrength(600.f), gravity(1000.f), currentState(CharacterState::Static),
    frameIndex(0), animationTimer(0.f), frameDuration(0.1f) {
    if(!texture.loadFromFile(texturePath)){
        std::cerr << "blad ladowania textury z pliku: " << texturePath << std::endl;
    }
    setTexture(texture);
    loadAnimation();
    setTextureRect(frames[currentState][frameIndex]);
    setPosition(100.f,100.f);
}

//zaladowanie miejsc poszczegolnych klatek w pliku
void Character::loadAnimation(){
    frames[CharacterState::Static] = {{19,19,20,29},{83,19,20,29},{147,19,20,29},{211,19,20,29}};
    frames[CharacterState::Run] = {{19,147,20,29},{83,147,20,29},{147,147,20,29},{211,147,20,29},{275,147,20,29},{339,147,20,29}};
    frames[CharacterState::Jump] = {{19,19,20,29}};
}

//sprawdzanie czy nadany stan jest inny niz aktualny, jesli tak to ziana sekwencji animacji
void Character::setAnimationState(CharacterState newState){
    if(currentState != newState){
        currentState = newState;
        frameIndex = 0;
        animationTimer = 0.f;
        setTextureRect(frames[currentState][frameIndex]);
    }
}

//poruszanie postaci
void Character::move(int horizontalDirection, bool jump, float dt){
    velocity.x = horizontalDirection * moveSpeed; //przesuniecie prawp - lewo

    //obrot w lewo i prawo w zalezonosci od ruchu
    if(horizontalDirection > 0){
        facingRight = true;
        setScale(1.f, 1.f);
        setOrigin(getGlobalBounds().width, 0);
    }
    else if(horizontalDirection < 0){
        facingRight = false;
        setScale(-1.f,1.f);
        setOrigin(0,0);
    }

    //skok
    if(jump && isOnGround){
        velocity.y = -jumpStrength;
        isOnGround = false;
    }
    if(!isOnGround){
        velocity.y += gravity * dt;
    }
    //przesuniecie sprite'a o velocity pomnozene przez roznice czasu
    sf::Sprite::move(velocity*dt);
}

//zmiana kolejnych klatek
void Character::animate(float dt){
    animationTimer += dt; // dodanie czasu ostatniej klatki do calego czasu animacji

    if(animationTimer >= frameDuration){
        frameIndex = (frameIndex+1)%frames[currentState].size();
        setTextureRect(frames[currentState][frameIndex]);
        animationTimer=0.f;
    }

    if (!isOnGround) {
        setAnimationState(CharacterState::Jump);
    } else if (velocity.x != 0) {
        setAnimationState(CharacterState::Run);
    } else {
        setAnimationState(CharacterState::Static);
    }
}

//wywolanie jednoczenie move i animate
void Character::update(int horizontalDirection, bool jump, float dt){
    move(horizontalDirection, jump, dt);
    animate(dt);
}

void Character::setGroundContact(bool grounding){
    isOnGround = grounding;
}



