#include "MapController.h"

#include <random>

constexpr float MAP_SIZE = 5000.0f;

MapController::MapController() {};


float MapController::generateRandomPosition(float itemSize) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(itemSize, MAP_SIZE - itemSize);
    return dist(gen);
}