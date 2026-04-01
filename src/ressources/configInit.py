import json
import pandas as pd
import sys
TILEJSONFILE = "./TileMapEditorOutput/zombieset.tsj"
APPLICATIONFILE = "./application_config.json"
ENUM_FILE_LOCATION = "../main/Abstract/TILE_ENUMS.hpp"
APPLICATIONCFILE ="../main/Abstract/GlobalProperties.hpp"
TILESETCOLS = 0
def loadJsonFile(path):
    with open (path,'r') as f:
        return json.load(f)


def toDataFrame(data):
    global TILESETCOLS
    TILESETCOLS = data['columns']
    rows = []
    typeInfo = {}
    for tile in data["tiles"]:
        row = {"id": tile["id"]}
        for prop in tile.get("properties", []):
            row[prop["name"]] = prop["value"]
            typeInfo[prop['name']] = prop["type"]
        rows.append(row)
    return pd.DataFrame(rows),typeInfo
def findGroupTiles(data):
    groupTiles = data[data['part'] == '00']
    newRow = []
    for idx, row in groupTiles.iterrows():
        name = row['name']
        id = row['id']
        variant = row['variant']
        height = row['height']
        width = row['width']
        if pd.isna(name) or pd.isna(height) or pd.isna(width):
            continue
        for i in range(int(width)):
            for j in range(int(height)):
                if i == 0 and j == 0:
                    continue
                newRow.append({
                    **row,
                    "id": id + i + j * TILESETCOLS,
                    "part": f"{j}{i}",

                })
    data = data[(data['part'] == '00') | (data['part'].isna())]
    data = pd.concat([data, pd.DataFrame(newRow)], ignore_index=True)
    return data

def writeDatafFrameBackToJson(jsonData,df,typeinfo,jsonFilePath):

    res = []
    df = df.sort_values('id')
    for idx, row in df.iterrows():
        entry = {}
        entry = {"id":row['id'],"properties":[]}
        for key in row.keys():
            if key == "id":
                continue
            if pd.isna(row[key]):
                continue
            props = {'name':key, 'value':row[key],'type':typeinfo[key]}
            entry['properties'].append(props)
        res.append(entry)
    jsonData['tiles']=res

    with open(jsonFilePath,'w')as file:
        json.dump(jsonData, file,indent=4)

def generateTypInfo(data,id):
    tile_w = data['tilewidth']
    tile_h = data['tileheight']
    cols = data['columns']
    enumId ={}
    column = id % cols
    row = id // cols
    pixelX = column * tile_w
    pixelY = row * tile_h
    width = tile_w
    height = tile_h
    tileInfo = [pixelX,pixelY,width,height]
    return tileInfo

def generateEnumOfTiles(data):
    res = []


    df,_ = toDataFrame(data)

    groupedByNameAndVariant = df.groupby(["variant", "name"], dropna=False)
    globEnums = {}
    enumToArrays = {}
    for group in groupedByNameAndVariant:
        group_df = group[1].sort_values('part', na_position='first')
        variant = group[0][0]
        name = group[0][1]
        part00 = group_df[group_df['part'] == '00']
        if not part00.empty:
            firstGroubMemberId = int(part00.iloc[0]['id'])
        else:
            firstGroubMemberId = int(group_df['id'].min())
        enumname = f"{name}{'_'+variant if not pd.isna(variant) else ''}"
        globEnums[enumname] = firstGroubMemberId
        if group[1]['part'].isna().all():

            enumToArrays[enumname] = [[generateTypInfo(data,firstGroubMemberId)]]
            continue

        height = int(group_df.iloc[0]['height'])
        width = int(group_df.iloc[0]['width'])
        grid = []
        for _ in range(height):
            grid.append([-1]*width)

        for idx,row in group_df.iterrows():
            partDigits = list(str(row['part']))
            i = int(partDigits[0])
            j = int(partDigits[1])
            grid[i][j] = generateTypInfo(data,row['id'])

        enumToArrays[enumname] = grid

        print(enumToArrays)

    return enumToArrays,globEnums
def tileInfoToCpp(tileInfo):
    return f"{{{tileInfo[0]}, {tileInfo[1]}, {tileInfo[2]}, {tileInfo[3]}}}"

def gridToCpp(grid):
    rows = []
    for row in grid:
        cells = ", ".join(tileInfoToCpp(cell) for cell in row)
        rows.append(f"{{{cells}}}")
    return "{" + ", ".join(rows) + "}"
def generateENUMFile(globalEnums,enumToArrays):
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
enum TileType {
"""
        for name in globalEnums.keys():
            id = globalEnums[name]
            header_content += f"    TILE_{name.upper()} = {id},\n"

        header_content += """};
inline const std::unordered_map<TileType, std::vector<std::vector<TileInfo>>> TILE_DICT = {
"""
        for name in enumToArrays.keys():
            values = gridToCpp(enumToArrays[name])
            header_content += f'    {{ TILE_{name.upper()}, {values} }},\n'


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



def updateTileMap():
    res = loadJsonFile(TILEJSONFILE)
    df,typeInfo = toDataFrame(res)
    df = findGroupTiles(df)
    writeDatafFrameBackToJson(res,df,typeInfo,TILEJSONFILE)
def generateEnums():
    print("sss")
    res = loadJsonFile(TILEJSONFILE)
    enumArrays,globalEnums = generateEnumOfTiles(res)
    print(globalEnums)
    generateENUMFile(globalEnums,enumArrays)
    createStaticVariables(APPLICATIONFILE)

mode = "generate"
if len(sys.argv) == 2:
    mode = sys.argv[1]

if mode =="update":
    print("update")
    updateTileMap()
else:
    generateEnums()