// menu.cpp
#include "menu.h"
#include "levels.h"
#include "characters.h"
#include "playtime.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <set>
#include <cctype> // dla toupper i isalnum

Menu::Menu(sf::RenderWindow& window, sf::Font& font)
    : m_window(window), m_font(font), m_selected(0), m_startGame(false)
{
    loadBackground("Images/bg1.png");
    m_options = { "Play", "Profile", "Leaderboard", "Exit" };
    initializeMenu();
    preloadLevelTextures();
}

void Menu::loadBackground(const std::string& filePath) {
    if (!m_backgroundTexture.loadFromFile(filePath)) {
        std::exit(-1);
    }
    m_backgroundSprite.setTexture(m_backgroundTexture);
    sf::Vector2u texSize = m_backgroundTexture.getSize();
    float scaleX = static_cast<float>(m_window.getSize().x) / texSize.x;
    float scaleY = static_cast<float>(m_window.getSize().y) / texSize.y;
    m_backgroundSprite.setScale(scaleX, scaleY);
}

void Menu::initializeMenu() {
    m_menuTexts.clear();
    for (size_t i = 0; i < m_options.size(); ++i) {
        sf::Text text(m_options[i], m_font, 40);
        text.setPosition(100.f, 150.f + static_cast<float>(i) * 80.f);
        text.setFillColor((i == m_selected) ? sf::Color::Red : sf::Color::White);
        m_menuTexts.push_back(text);
    }
}

bool Menu::run(bool firstRun) {
    if (firstRun && m_playerName.empty()) {
        inputPlayerName();
        if (m_playerName.empty()) {
            // Użytkownik anulował wpisywanie – zamykamy okno
            m_window.close();
            return false;
        }
    }


    while (m_window.isOpen()) {
        processEvents();

        if (m_startGame) {
            // Przechodzimy do wyboru poziomu
            return true;
        }

        render(); // Rysowanie menu i ewentualnie imienia gracza w rogu
    }

    // Zamknięto okno lub wybrano Exit
    return false;
}


void Menu::processEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
            return;
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Up) {
                m_menuTexts[m_selected].setFillColor(sf::Color::White);
                m_selected = (m_selected - 1 + static_cast<int>(m_menuTexts.size())) % static_cast<int>(m_menuTexts.size());
                m_menuTexts[m_selected].setFillColor(sf::Color::Red);
            }
            else if (event.key.code == sf::Keyboard::Down) {
                m_menuTexts[m_selected].setFillColor(sf::Color::White);
                m_selected = (m_selected + 1) % static_cast<int>(m_menuTexts.size());
                m_menuTexts[m_selected].setFillColor(sf::Color::Red);
            }
            else if (event.key.code == sf::Keyboard::Enter) {
                const std::string& choice = m_options[m_selected];

                if (choice == "Play") {
                    m_startGame = true;
                    return;
                }

                else if (choice == "Profile") {
                    showProfile();
                }
                else if (choice == "Leaderboard") {
                    displayPlayers();
                }
                else if (choice == "Exit") {
                    m_window.close();
                    return;
                }

                initializeMenu(); // resetujemy zaznaczenie
            }

        }
    }
}

void Menu::render() {
    m_window.clear();
    m_window.draw(m_backgroundSprite);

    // Rysujemy tytuł gry "Dual Power" w krwistoczerwonym kolorze
    sf::Color bloodRed(180, 0, 0);
    sf::Text title("Dual Power", m_font, 60);
    title.setFillColor(bloodRed);
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(2);
    sf::FloatRect tBounds = title.getLocalBounds();
    float tX = (static_cast<float>(m_window.getSize().x) - tBounds.width) / 2.f - tBounds.left;
    float tY = 20.f;
    title.setPosition(tX, tY);
    m_window.draw(title);

    // Rysujemy każdą opcję menu
    for (auto& text : m_menuTexts) {
        m_window.draw(text);
    }

    // Na dole ekranu: informacja o Enter
    sf::Text footer("Press Enter to select", m_font, 20);
    footer.setFillColor(sf::Color::White);
    sf::FloatRect fBounds = footer.getLocalBounds();
    float fx = (static_cast<float>(m_window.getSize().x) - fBounds.width) / 2.f - fBounds.left;
    float fy = static_cast<float>(m_window.getSize().y) - fBounds.height - 10.f;
    footer.setPosition(fx, fy);
    m_window.draw(footer);
    if (!m_playerName.empty()) {
        sf::Text nameText(m_playerName, m_font, 28);
        nameText.setFillColor(sf::Color::Yellow);
        sf::FloatRect bounds = nameText.getLocalBounds();
        nameText.setPosition(
            m_window.getSize().x - bounds.width - 20.f,
            10.f
            );
        m_window.draw(nameText);
    }

    m_window.display();
}

