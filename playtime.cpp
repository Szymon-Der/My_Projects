#include "playtime.h"
#include <fstream>
#include <sstream>
#include <map>
#include <iomanip>

struct Record {
    int totalTime;
    int lvlTime[3];
};

static std::map<std::string,Record> loadAll() {
    std::ifstream in("csv/players.csv");
    std::string line;
    std::map<std::string,Record> m;
    while (std::getline(in,line)) {
        std::stringstream ss(line);
        std::string name;
        Record r{0,{0,0,0}};
        char comma;
        // parsujemy: name,total,l1,l2,l3
        if (std::getline(ss,name,',')
            && ss >> r.totalTime
            && ss >> comma >> r.lvlTime[0]
            && ss >> comma >> r.lvlTime[1]
            && ss >> comma >> r.lvlTime[2])
        {
            m[name] = r;
        }
    }
    return m;
}

static void saveAll(const std::map<std::string,Record>& m) {
    std::ofstream out("csv/players.csv");
    for (auto& [name,r] : m) {
        out << name << ","
            << r.totalTime << ","
            << r.lvlTime[0] << ","
            << r.lvlTime[1] << ","
            << r.lvlTime[2]
            << "\n";
    }
}

void updatePlayTime(const std::string& playerName, int seconds) {
    auto m = loadAll();
    m[playerName].totalTime += seconds;
    saveAll(m);
}

int readPlayTime(const std::string& playerName) {
    auto m = loadAll();
    if (m.count(playerName)) return m[playerName].totalTime;
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

int readLevelTime(const std::string& playerName, int levelNumber) {
    auto m = loadAll();
    if (m.count(playerName) && levelNumber >= 1 && levelNumber <= 3)
        return m[playerName].lvlTime[levelNumber-1];
    return 0;
}

void updateLevelTime(const std::string& playerName, int levelNumber, int elapsedSeconds) {
    auto m = loadAll();
    if (!m.count(playerName) || levelNumber < 1 || levelNumber > 3) return;
    m[playerName].lvlTime[levelNumber-1] = elapsedSeconds;
    saveAll(m);
}
