from __future__ import annotations
from dataclasses import dataclass, field
from typing import Any
import json
from dataclasses import asdict
import sys
@dataclass
class TileConfig:
    name: str
    variant: str = ""
    hasParts: bool = False
    height: int = 1
    width: int = 1
#####AI generated [1] citation is in ../../LLMcites.txt###########################
# ---------------------------------------------------------------------------
# Property value types
# ---------------------------------------------------------------------------

@dataclass
class ClassValue:
    data: dict[str, Any]

    def to_dict(self) -> dict:
        return self.data

    def __repr__(self) -> str:
        return f"ClassValue({self.data})"


PropertyValue = bool | int | float | str | ClassValue


def parse_property_value(prop_type: str, raw_value: Any) -> PropertyValue:
    match prop_type:
        case "bool":
            return bool(raw_value)
        case "int":
            return int(raw_value)
        case "float":
            return float(raw_value)
        case "class":
            return ClassValue(raw_value if isinstance(raw_value, dict) else {})
        case _:
            return str(raw_value)


def serialize_property_value(value: PropertyValue) -> Any:
    if isinstance(value, ClassValue):
        return value.to_dict()
    return value


# ---------------------------------------------------------------------------
# Property
# ---------------------------------------------------------------------------

@dataclass
class TileProperty:
    name: str
    property_type: str
    type: str
    value: PropertyValue


    @staticmethod
    def from_dict(d: dict) -> TileProperty:
        raw_type = d.get("type", "string")
        return TileProperty(
            name=d["name"],
            property_type=d.get("propertytype") or d.get("propertyType", ""),
            type=raw_type,
            value=parse_property_value(raw_type, d.get("value"))
        )

    def to_dict(self, in_property_definition: bool = False) -> dict:
        key = "propertyType" if in_property_definition else "propertytype"
        return {
        "name": self.name,
        key: self.property_type,
        "type": self.type,
        "value": serialize_property_value(self.value),
    }


@dataclass
class PropertyDefinition:
    id:str
    name: str
    type: str

    @staticmethod
    def from_dict(d: dict) -> PropertyDefinition:
        if d["type"] == "class":
            return ClassPropertyDefinition.from_dict(d)
        return EnumPropertyDefinition.from_dict(d)

    def to_dict(self) -> dict:
        return {
            "name": self.name,
            "type": self.type,
        }


# ---------------------------------------------------------------------------
# Enum
# ---------------------------------------------------------------------------

@dataclass
class EnumPropertyDefinition(PropertyDefinition):
    valuesAsFlags: bool = False
    storageType: str = "string"
    values: list[str] = field(default_factory=list)

    @staticmethod
    def from_dict(d: dict) -> EnumPropertyDefinition:
        return EnumPropertyDefinition(
            name=d["name"],
            type=d.get("type", "enum"),
            valuesAsFlags=d.get("valuesAsFlags", False),
            storageType=d.get("storageType", "string"),
            values=list(d.get("values", [])),
            id = d.get('id',None)
        )

    def to_dict(self) -> dict:
        return {
            **super().to_dict(),
            "valuesAsFlags": self.valuesAsFlags,
            "storageType": self.storageType,
            "values": list(self.values),
            "id": self.id
        }


# ---------------------------------------------------------------------------
# Class
# ---------------------------------------------------------------------------

@dataclass
class ClassPropertyDefinition(PropertyDefinition):
    color: str = ""
    drawFill: bool = False
    members: list[TileProperty] = field(default_factory=list)
    useAs: list[str] = field(default_factory=list)

    @staticmethod
    def from_dict(d: dict) -> ClassPropertyDefinition:
        return ClassPropertyDefinition(
            name=d["name"],
            type=d.get("type", "class"),
            color=d.get("color", ""),
            drawFill=d.get("drawFill", False),
            members=[TileProperty.from_dict(m) for m in d.get("members", [])],
            useAs=list(d.get("useAs", [])),
            id = d.get('id',None)
        )

    def to_dict(self) -> dict:
        return {
            **super().to_dict(),
            "color": self.color,
            "drawFill": self.drawFill,
            "members": [m.to_dict(in_property_definition=True) for m in self.members],  # ← camelCase
            "useAs": list(self.useAs),
            "id": self.id
        }


# ---------------------------------------------------------------------------
# Container
# ---------------------------------------------------------------------------

