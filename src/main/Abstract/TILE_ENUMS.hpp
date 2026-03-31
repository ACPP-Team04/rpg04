#pragma once
#include <string>
#include <unordered_map>
struct TileInfo{
    int PixelX;
    int PixelY;
    int width;
    int height;
};
enum TileType {
    PLAYER_IDLE_FRONT = 595,
};
inline const std::unordered_map<TileType, TileInfo> TILE_DICT = {
    { PLAYER_IDLE_FRONT, { 192, 496,16,16 },
}};