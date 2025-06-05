#ifndef TIMER_H
#define TIMER_H

#include <SFML/Graphics.hpp>
#include <string>

class Timer {
public:
    Timer(int seconds);
    void pause();
    void resume();
    std::string getFormattedTime() const;

private:
    sf::Clock clock;
    sf::Time startTime;
    sf::Time pausedTime;
    sf::Time totalPausedDuration;
    int durationSeconds;
    bool isPaused = false;
};

#endif // TIMER_H
