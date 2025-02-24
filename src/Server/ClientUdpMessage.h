#pragma once

#include "MessageType.h"

class ClientUdpMessage {

public:
	ClientUdpMessage(MessageType messageType, int playerId, float x, float y);
	MessageType messageType;
	int playerId;
	float x;
	float y;
};
