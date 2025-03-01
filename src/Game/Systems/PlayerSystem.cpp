#include "PlayerSystem.h"

#include "../Config/GameConfig.h"

const int PLAYER_MAX_SIZE = 2240;

PlayerSystem::PlayerSystem(GameState& state) : gameState(state) {}


Player PlayerSystem::addPlayer(const std::string& nickname) {
    float startX = rand() % GameConfig::MAP_SIZE;
    float startY = rand() % GameConfig::MAP_SIZE;

    Player newPlayer(nextPlayerId, nickname, startX, startY, 0, 0, 64);
    gameState.addPlayer(newPlayer);
    nextPlayerId++;

    return newPlayer;
}

void PlayerSystem::removePlayer(int playerId) {
    gameState.removePlayer(playerId);
}

Player& PlayerSystem::getPlayer(int playerId) {
    return gameState.getPlayer(playerId);
}

void PlayerSystem::eatFood(Player& player) {
    if (player.getSize() < PLAYER_MAX_SIZE) {
        player.increaseSize(1);
    }

    player.addPoints(1);
}