#include "ClientUdpMessage.h"

ClientUdpMessage::ClientUdpMessage(MessageType messageType, int playerId, float x, float y) 
	: messageType(messageType), playerId(playerId), x(x), y(y) {};