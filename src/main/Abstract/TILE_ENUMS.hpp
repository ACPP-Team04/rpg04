#pragma once
#include <string>
#include <unordered_map>
struct TileInfo {
	int PixelX;
	int PixelY;
	int width;
	int height;
};
enum TileType {
	PLAYER_IDLE_BACK = 548,
	PLAYER_IDLE_FRONT = 595,
};
inline const std::unordered_map<TileType, TileInfo> TILE_DICT = {
    {PLAYER_IDLE_BACK, {496, 176, 16, 16}},
    {PLAYER_IDLE_FRONT, {496, 192, 16, 16}},
};