from __future__ import annotations
from dataclasses import dataclass, field
from typing import Any
import json
from dataclasses import asdict
import sys
@dataclass
class EnumPropertyDefinition:
    id:str
    name: str
    type: str
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

@dataclass
class CustomProperties:
    definitions: list[EnumPropertyDefinition] = field(default_factory=list)

    @staticmethod
    def from_dict(d: list[dict]) -> CustomProperties:
        return CustomProperties(
            definitions=sorted(
                [EnumPropertyDefinition.from_dict(entry) for entry in d if entry["type"] == "enum"],
                key=lambda e: e.id
            ))
    @staticmethod
    def from_file(path: str) -> CustomProperties:
        with open(path, encoding="utf-8") as f:
            return CustomProperties.from_dict(json.load(f))


def writeJsonFile(path,indent):
    with open(path,'w') as f:
        return json.dump(path,indent=indent)

def loadJsonFile(path):
    with open(path, 'r') as f:
        return json.load(f)


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
"""
        header = generateAllCusomEnums(customEnums,header)
        file.write(header)

def extractCustomeEnums(customProperties:CustomProperties):
    result = {}
    for customProperty in customProperties.definitions:
        if customProperty.name == "TileInfoEnum":
            continue
        result[customProperty.name] = []
        for idx,enumNames in enumerate(customProperty.values):
            result[customProperty.name].append(enumNames)
    return result

PROPERTYES_BASE_JSON ="./TileMapEditorOutput/filledWithEnums/properties.json"
CPATH = "../main/Abstract/TILE_ENUMS.hpp"
customProperties = CustomProperties.from_dict(loadJsonFile(PROPERTYES_BASE_JSON))
mode = ""
if len(sys.argv) == 2:
    mode = sys.argv[1]
if mode != "update":
    print("Execute with python configTileset.py <update>")
if mode =="update":
    print("update")
    generateEnumFile([],extractCustomeEnums(customProperties),CPATH)
    print("--------------Updated ENUMS.hpp")
