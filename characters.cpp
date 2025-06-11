#include "characters.h"

//=======================================
//------------Character------------------
//=======================================

//konstrukotr nadajacy wartosci fizyczne oraz przypisujacy plik z textura jako zmienna
Character::Character(const std::string& texturePath) :
    isOnGround(false), facingRight(true), moveSpeed(150.f),
    jumpStrength(170.f), gravity(200.f), currentState(CharacterState::Static),
    frameIndex(0), animationTimer(0.f), frameDuration(0.1f) {
    if(!texture.loadFromFile(texturePath)){
        std::cerr << "blad ladowania textury z pliku: " << texturePath << std::endl;
    }
    setTexture(texture);
    loadAnimation();
    setTextureRect(frames[currentState][frameIndex]);
    setPosition(100.f,100.f);
    setScale(1.5f,1.5f);
    setOrigin(getLocalBounds().width / 2.f, 0.f);
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
        setScale(1.5f, 1.5f);
        //setOrigin(getGlobalBounds().width, 0);
    }
    else if(horizontalDirection < 0){
        facingRight = false;
        setScale(-1.5f,1.5f);
        //setOrigin(0,0);
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
    // Logika ustawiania stanu animacji została przeniesiona do metody update()
}

//wywolanie jednoczenie move i animate
void Character::update(int horizontalDirection, bool jump, float dt){
    // Najpierw zdecyduj o stanie animacji
    if (velocity.y != 0) {
        setAnimationState(CharacterState::Jump);
    } else if (velocity.x != 0 && velocity.y == 0) { // Sprawdź kierunek ruchu poziomego
        setAnimationState(CharacterState::Run);
    } else {
        setAnimationState(CharacterState::Static);
    }


    // Następnie zaktualizuj pozycję i odtwórz animację dla bieżącego stanu
    move(horizontalDirection, jump, dt);
    animate(dt);
}


void Character::setGroundContact(bool grounding){
    isOnGround = grounding;
}

void Character::setVelocity(float vx, float vy) {
    this->velocity.x = vx;
    this->velocity.y = vy;
}

sf::Vector2f Character::getVelocity() {
    return this->velocity;
}


//=======================================
//---------------NPC---------------------
//=======================================

//kostruktor dla pourszajacego sie npc
NPC::NPC(const std::string& texturePath, bool isShooting, std::pair<float, float> maxPositions) :
    Character(texturePath), isShooting(isShooting), maxPositions(maxPositions), shootCooldown(4.f), lastShoot(0.f){
    loadAnimation();
    setAnimationState(CharacterState::Run);
    this->moveSpeed = 50.f;
    setScale(1.5f,1.5f);
    setOrigin(getLocalBounds().width / 2.f, 0.f);
}

//konstruktor nieporuszajacego sie npc
NPC::NPC(const std::string& texturePath, bool isShooting, bool isFacingRight): Character(texturePath), isShooting(isShooting), shootCooldown(3.f), lastShoot(0.f){
    loadAnimation();
    setAnimationState(CharacterState::Static);
    this->moveSpeed = 0.f;
    this->facingRight = isFacingRight;
    setOrigin(getLocalBounds().width / 2.f, 0.f);
    if(!isFacingRight){
        setScale(1.5f, 1.5f);
    }
    else{
        setScale(-1.5f, 1.5f);
    }
}

void NPC::update(float dt){
    this->move(dt);
    this->animate(dt);
    if(this->isShooting){
        this->shoot(dt);
    }
}

//poruszanie sie prawo lewo miedzy znacznikami nadanymi w kostrutkorze
void NPC::move(float dt){
    //zmiana kierunku w po przekorczeniu pkt granicznych
    float x = getPosition().x;

    // Sprawdzenie granic — zakładamy brak zmiany originu
    if(moveSpeed!=0){
        if (x + getGlobalBounds().width > maxPositions.second && facingRight) {
            facingRight = false;
        } else if (x < maxPositions.first && !facingRight) {
            facingRight = true;
        }
    }

    if (facingRight) {
        velocity.x = moveSpeed;
        setScale(-1.5f, 1.5f);
    } else {
        velocity.x = -moveSpeed;
        setScale(1.5f, 1.5f);
    }

    if(!isOnGround){
        velocity.y += gravity * dt;
    }



    sf::Sprite::move(velocity * dt);
}

void NPC::shoot(float dt){
    lastShoot += dt;
    if(isShooting && lastShoot > shootCooldown){
        if(!facingRight){
            Bullet newBullet(-150.f, {this->getPosition().x-this->getGlobalBounds().width, this->getPosition().y + this->getGlobalBounds().height / 2});
            bullets.push_back(newBullet);
        }
        else{
            Bullet newBullet(150.f, {this->getPosition().x, this->getPosition().y + this->getGlobalBounds().height / 2});
            bullets.push_back(newBullet);
        }
        lastShoot = 0;
    }
}

