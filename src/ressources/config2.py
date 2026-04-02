import json
import os


TILEJSONFILE = "./TileMapEditorOutput/zombieset.tsj"
PROPERTYES_JSON = "./TileMapEditorOutput/propertytypes.json"
ENUM_FILE_LOCATION = "../main/Abstract/TILE_ENUMS.hpp"
TILEWIDTH = 16
TILEHEIGHT = 16
TILECOLUMNS = 45
APPLICATIONCFILE ="../main/Abstract/GlobalProperties.hpp"
APPLICATIONFILE = "./application_config.json"
def loadJsonFile(path):
    with open(path, 'r') as f:
        return json.load(f)

class TileInfo:
    def __init__(self, id, name, variant=""):
        global TILEWIDTH
        global TILEHEIGHT
        global TILECOLUMNS
        self.id = id
        self.name = name
        self.variant = variant
        self.partWidth = 1
        self.partHeight = 1
        self.tileWidth = TILEWIDTH
        self.tileHeight = TILEHEIGHT
        self.tileColumns = TILECOLUMNS

    def getEnumNameParent(self):
        v = f"_{self.variant.upper()}" if self.variant else ""
        return f"{self.name.upper()}{v}"

    def getEnumNamePart(self, x, y):
        v = f"_{self.variant.upper()}" if self.variant else ""
        return f"{self.name.upper()}{v}_PART_{y}{x}"

    def getParentPixelInfo(self):
        column = self.id % self.tileColumns
        row = self.id // self.tileColumns

        pixelX = column * self.tileWidth
        pixelY = row * self.tileHeight

        fullWidth = self.tileWidth * self.partWidth
        fullHeight = self.tileHeight * self.partHeight

        return [pixelX, pixelY, fullWidth, fullHeight]

    def getPartPixelInfo(self, x, y):

        actual_id = self.id + x + (y * self.tileColumns)

        column = actual_id % self.tileColumns
        row = actual_id // self.tileColumns

        pixelX = column * self.tileWidth
        pixelY = row * self.tileHeight
        return [pixelX, pixelY, self.tileWidth, self.tileHeight]


def parseTileInfo(tileId, valueDict, nameFromProp):
    info = TileInfo(tileId, nameFromProp, valueDict.get("variant", ""))

    info.hasPart = valueDict.get("hasParts", False)
    if info.hasPart:
        info.partWidth = valueDict.get("width", 1)
        info.partHeight = valueDict.get("height", 1)

    return info

res = loadJsonFile(TILEJSONFILE)
tileInfos = {}

if 'tiles' in res:
    for tile in res['tiles']:
        id = tile['id']
        for prop in tile.get('properties', []):
            if prop.get('propertytype') == 'TileConfig':
                info = parseTileInfo(id, prop['value'], prop['name'])
                tileInfos[id] = info
                break

def generateAllEnums(tileDict):
    enums = set()
    for id in tileDict:
        info = tileDict[id]
        enums.add(info.getEnumNameParent())
        if info.hasPart:
            for y in range(info.partHeight):
                for x in range(info.partWidth):
                    enums.add(info.getEnumNamePart(x, y))

    return sorted(list(enums))


def generateAllEnumsWithPixelInfo(tileDict):
    enums = {}
    for id in tileDict:
        info = tileDict[id]
        enums[info.getEnumNameParent()] = {"pixelInfo":info.getParentPixelInfo(),"id":id, "type":"hasPart"}
        if info.hasPart:
            for y in range(info.partHeight):
                for x in range(info.partWidth):
                    actualId = id + x + (y * TILECOLUMNS)
                    enums[info.getEnumNamePart(x, y)] = {"pixelInfo":info.getPartPixelInfo(x,y),"id":actualId, "type":"isPart"}

    return enums