void Menu::showProfile() {
    bool back = false;

    // Nagłówek: nazwa gracza (na górze)
    sf::Text playerNameText(m_playerName, m_font, 40);
    playerNameText.setFillColor(sf::Color::Yellow);
    sf::FloatRect nameBounds = playerNameText.getLocalBounds();
    playerNameText.setOrigin(nameBounds.width / 2.f, nameBounds.height / 2.f);
    playerNameText.setPosition(
        static_cast<float>(m_window.getSize().x) / 2.f,
        60.f
        );

    // Podpis: czas gry
    int seconds = readPlayTime(m_playerName);

    // Pierwsza część – biały napis
    sf::Text labelText("Time spent on playing:", m_font, 28);
    labelText.setFillColor(sf::Color::White);
    sf::FloatRect labelBounds = labelText.getLocalBounds();
    labelText.setOrigin(labelBounds.width / 2.f, labelBounds.height / 2.f);
    labelText.setPosition(
        static_cast<float>(m_window.getSize().x) / 2.f - 40.f, // lekko w lewo
        120.f
        );

    // Druga część – zielony czas
    sf::Text timeOnlyText(formatTime(seconds), m_font, 28);
    timeOnlyText.setFillColor(sf::Color::Green);
    sf::FloatRect timeBounds = timeOnlyText.getLocalBounds();
    timeOnlyText.setOrigin(timeBounds.width / 2.f, timeBounds.height / 2.f);
    timeOnlyText.setPosition(
        labelText.getPosition().x + labelBounds.width / 2.f + timeBounds.width / 2.f + 5.f,
        120.f
        );


    // Stopka: ESC aby wyjść
    sf::Text footer("Press ESC to return to menu", m_font, 20);
    footer.setFillColor(sf::Color::White);
    sf::FloatRect fBounds = footer.getLocalBounds();
    footer.setOrigin(fBounds.width / 2.f, fBounds.height / 2.f);
    footer.setPosition(
        static_cast<float>(m_window.getSize().x) / 2.f,
        static_cast<float>(m_window.getSize().y) - fBounds.height - 20.f
        );

    while (m_window.isOpen() && !back) {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                m_window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                back = true;
        }

        m_window.clear();
        m_window.draw(m_backgroundSprite);
        m_window.draw(playerNameText);
        m_window.draw(labelText);
        m_window.draw(timeOnlyText);
        m_window.draw(footer);
        m_window.display();
    }
}



std::vector<std::string> Menu::readPlayerNamesFromCSV(const std::string& filename) {
    std::vector<std::string> result;
    std::ifstream file(filename);
    if (!file.is_open()) {
        return result;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;
        if (std::getline(ss, token, ',')) {
            if (!token.empty()) {
                result.push_back(token);
            }
        }
    }
    file.close();
    return result;
}