void NPC::loadAnimation(){
    frames[CharacterState::Static] = {{25,31,30,33},{105,31,30,33},{185,31,30,33},{265,31,30,33},{345,31,30,33},{425,31,30,33},{505,31,30,33}};
    frames[CharacterState::Run] = {{25,94,30,33},{105,94,30,33},{185,94,30,33},{265,94,30,33},{345,94,30,33},{425,94,30,33},{505,94,30,33},{585,94,30,33}};
    frames[CharacterState::Jump] = {{25,31,30,33}};
}

std::vector<Bullet>& NPC::getBullets(){
    return bullets;
}

//=======================================
//---------------BULLET------------------
//=======================================

Bullet::Bullet(float velo, sf::Vector2f startPosition): sf::CircleShape(6.f), velocity(velo), active(true){
    setFillColor(sf::Color::Red);
    setOutlineColor(sf::Color::Black);
    setOutlineThickness(2.f);
    setPosition(startPosition);
}

void Bullet::update(float dt){
    this->move(velocity*dt,0);
}

void Bullet::deactive(){
    active = false;
}
void Bullet::activate(){
    active = true;
}
bool Bullet::getActive() const {
    return active;
}


//=======================================
//---------------KOLIZJE-----------------
//=======================================
//kolizja npc i character do sciany
void resolveCollisions(Character& character, const std::vector<sf::FloatRect>& tiles) {
    sf::FloatRect bounds = character.getGlobalBounds();
    sf::Vector2f characterPos = character.getPosition();
    sf::Vector2f characterVel = character.getVelocity();
    // sf::Vector2f origin = character.getOrigin(); // Origin jest brany pod uwagę przez getGlobalBounds() i setPosition()

    bool onGround = false;

    // Użyj kopii prędkości, żeby ewentualne zmiany w pętli nie wpływały na kolejne iteracje
    sf::Vector2f newVelocity = characterVel;
    sf::Vector2f newPosition = characterPos;

    for (const auto& tile : tiles) {
        if (bounds.intersects(tile)) {
            // Obliczamy głębokość przenikania dla obu osi
            float overlapX = std::min(bounds.left + bounds.width, tile.left + tile.width) - std::max(bounds.left, tile.left);
            float overlapY = std::min(bounds.top + bounds.height, tile.top + tile.height) - std::max(bounds.top, tile.top);

            // Sprawdzamy, który kierunek kolizji jest dominujący (które przenikanie jest mniejsze)
            // Dodano małą tolerancję (epsilon) aby uniknąć problemów z punktami pływającymi
            const float epsilon = 0.001f;

            if (overlapX < overlapY + epsilon) { // Kolizja z boku (lewa/prawa) jest dominująca
                if (newVelocity.x > 0 && bounds.left < tile.left) {
                    // Kolizja od lewej strony kafelka (postać idzie w prawo)
                    // Wypychamy postać na lewo o wartość przenikania
                    newPosition.x = characterPos.x - overlapX;
                    newVelocity.x = 0; // Zatrzymujemy ruch w poziomie
                } else if (newVelocity.x < 0 && bounds.left + bounds.width > tile.left + tile.width) {
                    // Kolizja od prawej strony kafelka (postać idzie w lewo)
                    // Wypychamy postać na prawo o wartość przenikania
                    newPosition.x = characterPos.x + overlapX;
                    newVelocity.x = 0; // Zatrzymujemy ruch w poziomie
                }
            } else { // Kolizja z góry/dołu jest dominująca
                if (newVelocity.y >= 0 && bounds.top < tile.top) { // Zmieniono z > na >= aby uwzględnić prędkość 0
                    // Kolizja od góry kafelka (postać spada lub stoi)
                    // Ustawiamy postać na kafelku
                    newPosition.y = characterPos.y - overlapY;
                    newVelocity.y = 0; // Zatrzymujemy ruch w pionie (na dół)
                    onGround = true; // Postać jest na ziemi
                } else if (newVelocity.y < 0 && bounds.top + bounds.height > tile.top + tile.height) {
                    // Kolizja od dołu kafelka (postać skacze w górę i uderza w spód kafelka)
                    // Wypychamy postać pod kafelek
                    newPosition.y = characterPos.y + overlapY;
                    newVelocity.y = 0; // Zatrzymujemy ruch w pionie (w górę)
                }
            }
        }
    }

    // Dopiero po przetworzeniu wszystkich kolizji aktualizujemy pozycję i prędkość postaci
    character.setPosition(newPosition);
    character.setVelocity(newVelocity.x, newVelocity.y);
    character.setGroundContact(onGround);
}


//kolzja bullet do sicany
void resolveCollisions(Bullet& bullet, const std::vector<sf::FloatRect>& tiles) {
    sf::FloatRect bounds = bullet.getGlobalBounds();

    for (const auto& tile : tiles) {
        if (bounds.intersects(tile)) {
            bullet.deactive(); // dezaktywuj pocisk przy kolizji
            break;
        }
    }
}
