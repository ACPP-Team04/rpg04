import json

TILEJSONFILE = "./TileMapEditorOutput/textJson.tsj"
APPLICATIONFILE = "./application_config.json"
ENUM_FILE_LOCATION = "../main/Abstract/TILE_ENUMS.hpp"
APPLICATIONCFILE ="../main/Abstract/GlobalProperties.hpp"
def loadJsonFile(path):
    with open (path,'r') as f:
        return json.load(f)

def generateEnumOfTiles(data):
    res = []
    tile_w = data['tilewidth']
    tile_h = data['tileheight']
    cols = data['columns']
    for tile in data['tiles']:
        properties = tile.get("properties",[])
        name = ''
        id = tile['id']
        for prop in properties:
            propName = prop.get("name",None)
            if propName == "name":
                name = prop.get("value",None)
            if name is None:
                raise Exception(id+ " No Name")
        enumId ={}
        enumId['id'] = id
        enumId['name'] = name
        column = id % cols
        row = id // cols
        enumId["PixelX"] = column * tile_w
        enumId["PixelY"] = row * tile_h
        enumId['tile_w'] = tile_w
        enumId['tile_h'] = tile_h
        res.append(enumId)

    return res

def generateENUMFile(data):
    with open(ENUM_FILE_LOCATION,"w") as file:
        header_content = """#pragma once
#include <string>
#include <unordered_map>
struct TileInfo{
    int PixelX;
    int PixelY;
    int width;
    int height;
};
enum TileType {
"""
        for tile in data:
            print(tile)
            name = str.upper(tile['name'])
            id = tile['id']
            header_content += f"    {name} = {id},\n"

        header_content += """};
inline const std::unordered_map<TileType, TileInfo> TILE_DICT = {
"""
        for tile in data:
            name = str.upper(tile['name'])
            r = tile['PixelX']
            c = tile['PixelY']
            w = tile['tile_w']
            h = tile['tile_h']
            header_content += f'    {{ {name}, {{ {r}, {c},{w},{h} }} }},\n'


        header_content += "};"
        file.write(header_content)

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


res = loadJsonFile(TILEJSONFILE)
res = generateEnumOfTiles(res)
generateENUMFile(res)
createStaticVariables(APPLICATIONFILE)