@dataclass
class CustomProperties:
    definitions: list[PropertyDefinition] = field(default_factory=list)

    @staticmethod
    def from_dict(d: list[dict]) -> CustomProperties:
        return CustomProperties(
            definitions=sorted(
                [PropertyDefinition.from_dict(entry) for entry in d],
                key=lambda e: e.id
            ))

    @staticmethod
    def from_json(json_str: str) -> CustomProperties:
        return CustomProperties.from_dict(json.loads(json_str))

    @staticmethod
    def from_file(path: str) -> CustomProperties:
        with open(path, encoding="utf-8") as f:
            return CustomProperties.from_dict(json.load(f))

    def to_dict(self) -> list[dict]:
        for d in self.definitions:
            if d.type == "class" and d.name == "RENDER_COMPONENT":
                for member in d.members:
                    if member.name == "activeTile":
                        tileInfoEnum = self.get("TileInfoEnum")
                        if tileInfoEnum and tileInfoEnum.values:
                            member.value = tileInfoEnum.values[0]
        return [d.to_dict() for d in self.definitions]


    def to_json(self, indent: int = 1) -> str:
        return json.dumps(self.to_dict(), indent=indent, ensure_ascii=False)

    def to_file(self, path: str, indent: int = 1) -> None:
        with open(path, "w", encoding="utf-8") as f:
            json.dump(self.to_dict(), f, indent=indent, ensure_ascii=False)

    # --- Helpers -----------------------------------------------------------

    def get(self, name: str) -> PropertyDefinition | None:
        return next((d for d in self.definitions if d.name == name), None)

    def get_all_of_type(self, type_: str) -> list[PropertyDefinition]:
        return [d for d in self.definitions if d.type == type_]

# ---------------------------------------------------------------------------
# Tile
# ---------------------------------------------------------------------------

@dataclass
class Tile:
    id: int
    properties: list[TileProperty] = field(default_factory=list)
    tileConfig: TileConfig | None = field(default=None)


    def get_property(self, name: str) -> TileProperty | None:
        return next((p for p in self.properties if p.name == name), None)


    def convertClasValueToTileConfig(self,name,classValue:ClassValue):

        return TileConfig(
            **classValue.data,
            name = name
        )
    def get_Tile_config(self):
        return next((self.convertClasValueToTileConfig(p.name,p.value)
                     for p in self.properties if p.property_type == 'TileConfig'), None)

    def deleteTileConfigConfig(self):
        self.tileConfig = None
        keepProperties = []
        for p in self.properties:
            if p.property_type != 'TileConfig':
                keepProperties.append(p)
        self.properties = keepProperties
    def tileConfigValueDict(self,tc: TileConfig) -> dict:
        d = asdict(tc)
        d.pop('name', None)
        return d

    def replaceTileConfig(self, tileConfig: TileConfig):
        self.tileConfig = tileConfig
        for p in self.properties:
            if p.property_type == 'TileConfig':
                p.name = tileConfig.name
                p.value.data = self.tileConfigValueDict(tileConfig)
                return
        self.properties.insert(0, TileProperty(
            name=tileConfig.name,
            property_type='TileConfig',
            type='class',
            value=ClassValue(self.tileConfigValueDict(tileConfig)),
        ))
    def updateRenderComponent(self):
        activeTile = self.tileConfig.name
        hasNoRenderComponent = True
        for prop in self.properties:
            if prop.property_type == "RENDER_COMPONENT":
                hasNoRenderComponent = False
                prop.value = parse_property_value("class", {"activeTile": activeTile})
        if hasNoRenderComponent:
            prop = TileProperty(
                name='render_component',
                property_type="RENDER_COMPONENT",
                type="class",
                value=parse_property_value("class", {"activeTile":activeTile}),
            )
            self.properties.append(prop)

    @staticmethod
    def from_dict(d: dict) -> Tile:
        res = Tile(
            id=d["id"],
            properties=[TileProperty.from_dict(p) for p in d.get("properties", [])],
        )
        res.tileConfig = res.get_Tile_config()
        return res

    def to_dict(self) -> dict:
        d: dict[str, Any] = {"id": self.id}
        if self.properties:
            d["properties"] = [p.to_dict() for p in self.properties]
        return d


# ---------------------------------------------------------------------------
# Tileset
# ---------------------------------------------------------------------------

