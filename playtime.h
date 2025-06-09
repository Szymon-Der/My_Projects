#pragma once
#include <string>

void updatePlayTime(const std::string& playerName, int seconds);
int readPlayTime(const std::string& playerName);
std::string formatTime(int totalSeconds);
