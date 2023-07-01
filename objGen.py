import sys
import os
import json

#python script for generation of special object files

extractedProperties = ["direction", "half", "shape", "color", "part", "open", "snowy", "north", "west", "east", "south"]

def getKey(d:dict) -> str:
    """Internal function that properly formats the key for further processing"""
    res = []
    for k in d.keys():
        if k in extractedProperties:
            res.append("%s=%s" % (k, d[k]))
    return ",".join(res)

class cube:
    """A wrapper for 8 vertices and 6 faces"""
    def __init__(self, f:list, t:list) -> None:
        startx = f[0] / 16
        starty = f[1] / 16
        startz = f[2] / 16
        endx = t[0] / 16
        endy = t[1] / 16
        endz = t[2] / 16
        self.vertices = ((startx, starty, startz), (endx, starty, startz), (startx, endy, startz), (startx, starty, endz), 
                    (startx, endy, endz), (endx, starty, endz), (endx, endy, startz), (endx, endy, endz))
        self.faces = ((1, 0, 2, 3), (1, 0, 4, 5), (2, 0, 4, 6), (7, 3, 1, 5), (7, 5, 4, 6), (7, 6, 2, 3))

#recursive function for 
def getModel(name:str, modelFolder:str) -> list[cube]:
    """Returns all cubes that make up a model"""
    result = []
    filePath = os.path.join(modelFolder, name + ".json")
    with open(filePath, "r") as f:
        contents = json.load(f)
        if "parent" in contents.keys():
            parent = getModel(contents["parent"].replace("minecraft:block/", "").replace("block/", ""), modelFolder)
            result.extend(parent)
        if "elements" in contents.keys():
            for el in contents["elements"]:
                c = cube(el["from"], el["to"])
                if c.vertices not in result:
                    result.append(c)
    return result

def getBlockstates(pathToBlockstates:str) -> dict:
    """Gets all blockstate definitions within a given resourcepack directory and stores them in a dict"""
    models = {}
    #foreach json file in the blockstates directory
    for filename in os.listdir(pathToBlockstates):
        filePath = os.path.join(pathToBlockstates, filename)
        if filePath[-5:] == ".json":
            with open(filePath, "r") as j:
                #we open the file and parse the json
                contents = json.load(j)
                #annoyingly enough the format has two versions
                if "variants" in contents.keys():
                    for key, v in contents["variants"].items():
                        if not isinstance(v, list):
                            v = [v]
                        if key in extractedProperties:
                            models[filename[:-5] + ";" + key] = [v[0]["model"]]
                else:
                    multipart = contents["multipart"]
                    #sometimes the multipart model has two variants. For now we discard all but one
                    for v in multipart:
                        if isinstance(v["apply"], list):
                            v["apply"] = v["apply"][0]
                    #first we get the base model parts
                    base = [v["apply"]["model"].replace("minecraft:block/", "") for v in multipart if "when" not in v.keys()]
                    #then we get the parts that differ between blocks
                    variants = {getKey(v["when"]):[v["apply"]["model"].replace("minecraft:block/", "")] for v in multipart if "when" in v.keys()}
                    #then we add base model parts to each block variant
                    for k, v in variants.items():
                        v += base
                        models[filename[:-5] + ";" + k] = v
    #horrible indentation here. I would write this thing in C if there was much point
    return models

if __name__ == "__main__":
    path = sys.argv[1] #path to assets/minecraft
    with open("obj.obj", "w") as f:
        #ok so first we get the blockstates
        pathToBlockstates = os.path.join(path, "blockstates")
        blockstates = getBlockstates(pathToBlockstates)
        #print(blockstates)
        # ok so now we have the information what all the block variants are, and where they are stored    
        resultStr = ""
        modelFolder = os.path.join(path, "models", "block")
        for k, v in blockstates.items():
            resultStr += "o %s\n" % k
            vertexCounter = 1 #how many vertices we have
            for modelName in v:
                cubes = getModel(modelName, modelFolder)
                for c in cubes:
                    for vertex in c.vertices:
                        resultStr += "v %.6f %.6f %.6f\n" % vertex 
                        vertexCounter += 1
                    for face in c.faces:
                        resultStr += "f %d %d %d %d\n" % (vertexCounter - face[0], vertexCounter - face[1], vertexCounter - face[2], vertexCounter - face[3])
                    
        f.write(resultStr)