@dataclass
class Tileset:
    columns: int
    tilecount: int
    tileheight: int
    tilewidth: int
    tiles: list[Tile] = field(default_factory=list)
    extra: dict = field(default_factory=dict)

    KNOWN_FIELDS = {'columns', 'tilecount', 'tileheight', 'tilewidth', 'tiles'}

    @staticmethod
    def from_dict(d: dict) -> Tileset:
        extra = {k: v for k, v in d.items() if k not in Tileset.KNOWN_FIELDS}
        return Tileset(
            columns=d["columns"],
            tilecount=d["tilecount"],
            tileheight=d["tileheight"],
            tilewidth=d["tilewidth"],
            tiles=sorted([Tile.from_dict(t) for t in d.get("tiles", [])], key=lambda x: x.id),
            extra=extra,
        )

    def to_dict(self) -> dict:
        return {
            **self.extra,
            "columns": self.columns,
            "tilecount": self.tilecount,
            "tileheight": self.tileheight,
            "tilewidth": self.tilewidth,
            "tiles": [t.to_dict() for t in self.tiles],
        }

    def getTileIds(self):
        return [p.id for p in self.tiles]

    @staticmethod
    def from_json(json_str: str) -> Tileset:
        return Tileset.from_dict(json.loads(json_str))

    @staticmethod
    def from_file(path: str) -> Tileset:
        with open(path, encoding="utf-8") as f:
            return Tileset.from_dict(json.load(f))


    def updateRenderComponent(self):
        for tile in self.tiles:
            if tile.tileConfig is None:
                continue
            tile.updateRenderComponent()

    def to_json(self, indent: int = 1) -> str:
        return json.dumps(self.to_dict(), indent=indent, ensure_ascii=False)

    def to_file(self, path: str, indent: int = 1) -> None:
        with open(path, "w", encoding="utf-8") as f:
            json.dump(self.to_dict(), f, indent=indent, ensure_ascii=False)

    # --- Helpers ----------------------------------------------------------

    def get_tile(self, tile_id: int) -> Tile | None:
        return next((t for t in self.tiles if t.id == tile_id), None)

    def tiles_with_property(self, name: str) -> list[Tile]:
        return [t for t in self.tiles if t.get_property(name) is not None]

    def addEmptyTile(self, tileID):
        tile = Tile(
            tileConfig=None,
            id=tileID,
            properties=[]
        )
        self.tiles.append(tile)


##################################################################################

def writeJsonFile(path,indent):
    with open(path,'w') as f:
        return json.dump(path,indent=indent)

def loadJsonFile(path):
    with open(path, 'r') as f:
        return json.load(f)


def tileToEnumParParentName(tile:Tile):

    return f"{tile.tileConfig.name.upper()}{'_' + tile.tileConfig.variant if tile.tileConfig.variant is not None else ''}"
def tileToEnumPartName(tile:Tile,x,y):
    return f'{tileToEnumParParentName(tile).upper()}_{x}{y}'

def getPixelInfo(id,tileset,partWidth,partHeight):
    tileColumns = tileset.columns
    tileWidth = tileset.tilewidth
    tileHeight = tileset.tileheight
    column = id % tileColumns
    row = id // tileColumns

    pixelX = column * tileWidth
    pixelY = row * tileHeight

    fullWidth = tileWidth * partWidth
    fullHeight = tileHeight * partHeight

    return [pixelX, pixelY, fullWidth, fullHeight]


def getEums(tileSet: Tileset):
    enumParts = {}
    enumParent = {}
    tileIdsWithHasPartsOrNoObject = []
    for tile in tileSet.tiles:
        if tile.tileConfig is None:
            continue

        height = tile.tileConfig.height
        width = tile.tileConfig.width
        if (height == 1 and width == 1 and not tile.tileConfig.hasParts):
            enumParent[tileToEnumParParentName(tile)] = [tile.id,getPixelInfo(tile.id,tileSet,1,1)]
            tileIdsWithHasPartsOrNoObject.append(tile.id)

        if tile.tileConfig.hasParts:
            enumParent[tileToEnumParParentName(tile)] = [tile.id,getPixelInfo(tile.id,tileSet,width,height)]
            tileIdsWithHasPartsOrNoObject.append(tile.id)
        for y in range(height):
            for x in range(width):
                actualId = tile.id + x + (y * tileSet.columns)
                enumParts[tileToEnumPartName(tile, y, x)] = [actualId, getPixelInfo(actualId, tileSet, 1, 1)]



    return enumParent,enumParts,tileIdsWithHasPartsOrNoObject

def updateTilesSetWithNewConfig(tileSet: Tileset, tileIdsWithHasPartsOrNoObject, enumParts):
    for tile in tileSet.tiles:
        if tile.tileConfig is None:
            continue
        if tile.id not in tileIdsWithHasPartsOrNoObject:
            tile.deleteTileConfigConfig()
    for name, info in enumParts.items():
        actualId = info[0]
        if tileSet.get_tile(actualId) is None:
            tileSet.addEmptyTile(actualId)

    id_to_name: dict[int, str] = {info[0]: name for name, info in enumParts.items()}

    hasParts_tiles = [t for t in tileSet.tiles
                      if t.tileConfig is not None and t.tileConfig.hasParts]

    for tile in hasParts_tiles:
        height = tile.tileConfig.height
        width = tile.tileConfig.width
        for y in range(height):
            for x in range(width):
                actualId = tile.id + x + (y * tileSet.columns)
                partName = id_to_name.get(actualId)
                if partName is None:
                    continue
                partTile = tileSet.get_tile(actualId)
                if partTile is None:
                    partTile = Tile(actualId, [], None)
                    tileSet.tiles.append(partTile)
                partTile.replaceTileConfig(TileConfig(
                    hasParts=(actualId==tile.id), name=partName,
                    variant=tile.tileConfig.variant,
                    height=height, width=width,
                ))

    for tile in tileSet.tiles:
        if tile.tileConfig is None or tile.tileConfig.hasParts:
            continue
        partName = id_to_name.get(tile.id)
        if partName is not None:
            tile.replaceTileConfig(TileConfig(
                name=partName,
                variant=tile.tileConfig.variant,
                hasParts=False,
                height=tile.tileConfig.height,
                width=tile.tileConfig.width,
            ))

