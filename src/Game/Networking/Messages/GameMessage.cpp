#include "GameMessage.h"

GameMessage::GameMessage(std::unique_ptr<std::string> rawData, Format format): rawData(std::move(rawData)), format(format) {}

GameMessage::GameMessage(std::unique_ptr<std::vector<uint8_t>> flatBufferData) : flatBufferData(std::move(flatBufferData)), format(Format::FLATBUFFER) {}

nlohmann::json GameMessage::toJson() const {
	return nlohmann::json::parse(*rawData);
}

std::vector<uint8_t> GameMessage::toFlatBuffer() const {
	return *flatBufferData;
}

std::string GameMessage::getRawData() const {
	return *rawData;
}

GameMessage::Format GameMessage::getFormat() const {
	return format;
}

GameMessage GameMessage::fromJson(const nlohmann::json& jsonData) {
	return GameMessage(std::make_unique<std::string>(jsonData.dump()), Format::JSON);
}

GameMessage GameMessage::fromFlatBuffer(const std::vector<uint8_t>& flatBufferData) {
	return GameMessage(std::make_unique<std::vector<uint8_t>>(flatBufferData));
}