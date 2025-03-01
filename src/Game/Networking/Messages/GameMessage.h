#pragma once
#include <string>
#include <nlohmann/json.hpp>

class GameMessage {
public:
	enum class Format {JSON, FLATBUFFER};

	GameMessage(std::unique_ptr<std::string> rawData, Format format = Format::JSON);
	GameMessage(std::unique_ptr<std::vector<uint8_t>> flatBufferData);


	nlohmann::json toJson() const;
	std::vector<uint8_t> toFlatBuffer() const;

	std::string getRawData() const;
	Format getFormat() const;

	static GameMessage fromJson(const nlohmann::json& jsonData);
	static GameMessage fromFlatBuffer(const std::vector<uint8_t>& flatBufferData);

private:
	std::unique_ptr<std::string> rawData;
	std::unique_ptr<std::vector<uint8_t>> flatBufferData;
	Format format;
};