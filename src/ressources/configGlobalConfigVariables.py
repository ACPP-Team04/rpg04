import json
import pandas as pd
import sys
APPLICATIONFILE = "./application_config.json"
APPLICATIONCFILE ="../main/Abstract/GlobalProperties.hpp"
def loadJsonFile(path):
    with open(path, 'r') as f:
        return json.load(f)

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

createStaticVariables(APPLICATIONFILE)