#include "playtime.h"
#include <fstream>
#include <sstream>
#include <map>
#include <iomanip>

void updatePlayTime(const std::string& playerName, int seconds) {
    std::map<std::string, int> players;

    std::ifstream in("csv/players.csv");
    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string name;
        int time;
        if (std::getline(ss, name, ',') && ss >> time) {
            players[name] = time;
        }
    }
    in.close();

    players[playerName] += seconds;

    std::ofstream out("csv/players.csv");
    for (const auto& [name, time] : players) {
        out << name << "," << time << "\n";
    }
}

int readPlayTime(const std::string& playerName) {
    std::ifstream in("csv/players.csv");
    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string name;
        int time;
        if (std::getline(ss, name, ',') && ss >> time) {
            if (name == playerName)
                return time;
        }
    }
    return 0;
}

std::string formatTime(int totalSeconds) {
    int h = totalSeconds / 3600;
    int m = (totalSeconds % 3600) / 60;
    int s = totalSeconds % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << h << ":"
        << std::setw(2) << std::setfill('0') << m << ":"
        << std::setw(2) << std::setfill('0') << s;
    return oss.str();
}
