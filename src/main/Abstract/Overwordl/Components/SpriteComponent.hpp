#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "DefaultComponent.hpp"
struct TileInfo {
	int pixelX;
	int pixelY;
	int width;
	int height;
	std::string tilesetPath;
};

struct SpriteComponent : public Component<SpriteComponent>, DefaultComponent {
	TileInfo tileInfo;
	std::string tilesetPath;

	void readFromJson(tson::TiledClass &j) override {}
	void readFromObject(tson::Object &object, ParseContext &context) override
	{
		if (object.getObjectType() == tson::ObjectType::Object) {
			tson::Tileset *tileset = context.map->getTilesetByGid(object.getGid());
			int localId = object.getGid() - tileset->getFirstgid();
			int cols = tileset->getColumns();
			int tw = tileset->getTileSize().x;
			int th = tileset->getTileSize().y;

			tileInfo = {(localId % cols) * tw, (localId / cols) * th, tw, th};
			tilesetPath = (fs::path(MAP).parent_path() / tileset->getImagePath()).string();
		}
		if (object.getObjectType() == tson::ObjectType::Rectangle) {
			if (context.map->getTilesets().empty()) {
				throw std::runtime_error("You need to embedd a tileset!");
			}

			auto firsTileset = context.map->getTilesets()[0];

			int tilewidth = firsTileset.getTileSize().x;
			int tileheight = firsTileset.getTileSize().y;
			tileInfo = {0, 0, tilewidth, tileheight};
			tilesetPath = (fs::path(MAP).parent_path() / firsTileset.getImagePath()).string();
		}

		switch (object.getFlipFlags()) {

		case tson::TileFlipFlags::None:
			break;
		case tson::TileFlipFlags::Diagonally:
			break;
		case tson::TileFlipFlags::Vertically:
			tileInfo.pixelY += tileInfo.height;
			tileInfo.height *= -1;
		case tson::TileFlipFlags::Horizontally:
			tileInfo.pixelX += tileInfo.width;
			tileInfo.width *= -1;
		}

		tileInfo.tilesetPath = tilesetPath;
	}
};