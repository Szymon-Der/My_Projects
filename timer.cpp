#include "timer.h"
#include <sstream>
#include <iomanip>

Timer::Timer(int seconds) : durationSeconds(seconds) {
    startTime = clock.getElapsedTime();
    pausedTime = sf::Time::Zero;
    totalPausedDuration = sf::Time::Zero;
}

void Timer::pause() {
    if (!isPaused) {
        pausedTime = clock.getElapsedTime();
        isPaused = true;
    }
}

void Timer::resume() {
    if (isPaused) {
        totalPausedDuration += clock.getElapsedTime() - pausedTime;
        isPaused = false;
    }
}

std::string Timer::getFormattedTime() const {
    sf::Time currentTime = isPaused ? pausedTime : clock.getElapsedTime();
    int elapsed = static_cast<int>((currentTime - startTime - totalPausedDuration).asSeconds());
    int remaining = durationSeconds - elapsed;
    if (remaining < 0) remaining = 0;

    int minutes = remaining / 60;
    int seconds = remaining % 60;

    std::ostringstream ss;
    ss << std::setw(2) << std::setfill('0') << minutes << ":"
       << std::setw(2) << std::setfill('0') << seconds;
    return ss.str();
}
