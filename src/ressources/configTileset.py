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
    def updateRenderComponent(self,enumNum):
        hasNoRenderComponent = True
        for prop in self.properties:
            if prop.property_type == "SPRITE_COMPONENT":
                hasNoRenderComponent = False
                prop.value = parse_property_value("class", {"texture": int(enumNum)})
        if hasNoRenderComponent:
            prop = TileProperty(
                name='SPRITE_COMPONENT',
                property_type="SPRITE_COMPONENT",
                type="class",
                value=parse_property_value("class", {"texture": int(enumNum)}),
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
            "tiles": [t.to_dict() for t in sorted(self.tiles, key=lambda t: t.id)],
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

    return f"{tile.tileConfig.name.upper()}{'_' + tile.tileConfig.variant.upper() if tile.tileConfig.variant is not None else ''}"
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



def placeTileConfig(tileSet:Tileset):
    enumParts = {}
    enumParent = {}
    idalreadyTaken = []
    for tile in tileSet.tiles:
        if tile.tileConfig is None:
            continue
        if tile.id in idalreadyTaken:
            continue
        height = tile.tileConfig.height
        width = tile.tileConfig.width
        if (height == 1 and width == 1 and not tile.tileConfig.hasParts):
            enumParent[tile.id] = [tileToEnumParParentName(tile),getPixelInfo(tile.id,tileSet,1,1),tile.tileConfig,tile.properties]
            idalreadyTaken.append(tile.id)
            continue
        if (tile.tileConfig.hasParts):
            enumParent[tile.id] = [tileToEnumParParentName(tile),getPixelInfo(tile.id,tileSet,width,height),tile.tileConfig,tile.properties]
        for y in range(height):
            for x in range(width):
                actualId = tile.id + x + (y * tileSet.columns)
                enumParts[actualId]= [tileToEnumPartName(tile,y,x),getPixelInfo(actualId,tileSet,1,1),tile.tileConfig,tile.properties]
                idalreadyTaken.append(actualId)
    return enumParent,enumParts

import copy

def replaceConfigAndProps(enumParts, tileset: Tileset):
    for keyId in enumParts.keys():
        if tileset.get_tile(keyId) is None:
            tileset.addEmptyTile(keyId)

    for idx, keyId in enumerate(enumParts.keys()):
        tile = tileset.get_tile(keyId)
        if tile is None:
            continue
        entry = enumParts[keyId]
        enumName = entry[0]
        parentConfig = entry[2]
        tile.properties = copy.deepcopy(entry[3])
        newConfig = TileConfig(
            name=enumName,
            variant=parentConfig.variant,
            hasParts=parentConfig.hasParts,
            height=parentConfig.height,
            width=parentConfig.width,
        )
        tile.replaceTileConfig(newConfig)
        tile.updateRenderComponent(idx)

def flatEnumDicts(enumDicts, offset=0):
    return [
        {"name": enumDicts[key][0], "number": idx + offset, "pixelInfo": enumDicts[key][1]}
        for idx, key in enumerate(enumDicts.keys())
    ]

def flattenAndConcatEnums(enumParts, enumParents):
    parts = flatEnumDicts(enumParts)
    parents = flatEnumDicts(enumParents, offset=len(enumParts))
    return parts + parents

def generateTileEnums(flatEnums,header):

    header+= """
struct TileInfo {
    int pixelX;
    int pixelY;
    int width;
    int height;
    };

enum TileType {
"""
    for key in flatEnums:
        header+=f"{key['name'].upper()} = {key['number']},\n"

    header+="""};"""
    return header

def generateTilEnumMap(flatEnums, header):
    header += "inline const std::unordered_map<TileType, TileInfo> TILE_DICT = {\n"
    for key in flatEnums:
        name = key['name']
        values = key['pixelInfo']
        header += f'    {{ {name.upper()}, {{{values[0]}, {values[1]}, {values[2]}, {values[3]}}}}},\n'
    header += "};\n"
    return header

def generateAllCusomEnums(customEnums, header):
    for name in customEnums.keys():
        header += f"\nenum {name.upper()} {{\n"
        for idx, enumValue in enumerate(customEnums[name]):
            header += f"    {enumValue} = {idx},\n"
        header += "};\n"
    return header
def generateEnumFile(flatTileEnums,customEnums,path):
    with open(path, "w") as file:
        header = """#pragma once
#include <string>
#include <unordered_map>
"""
        header = generateTileEnums(flatTileEnums,header)
        header = generateAllCusomEnums(customEnums,header)
        header = generateTilEnumMap(flatTileEnums,header)
        file.write(header)

def extractCustomeEnums(customProperties:CustomProperties):
    result = {}
    for customProperty in customProperties.get_all_of_type("enum"):
        if customProperty.name == "TileInfoEnum":
            continue
        result[customProperty.name] = []
        for idx,enumNames in enumerate(customProperty.values):
            result[customProperty.name].append(enumNames)
    return result



TILEJSONFILE = "./TileMapEditorOutput/base_config/Tilesheet_BaseConfig.tsj"
TILEJSONOUTPUTFILE = "./TileMapEditorOutput/filledWithEnums/Tilesheet_WithEnums.tsj"
PROPERTYES_BASE_JSON = "./TileMapEditorOutput/base_config/mainProperty.json"
PROPERTYES_FILLED_ENUMS_JSON = "./TileMapEditorOutput/filledWithEnums/properties.json"
CPATH = "../main/Abstract/TILE_ENUMS.hpp"
tileset = Tileset.from_dict(loadJsonFile(TILEJSONFILE))
customProperties = CustomProperties.from_dict(loadJsonFile(PROPERTYES_BASE_JSON))
mode = ""
if len(sys.argv) == 2:
    mode = sys.argv[1]
if mode != "update":
    print("Execute with python configTileset.py <update>")
if mode =="update":
    print("update")
    enumParent,enumParts = placeTileConfig(tileset)
    replaceConfigAndProps(enumParts=enumParts,tileset=tileset)
    resultTileEnums = flattenAndConcatEnums(enumParts=enumParts,enumParents=enumParent)
    customProperties.get("TileInfoEnum").values = [val['name'] for val in resultTileEnums]
    customProperties.to_file(PROPERTYES_FILLED_ENUMS_JSON,4)
    generateEnumFile(resultTileEnums,extractCustomeEnums(customProperties),CPATH)
    tileset.to_file(TILEJSONOUTPUTFILE,4)
    print("--------------Updated TILESET")
    print("--------------Updated TILE_ENUMS.hpp")