void Menu::displayPlayers() {
    std::ifstream file("csv/players.csv");
    std::vector<std::pair<std::string, int>> players;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string name, timeStr;
        if (std::getline(ss, name, ',') && std::getline(ss, timeStr)) {
            try {
                int time = std::stoi(timeStr);
                players.emplace_back(name, time);
            } catch (...) {
                players.emplace_back(name, 0); // jeśli nie ma czasu, zakładamy 0
            }
        } else {
            players.emplace_back(line, 0); // stary format bez czasu
        }
    }

    // Sortowanie malejąco po czasie
    std::sort(players.begin(), players.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });

    if (players.size() > 10) {
        players.resize(10);
    }


    bool back = false;
    while (m_window.isOpen() && !back) {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                back = true;
            }
        }

        m_window.clear();
        m_window.draw(m_backgroundSprite);

        float winW = static_cast<float>(m_window.getSize().x);
        float winH = static_cast<float>(m_window.getSize().y);
        float centerX = winW / 2.f;

        // ---------- Nagłówek "Top Players" w czerwonym kolorze ----------
        sf::Color redColor(220, 20, 20);
        sf::Text header("Top Players", m_font, 36);
        header.setFillColor(redColor);
        sf::FloatRect hBounds = header.getLocalBounds();
        float hX = (winW - hBounds.width) / 2.f - hBounds.left;
        float hY = 20.f;
        header.setPosition(hX, hY);
        m_window.draw(header);

        // ---------- Rysowanie podium (u góry ekranu) ----------
        float baseY = hY + hBounds.height + 100.f;
        float stepW = 180.f;
        float maxH = 180.f;

        std::vector<std::string> podiumNames;
        for (size_t i = 0; i < std::min<size_t>(3, players.size()); ++i) {
            podiumNames.push_back(players[i].first);
        }

        float x1 = centerX - (stepW / 2.f);
        float gap = stepW + 20.f;
        float x2 = x1 - gap;
        float x3 = x1 + gap;

        float h1 = 180.f;
        float h2 = 140.f;
        float h3 = 100.f;

        // 2. miejsce (jeśli jest)
        if (podiumNames.size() >= 2) {
            sf::RectangleShape rect2({ stepW, h2 });
            rect2.setFillColor(sf::Color(192, 192, 192));
            rect2.setPosition(x2, baseY + (maxH - h2));
            m_window.draw(rect2);

            sf::Text name2(podiumNames[1], m_font, 30);
            name2.setFillColor(sf::Color::White);
            sf::FloatRect n2b = name2.getLocalBounds();
            float name2X = x2 + (stepW - n2b.width) / 2.f - n2b.left;
            float name2Y = rect2.getPosition().y - n2b.height - 10.f;
            name2.setPosition(name2X, name2Y);
            m_window.draw(name2);

            sf::Text num2("2", m_font, 36);
            num2.setFillColor(sf::Color::Black);
            sf::FloatRect num2b = num2.getLocalBounds();
            float num2X = x2 + (stepW - num2b.width) / 2.f - num2b.left;
            float num2Y = rect2.getPosition().y + (h2 - num2b.height) / 2.f - num2b.top;
            num2.setPosition(num2X, num2Y);
            m_window.draw(num2);
        }

        // 1. miejsce (jeśli jest)
        if (podiumNames.size() >= 1) {
            sf::RectangleShape rect1({ stepW, h1 });
            rect1.setFillColor(sf::Color(255, 215, 0));
            rect1.setPosition(x1, baseY);
            m_window.draw(rect1);

            sf::Text name1(podiumNames[0], m_font, 30);
            name1.setFillColor(sf::Color::White);
            sf::FloatRect n1b = name1.getLocalBounds();
            float name1X = x1 + (stepW - n1b.width) / 2.f - n1b.left;
            float name1Y = rect1.getPosition().y - n1b.height - 10.f;
            name1.setPosition(name1X, name1Y);
            m_window.draw(name1);

            sf::Text num1("1", m_font, 48);
            num1.setFillColor(sf::Color::Black);
            sf::FloatRect num1b = num1.getLocalBounds();
            float num1X = x1 + (stepW - num1b.width) / 2.f - num1b.left;
            float num1Y = rect1.getPosition().y + (h1 - num1b.height) / 2.f - num1b.top;
            num1.setPosition(num1X, num1Y);
            m_window.draw(num1);
        }

        // 3. miejsce (jeśli jest)
        if (podiumNames.size() >= 3) {
            sf::RectangleShape rect3({ stepW, h3 });
            rect3.setFillColor(sf::Color(205, 127, 50));
            rect3.setPosition(x3, baseY + (maxH - h3));
            m_window.draw(rect3);

            sf::Text name3(podiumNames[2], m_font, 30);
            name3.setFillColor(sf::Color::White);
            sf::FloatRect n3b = name3.getLocalBounds();
            float name3X = x3 + (stepW - n3b.width) / 2.f - n3b.left;
            float name3Y = rect3.getPosition().y - n3b.height - 10.f;
            name3.setPosition(name3X, name3Y);
            m_window.draw(name3);

            sf::Text num3("3", m_font, 36);
            num3.setFillColor(sf::Color::Black);
            sf::FloatRect num3b = num3.getLocalBounds();
            float num3X = x3 + (stepW - num3b.width) / 2.f - num3b.left;
            float num3Y = rect3.getPosition().y + (h3 - num3b.height) / 2.f - num3b.top;
            num3.setPosition(num3X, num3Y);
            m_window.draw(num3);
        }

        // ---------- Lista pozostałych graczy (4. miejsce - 10. miejsce) ----------
        float startY = baseY + maxH + 40.f;
        float listX = centerX - (stepW / 2.f);
        for (size_t i = 3; i < players.size(); ++i) {
            std::string line = std::to_string(static_cast<int>(i) + 1) + ". " + players[i].first;
            sf::Text text(line, m_font, 24);
            text.setFillColor(sf::Color::White);
            float ty = startY + static_cast<float>(i - 3) * 35.f;
            text.setPosition(listX, ty);
            m_window.draw(text);
        }

        // Komunikat "Press ESC to return to menu" na dole ekranu
        sf::Text footer("Press ESC to return to menu", m_font, 20);
        footer.setFillColor(sf::Color::White);
        sf::FloatRect fBounds = footer.getLocalBounds();
        float fx2 = (winW - fBounds.width) / 2.f - fBounds.left;
        float fy2 = winH - fBounds.height - 10.f;
        footer.setPosition(fx2, fy2);
        m_window.draw(footer);

        m_window.display();
    }
}

