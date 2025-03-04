#include "MessageHandler.h"

#include <nlohmann/json.hpp>

using nlohmann::json;

GameMessage MessageHandler::serializePlayersStatus(std::unordered_map<int, Player> players) {
    json playerData = {
        {"type", "update"}
    };

    for (auto& [id, player] : players) {
        if (!player.isAlive()) {
            continue;
        }
        playerData["players"].push_back({
            {"id",       id},
            {"nickname", player.getNickname()},
            {"x",        player.getX()},
            {"y",        player.getY()}
            });
    }

    return GameMessage::fromJson(playerData);
}

GameMessage MessageHandler::serializeDisconectedPlayer(int playerId) {
    
    json disconnectMsg = {
        {"type", "player_disconnect"},
        {"id", playerId}
    };

    return GameMessage::fromJson(disconnectMsg);
}

GameMessage MessageHandler::serializeEatEvent(int foodId, int playerId) {
    json foodEatenMessage = {
       {"type", "food_eaten"},
       {"id", foodId},
       {"playerId", playerId},
    };
    
    return GameMessage::fromJson(foodEatenMessage);
}

GameMessage MessageHandler::serializeFoodSpawn(std::vector<Food> newFoods) {
    json foodGeneratedMessage = {
        {"type", "food_spawn"}
    };

    for (auto& food : newFoods) {
        foodGeneratedMessage["foods"].push_back({
            {"id", food.id},
            {"x", food.x},
            {"y", food.y}
            });
    }

    return GameMessage::fromJson(foodGeneratedMessage);
}

GameMessage MessageHandler::serializeGameStateData(int playerId, GameState& gameState) {
    auto allPlayers = gameState.getAllPlayers();

    json foodList = json::array();

    for (const auto& food : gameState.getFoodList()) {
        foodList.push_back({
            {"id", food.second.id},
            {"x", food.second.x},
            {"y", food.second.y}
            });
    }

    json response = {
        {"type", "connect"},
        {"id",  playerId},
        {"foods", foodList}
    };

    for (auto& [id, player] : allPlayers) {
        response["players"].push_back({
            {"id",       id},
            {"nickname", player.getNickname()},
            {"x",        player.getX()},
            {"y",        player.getY()},
            {"size",     player.getSize()},
            {"points",   player.getPoints()}
            });
    }

    return GameMessage::fromJson(response);
}

GameMessage MessageHandler::serializeNewPlayer(Player player) {
    json connectedMsg = {
        {"type", "player_connect"},
        {"id", player.getId()},
        {"x", player.getX()},
        {"y", player.getY()},
        {"nickname", player.getNickname()},
    };
    return GameMessage::fromJson(connectedMsg);
}

GameMessage MessageHandler::serializePong(uint64_t timestamp) {
    json pongMessage = {
        {"type", "pong"},
        {"timestamp", timestamp}
    };
    return GameMessage::fromJson(pongMessage);
}


GameMessage MessageHandler::killPlayer(Player assassin, Player victim) {
    json killMessage = {
        {"type", "kill"},
        {"assassin", assassin.getId()},
        {"victim", victim.getId()},
        {"victimSize", victim.getSize()}
    };
    return GameMessage::fromJson(killMessage);
}