def updateTilesetWithenum(data, tileInfos):
    idToEnum = {}
    for tilId, info in tileInfos.items():
        idToEnum[tilId] = info.getEnumNameParent()

        if info.hasPart:
            for y in range(info.partHeight):
                for x in range(info.partWidth):
                    actual_part_id = tilId + x + (y * TILECOLUMNS)
                    idToEnum[actual_part_id] = info.getEnumNamePart(x, y)

    if 'tiles' not in data:
        data['tiles'] = []

    existingTileIds = {tile['id']: tile for tile in data['tiles']}
    for targetTileid, enumName in idToEnum.items():
        if targetTileid in existingTileIds:
            tileObj = existingTileIds[targetTileid]
        else:
            tileObj = {"id": targetTileid, "properties": []}
            data['tiles'].append(tileObj)
        found_prop = False
        if "properties" not in tileObj:
            tileObj["properties"] = []

        for prop in tileObj["properties"]:
            if prop["name"] == "render_component":
                prop["type"] = "class"
                prop["propertytype"] = "RENDER_COMPONENT"
                prop["value"] = {"activeTile":enumName}


            if prop["name"] == "transform_component":
                prop["type"] = "class"
                prop["propertytype"] = "TRANSFORM_COMPONENT"
                prop["value"] = {}
                found_prop = True
                break

        if not found_prop:
            tileObj["properties"].append({
                "name": "render_component",
                "type": "class",
                "propertytype":"RENDER_COMPONENT",
                "value": {
                    "activeTile": f"{enumName}"
                }
            })
            tileObj["properties"].append({
                "name": "transform_component",
                "type": "class",
                "propertytype":"TRANSFORM_COMPONENT",
                "value": {}
            })
    data['tiles'] = sorted(data['tiles'], key=lambda x: x['id'])
    with open(TILEJSONFILE,"w") as file:
        json.dump(data, file, indent=4)

propJson = loadJsonFile(PROPERTYES_JSON)
new_values = generateAllEnums(tileInfos)

highestId = 0
target_prop = None

for prop in propJson:
    if prop.get('name') == "TileInfoEnum":
        target_prop = prop
    if prop.get('id', 0) > highestId:
        highestId = prop['id']

if target_prop:
    target_prop['values'] = new_values
else:
    propJson.append({
        "id": highestId + 1,
        "name": "TileInfoEnum",
        "storageType": "string",
        "type": "enum",
        "values": new_values,
        "valuesAsFlags": False
    })

updateTilesetWithenum(res,tileInfos)
def generateENUMFile(enumList):
    with open(ENUM_FILE_LOCATION,"w") as file:
        header_content = """#pragma once
#include <string>
#include <vector>
#include <unordered_map>
struct TileInfo{
    int PixelX;
    int PixelY;
    int width;
    int height;
};
enum TileType{
    



"""
        for name, data in enumList.items():
            if data['type'] == 'isPart':
                header_content += f"    {name} = {data['id']},\n"

        header_content += "};\n\n"

        header_content +="""
inline const std::unordered_map<std::string, TileInfo> TILE_DICT = {
"""
        for name in enumList.keys():
            values = enumList[name]['pixelInfo']
            header_content += f'    {{ "{name.upper()}", {{{values[0]},{values[1]},{values[2]},{values[3]}}}}},\n'
        header_content += "};"
        header_content += "inline const std::unordered_map<TileType, std::string> TILE_ENUM_ID_TO_STRING = {\n"
        for name, data in enumList.items():
            if data['type'] == 'isPart':
                header_content += f'    {{ {name}, "{name.upper()}" }},\n'

        header_content += "};"
        file.write(header_content)

with open(PROPERTYES_JSON, 'w') as file:
    json.dump(propJson, file, indent=4)

generateENUMFile(generateAllEnumsWithPixelInfo(tileInfos))

def createStaticVariables(path):
    res = loadJsonFile(path)
    with open(APPLICATIONCFILE,'w') as f:
        header = f"""
#pragma once     
        """
        for prop, value in res.items():
            if isinstance(value, str):
                formatted_value = f'"{value}"'
            elif isinstance(value, bool):
                formatted_value = str(value).lower()
            else:
                formatted_value = value
            header += f"#define {prop.upper()} {formatted_value}\n"
        f.write(header)