void Menu::inputPlayerName() {
    sf::String playerInput;
    bool done = false;
    bool cancelled = false;
    std::string errorMsg;

    // "Enter player name:" na czerwono, większa czcionka
    sf::Color redColor(220, 20, 20);
    sf::Text promptText("Enter player name:", m_font, 36);
    promptText.setFillColor(redColor);

    // Wpisywane imię na żółto
    sf::Text inputText("", m_font, 32);
    inputText.setFillColor(sf::Color::Yellow);

    // Komunikat powitalny lub błąd
    sf::Text messageText("", m_font, 28);

    while (m_window.isOpen() && !done && !cancelled) {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                cancelled = true;
                break;
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b') {
                    if (!playerInput.isEmpty()) {
                        playerInput.erase(playerInput.getSize() - 1, 1);
                        errorMsg.clear();
                    }
                }
                else if (event.text.unicode < 128 && event.text.unicode != '\r') {
                    char c = static_cast<char>(event.text.unicode);
                    // Zezwalamy na litery, cyfry i underscore
                    if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
                        if (playerInput.getSize() < 10) {
                            // Konwertujemy na wielkie litery
                            playerInput += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
                            errorMsg.clear();
                        } else {
                            errorMsg = "Max length is 10 characters!";
                        }
                    }
                }
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                std::string name = playerInput.toAnsiString();
                if (name.empty()) {
                    errorMsg = "Name cannot be empty!";
                    messageText.setString(errorMsg);
                    messageText.setFillColor(sf::Color::Red);
                    continue;
                }
                std::set<std::string> existing;
                std::ifstream inFile("csv/players.csv");
                std::string line;
                while (std::getline(inFile, line)) {
                    if (line.empty()) continue;
                    std::stringstream ss(line);
                    std::string token;
                    if (std::getline(ss, token, ',')) {
                        if (!token.empty()) {
                            existing.insert(token);
                        }
                    }
                }
                inFile.close();

                if (existing.find(name) != existing.end()) {
                    messageText.setString("Welcome back, " + name + "!");
                } else {
                    std::ofstream outFile("csv/players.csv", std::ios::app);
                    outFile << name << ",0,0,0,0\n";
                    outFile.close();
                    messageText.setString("Nice to meet you, " + name + "! Welcome!");
                }
                messageText.setFillColor(sf::Color::Green);
                done = true;
                m_playerName = name; // zapisujemy w polu klasy Menu
            }
        }

        if (cancelled) break;

        inputText.setString(playerInput);
        if (!errorMsg.empty() && !done) {
            messageText.setString(errorMsg);
            messageText.setFillColor(sf::Color::Red);
        }

        auto winSize = m_window.getSize();
        float winW = static_cast<float>(winSize.x);
        float winH = static_cast<float>(winSize.y);

        // Pozycjonowanie promptu wyżej (25% w pionie)
        sf::FloatRect pBounds = promptText.getLocalBounds();
        float pX = (winW - pBounds.width) / 2.f - pBounds.left;
        float pY = winH * 0.25f - pBounds.height / 2.f;
        promptText.setPosition(pX, pY);

        // Pozycjonowanie inputu pod promptem
        sf::FloatRect iBounds = inputText.getLocalBounds();
        float iX = (winW - iBounds.width) / 2.f - iBounds.left;
        float iY = pY + pBounds.height + 15.f;
        inputText.setPosition(iX, iY);

        // Pozycjonowanie komunikatu powitalnego lub błędu trochę niżej
        sf::FloatRect mBounds = messageText.getLocalBounds();
        float mX = (winW - mBounds.width) / 2.f - mBounds.left;
        float mY = iY + iBounds.height + 35.f;
        messageText.setPosition(mX, mY);

        // Komunikat „ESC to exit” na dole ekranu
        sf::Text footer("Press ESC to exit", m_font, 20);
        footer.setFillColor(sf::Color::White);
        sf::FloatRect fBounds = footer.getLocalBounds();
        float fX = (winW - fBounds.width) / 2.f - fBounds.left;
        float fY = winH - fBounds.height - 10.f;
        footer.setPosition(fX, fY);

        m_window.clear();
        m_window.draw(m_backgroundSprite);
        m_window.draw(promptText);
        m_window.draw(inputText);
        if (done || !errorMsg.empty()) {
            m_window.draw(messageText);
        }
        m_window.draw(footer);
        m_window.display();
    }

    if (done && !cancelled) {
        sf::sleep(sf::seconds(1.5f)); // krótka przerwa przed wejściem do gry
    }
    // Jeśli anulowano (ESC), to po prostu powracamy do menu bez wpisanej nazwy
    if (cancelled) {
        m_playerName.clear();
    }
}


