// menu.h
#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Menu {
public:
    // Konstruktor przyjmuje referencję do już istniejącego sf::RenderWindow oraz sf::Font
    Menu(sf::RenderWindow& window, sf::Font& font);

    // Główna pętla menu. Zwraca true, jeśli wybrano Play i wpisano nazwę (czyli należy przejść do gry),
    // false jeśli zamknięto/wybrano Exit w menu
    bool run();
    int chooseLevel();
    int selectLevel();

    // Po wyjściu z run(): jeśli zwrócono true, w main można pobrać nazwę gracza (opcjonalnie)
    const std::string& getPlayerName() const { return m_playerName; }
    void showProfile();
    int getGamesPlayed(const std::string& filename, const std::string& playerName);
    void updateGamesPlayedCSV(const std::string& filename, const std::string& playerName);

private:
    std::vector<sf::Texture> m_levelTextures;
    void preloadLevelTextures();
    sf::RenderWindow& m_window;
    sf::Font& m_font;

    // Tekstura oraz sprite tła
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;

    // Lista opcji menu i odpowiadające im obiekty sf::Text
    std::vector<std::string> m_options;
    std::vector<sf::Text> m_menuTexts;
    int m_selected; // indeks aktualnie zaznaczonej opcji

    // Flaga / dane dotyczące rozpoczęcia gry
    bool m_startGame;           // true, gdy po wpisaniu nazwy gracza chcemy wyjść z menu
    std::string m_playerName;   // przechowuje wpisaną w menu nazwę gracza

    // Funkcje pomocnicze
    void loadBackground(const std::string& filePath);
    void initializeMenu();
    void processEvents();
    void render();

    // Poszczególne podmenu
    void showInstructions();
    void displayPlayers();
    void inputPlayerName();

    // Pomocnicze przy parsowaniu pliku CSV
    std::vector<std::string> readPlayerNamesFromCSV(const std::string& filename);
};

#endif // MENU_H
