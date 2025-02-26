#include "MapController.h"

#include <random>

constexpr float PLAYER_HALF_SIZE = 64.0f;
constexpr float MAP_SIZE = 5000.0f;

MapController::MapController() {};


float MapController::generateRandomPosition() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(PLAYER_HALF_SIZE, MAP_SIZE - PLAYER_HALF_SIZE);
    return dist(gen);
}