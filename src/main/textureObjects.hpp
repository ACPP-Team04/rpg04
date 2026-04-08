
#pragma once
#include "Abstract/TILE_ENUMS.hpp"

#include <vector>

enum class TileObject {
	CAR

};

std::vector<std::vector<TileType>> car = {
    {CAR_PART_00, CAR_PART_01},
};

inline std::unordered_map<TileObject, std::vector<std::vector<TileType>>> TILET()
{
	return {TileObject::CAR, car};
}