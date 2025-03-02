#include <stdint.h>

class Time {
public:
	static uint64_t getEpochTimeMillisUTC();
	
	static void setInitialTimestamp();
	static uint64_t getSetedTimestamp();

private:
	static uint64_t timestamp;
};