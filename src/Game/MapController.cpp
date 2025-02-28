#include "MapController.h"

#include "../GameConfig.h"
#include <random>

MapController::MapController() {};


float MapController::generateRandomPosition(float itemSize) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(itemSize, GameConfig::MAP_SIZE - itemSize);
    return dist(gen);
}