int Menu::selectLevel() {
    struct LevelData {
        std::string number;
        std::string name;
        std::string textureFile;
    };

    std::vector<LevelData> levels = {
        {"1", "EASY",   "Images/level1.png"},
        {"2", "MEDIUM", "Images/level2.png"},
        {"3", "HARD",   "Images/level3.png"}
    };

    sf::Texture starTex;
    if (!starTex.loadFromFile("Images/star.png")) {
        std::cerr << "Nie można załadować Images/star.png\n";
    }
    const float starScale   = 0.15f;
    const float starSpacing = 8.f;

    std::vector<sf::RectangleShape> boxes;
    std::vector<sf::Text> numberTexts;
    std::vector<sf::Text> nameTexts;
    std::vector<sf::Texture> textures;


    float boxWidth = 200.f;
    float boxHeight = 200.f;
    float gap = 80.f;
    float totalWidth = levels.size() * boxWidth + (levels.size() - 1) * gap;
    float startX = (m_window.getSize().x - totalWidth) / 2.f;
    float topY = 200.f;

    int selectedLevel = 0;

    sf::Text header("LEVELS", m_font, 50);
    header.setFillColor(sf::Color(255, 50, 50));
    sf::FloatRect hBounds = header.getLocalBounds();
    header.setPosition((m_window.getSize().x - hBounds.width) / 2.f, 50.f);

    for (size_t i = 0; i < levels.size(); ++i) {
        sf::RectangleShape box(sf::Vector2f(boxWidth, boxHeight));
        box.setPosition(startX + i * (boxWidth + gap), topY);

        box.setTexture(&m_levelTextures[i]);

        sf::Text numText(levels[i].number, m_font, 60);
        numText.setFillColor(sf::Color::Yellow);
        sf::FloatRect nBounds = numText.getLocalBounds();
        numText.setPosition(
            box.getPosition().x + (boxWidth - nBounds.width) / 2.f - nBounds.left,
            box.getPosition().y + (boxHeight - nBounds.height) / 2.f - nBounds.top
            );

        sf::Text nameText(levels[i].name, m_font, 28);
        nameText.setFillColor(sf::Color::White);
        sf::FloatRect nameBounds = nameText.getLocalBounds();
        nameText.setPosition(
            box.getPosition().x + (boxWidth - nameBounds.width) / 2.f - nameBounds.left,
            box.getPosition().y + boxHeight + 10.f
            );

        boxes.push_back(box);
        numberTexts.push_back(numText);
        nameTexts.push_back(nameText);
    }

    boxes[selectedLevel].setOutlineThickness(5.f);
    boxes[selectedLevel].setOutlineColor(sf::Color::Red);

    bool choosing = true;
    bool aborted = false;

    while (m_window.isOpen() && choosing) {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_window.close();
                return -1;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left) {
                    boxes[selectedLevel].setOutlineThickness(0.f);
                    selectedLevel = (selectedLevel - 1 + 3) % 3;
                    boxes[selectedLevel].setOutlineThickness(5.f);
                    boxes[selectedLevel].setOutlineColor(sf::Color::Red);
                } else if (event.key.code == sf::Keyboard::Right) {
                    boxes[selectedLevel].setOutlineThickness(0.f);
                    selectedLevel = (selectedLevel + 1) % 3;
                    boxes[selectedLevel].setOutlineThickness(5.f);
                    boxes[selectedLevel].setOutlineColor(sf::Color::Red);
                } else if (event.key.code == sf::Keyboard::Enter) {
                    choosing = false;
                } else if (event.key.code == sf::Keyboard::Escape) {
                    aborted = true;
                    choosing = false;
                }
            }
        }

        m_window.clear();
        m_window.draw(m_backgroundSprite);
        m_window.draw(header);
        for (size_t i = 0; i < boxes.size(); ++i) {
            m_window.draw(boxes[i]);
            m_window.draw(numberTexts[i]);
            m_window.draw(nameTexts[i]);
            int lvl     = static_cast<int>(i) + 1;
            int elapsed = readLevelTime(m_playerName, lvl);
            if (elapsed > 0) {
                float total = (lvl == 1 ? 50.f : 180.f);
                float ratio = (total - elapsed) / total;
                int starCnt = (ratio > 0.4f ? 3 : (ratio > 0.2f ? 2 : 1));

                float sw     = starTex.getSize().x * starScale;
                float startX = boxes[i].getPosition().x
                               + (boxWidth - (starCnt*sw + (starCnt-1)*starSpacing)) / 2.f;
                float sy     = boxes[i].getPosition().y   // y pudełka
                           - sw                       // wysokość gwiazdki
                           - 10.f;                    // 10px odstępu nad boxem

                for (int s = 0; s < starCnt; ++s) {
                    sf::Sprite st(starTex);
                    st.setScale(starScale, starScale);
                    st.setPosition(startX + s*(sw + starSpacing), sy);
                    m_window.draw(st);
                }
            }

        }
        // Komunikat „ESC to return to menu” na dole ekranu
        sf::Text footer("Press ESC to return to menu", m_font, 20);
        footer.setFillColor(sf::Color::White);
        sf::FloatRect fBounds = footer.getLocalBounds();
        float fx = (static_cast<float>(m_window.getSize().x) - fBounds.width) / 2.f - fBounds.left;
        float fy = static_cast<float>(m_window.getSize().y) - fBounds.height - 10.f;
        footer.setPosition(fx, fy);
        m_window.draw(footer);
        // Wyświetlanie nazwy gracza w prawym górnym rogu
        sf::Text nameText(m_playerName, m_font, 28);
        nameText.setFillColor(sf::Color::Yellow);
        sf::FloatRect nBounds = nameText.getLocalBounds();
        nameText.setPosition(
            m_window.getSize().x - nBounds.width - 20.f,
            10.f
            );
        m_window.draw(nameText);


        m_window.display();
    }

    if (aborted) {
        m_startGame = false;  // resetujemy flagę, żeby nie przejść do gry
        return -1;
    }

    return selectedLevel + 1;

}


void Menu::preloadLevelTextures() {
    std::vector<std::string> files = {
        "Images/level1.jpg", "Images/level2.jpg", "Images/level3.jpg"
    };

    m_levelTextures.clear();
    for (const auto& file : files) {
        sf::Texture tex;
        if (!tex.loadFromFile(file)) {
            std::cerr << "Nie można załadować: " << file << std::endl;
            tex.create(200, 200); // placeholder
        }
        tex.setSmooth(true);
        m_levelTextures.push_back(std::move(tex));
    }
}