def copyParamFromParentObject(tileset: Tileset):
    for tile in tileset.tiles:
        if tile.tileConfig is None or not tile.tileConfig.hasParts:
            continue
        propsToCopy =[]
        for p in tile.properties:
            if p.property_type is None or (p.property_type in ['TileConfig', 'RENDER_COMPONENT']):
                continue
            propsToCopy.append(p)


        height = tile.tileConfig.height
        width = tile.tileConfig.width
        for y in range(height):
            for x in range(width):
                actualId = tile.id + x + (y * tileset.columns)
                childTile = tileset.get_tile(actualId)
                if childTile is None:
                    continue
                for prop in propsToCopy:
                    if childTile.get_property(prop.name) is None:
                        childTile.properties.append(prop)
def generateENUMFile(enumParent, enumParts, path):
    allEnums = {**enumParts, **enumParent}

    with open(path, "w") as file:
        header_content = """#pragma once
#include <string>
#include <unordered_map>

struct TileInfo {
    int pixelX;
    int pixelY;
    int width;
    int height;
};

enum TileType {
"""
        alreadIn = []
        allCEnums = []
        for name in enumParts.keys():
            i = enumParts[name][0]
            header_content += f"    {name.upper()} = {i},\n"
            alreadIn.append(i)
            allCEnums.append(name)

        for name in enumParent.keys():
            i = enumParent[name][0]
            if i in alreadIn:
                continue
            header_content += f"    {name.upper()} = {i},\n"
            allCEnums.append(name)

        header_content += "};\n\n"

        header_content += "inline const std::unordered_map<std::string, TileInfo> TILE_DICT = {\n"
        for i, (name, data) in enumerate(allEnums.items()):
            values = data[1]
            header_content += f'    {{ "{name.upper()}", {{{values[0]}, {values[1]}, {values[2]}, {values[3]}}}}},\n'
        header_content += "};\n\n"

        header_content += "inline const std::unordered_map<TileType, std::string> TILE_ENUM_ID_TO_STRING = {\n"
        for name in allCEnums:
            header_content += f'    {{ {name.upper()}, "{name.upper()}" }},\n'
        header_content += "};\n"

        file.write(header_content)

def updateTilSetWithPartsAndEnums(tileSet:Tileset):
    enumParent,enumParts,tileIdsWithHasPartsOrNoObject = getEums(tileSet)


    updateTilesSetWithNewConfig(tileSet,tileIdsWithHasPartsOrNoObject,enumParts)
    copyParamFromParentObject(tileSet)
    tileSet.updateRenderComponent()
    tileSet.tiles.sort(key=lambda t: t.id)
    resultEnums = []

    for name in sorted(enumParts):
        resultEnums.append(name.upper())
    for name in sorted(enumParent):
        resultEnums.append(name.upper())

    return resultEnums,enumParent,enumParts





TILEJSONFILE = "./TileMapEditorOutput/base_config/Tilesheet_BaseConfig.tsj"
TILEJSONOUTPUTFILE = "./TileMapEditorOutput/filledWithEnums/Tilesheet_WithEnums.tsj"
PROPERTYES_BASE_JSON = "./TileMapEditorOutput/base_config/mainProperty.json"
PROPERTYES_FILLED_ENUMS_JSON = "./TileMapEditorOutput/filledWithEnums/properties.json"
CPATH = "../main/Abstract/TILE_ENUMS.hpp"
tileset = Tileset.from_dict(loadJsonFile(TILEJSONFILE))
customProperties = CustomProperties.from_dict(loadJsonFile(PROPERTYES_BASE_JSON))
resultEnums,enumParent,enumParts = updateTilSetWithPartsAndEnums(tileSet=tileset)

mode = ""
if len(sys.argv) == 2:
    mode = sys.argv[1]
if mode != "update":
    print("Execute with python configTileset.py <update>")
if mode =="update":
    print("update")
    customProperties.get("TileInfoEnum").values = resultEnums
    customProperties.to_file(PROPERTYES_FILLED_ENUMS_JSON,4)
    tileset.to_file(TILEJSONOUTPUTFILE,4)
    print("--------------Updated TILESET")
    generateENUMFile(enumParent,enumParts,CPATH)
    print("--------------Updated TILE_ENUMS.hpp")
