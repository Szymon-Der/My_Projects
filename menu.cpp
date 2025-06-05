// menu.cpp
#include "menu.h"
#include "levels.h"
#include "characters.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <set>
#include <cctype> // dla toupper i isalnum

Menu::Menu(sf::RenderWindow& window, sf::Font& font)
    : m_window(window), m_font(font), m_selected(0), m_startGame(false)
{
    loadBackground("Images/bg.jpg");
    m_options = { "Play", "Instructions", "Leaderboard", "Exit" };
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

bool Menu::run() {
    while (m_window.isOpen()) {
        processEvents();
        if (m_startGame) {
            // Użytkownik wpisał nazwę i nie anulował – wyjście z menu, przejście do gry
            return true;
        }
        render();
    }
    // Jeśli okno zostało zamknięte lub wybrano Exit, kończymy bez uruchamiania gry
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
                    inputPlayerName();
                    // Jeśli użytkownik nie anulował wpisywania (ESC), to rozpoczynamy grę
                    if (!m_playerName.empty()) {
                        m_startGame = true;
                        return;
                    }
                    // Jeśli anulował, wracamy do menu i resetujemy zaznaczenie
                }
                else if (choice == "Instructions") {
                    showInstructions();
                }
                else if (choice == "Leaderboard") {
                    displayPlayers();
                }
                else if (choice == "Exit") {
                    m_window.close();
                    return;
                }
                initializeMenu();
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
    sf::Text footer("Press Enter to select/confirm options", m_font, 20);
    footer.setFillColor(sf::Color::White);
    sf::FloatRect fBounds = footer.getLocalBounds();
    float fx = (static_cast<float>(m_window.getSize().x) - fBounds.width) / 2.f - fBounds.left;
    float fy = static_cast<float>(m_window.getSize().y) - fBounds.height - 10.f;
    footer.setPosition(fx, fy);
    m_window.draw(footer);

    m_window.display();
}

void Menu::showInstructions() {
    sf::Text instructions(
        "Use arrow keys to navigate.\n"
        "Press Enter to confirm.\n"
        "Press ESC to return to menu.",
        m_font, 28
        );
    instructions.setFillColor(sf::Color::White);

    auto winSize = m_window.getSize();
    sf::FloatRect bounds = instructions.getLocalBounds();
    instructions.setPosition(
        (static_cast<float>(winSize.x) - bounds.width) / 2.f - bounds.left,
        (static_cast<float>(winSize.y) - bounds.height) / 2.f - bounds.top
        );

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
        m_window.draw(instructions);
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
    std::vector<std::string> players = readPlayerNamesFromCSV("csv/players.csv");
    std::sort(players.begin(), players.end());

    // Pokazujemy tylko 10 najlepszych
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
            podiumNames.push_back(players[i]);
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
            std::string line = std::to_string(static_cast<int>(i) + 1) + ". " + players[i];
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
                    outFile << name << "\n";
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
        sf::Text footer("Press ESC to return to menu", m_font, 20);
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
        }
        // Komunikat „ESC to return to menu” na dole ekranu
        sf::Text footer("Press ESC to return to menu", m_font, 20);
        footer.setFillColor(sf::Color::White);
        sf::FloatRect fBounds = footer.getLocalBounds();
        float fx = (static_cast<float>(m_window.getSize().x) - fBounds.width) / 2.f - fBounds.left;
        float fy = static_cast<float>(m_window.getSize().y) - fBounds.height - 10.f;
        footer.setPosition(fx, fy);
        m_window.draw(footer);

        m_window.display();
    }

    if (aborted) return -1;

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
