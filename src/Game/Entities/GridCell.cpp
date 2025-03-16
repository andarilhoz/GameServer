#pragma once

#include <vector>

struct GridCell {
	int x, y;
	bool operator == (const GridCell& other) const {
		return x == other.x && y == other.y;
	}
};

namespace std {
	template<> struct hash<GridCell> {
		size_t operator()(const GridCell& cell) const {
			return hash<int>()(cell.x) ^ hash<int>()(cell.y);
		}
	};
}