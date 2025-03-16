#pragma once

#include "../Networking/Messages/GameMessage.h"

namespace GameConfig {
	constexpr float DISCONECT_TIMEOUT = 25.0f;

	constexpr float MAP_SIZE = 2000;

	constexpr int MIN_FOOD = 500;

	constexpr int GRID_CELL_SIZE = 64;
	constexpr int FOOD_SIZE = 16;
	constexpr int FOOD_COUNT = 500;

	constexpr float SPEED = 200;
	constexpr float MAX_SPEED = 200.0f;
	constexpr float MIN_SPEED = 10.0f;

	constexpr float PLAYER_MIN_SIZE = 64.0f;
	constexpr float PLAYER_MAX_SIZE = 500.0f;

	constexpr int TCP_PORT = 5555;
	constexpr int UDP_PORT = 7777;
	constexpr GameMessage::Format format = GameMessage::Format::JSON;
};
