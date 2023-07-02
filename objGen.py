import sys
import os
import json

#python script for generation of special object files

extractedProperties = ["direction", "half", "shape", "color", "part", "open", "snowy", "north", "west", "east", "south"]

def getModelFilename(rawName:str) -> str:
    """Removes weird boilerplate from model filenames"""
    return rawName.replace("minecraft:block/", "").replace("block/", "")

def getKey(d:dict) -> str:
    """Internal function that properly formats the key for further processing"""
    res = []
    for k in d.keys():
        if k in extractedProperties:
            res.append("%s=%s" % (k, d[k]))
    return ",".join(res)

class cube:
    """A wrapper for 8 vertices and 6 faces, also handles materials for faces"""
    eastTemplate = (1, 0, 2, 3)
    upTemplate = (1, 0, 4, 5)
    southTemplate = (2, 0, 4, 6)
    northTemplate = (7, 3, 1, 5)
    westTemplate = (7, 5, 4, 6)
    downTemplate = (7, 6, 2, 3)
    def __init__(self, f:list, t:list, m:dict = None, faces:dict = None) -> None:
        if f != None and t != None:
            startx = f[0] / 16
            starty = f[1] / 16
            startz = f[2] / 16
            endx = t[0] / 16
            endy = t[1] / 16
            endz = t[2] / 16
            self.vertices = ((startx, starty, startz), (endx, starty, startz), (startx, endy, startz), (startx, starty, endz), 
                        (startx, endy, endz), (endx, starty, endz), (endx, endy, startz), (endx, endy, endz))
            if faces == None:
                self.faces = (self.eastTemplate, self.upTemplate, self.southTemplate, self.northTemplate, self.westTemplate, self.downTemplate)
                #E,+y,S,N,W,-y
            else:
                self.faces = []
                for key in faces.keys():
                    match key:
                        case "east":
                            self.faces.append(self.eastTemplate)
                        case "up":
                            self.faces.append(self.upTemplate)
                        case "south":
                            self.faces.append(self.southTemplate)
                        case "north":
                            self.faces.append(self.northTemplate)
                        case "west":
                            self.faces.append(self.westTemplate)
                        case "down":
                            self.faces.append(self.downTemplate)
        self.materials = None
        if m != None:
            if "all" in m.keys():
                self.materials = getModelFilename(m["all"])
            else:
                self.materials = []
                for val in faces.values():
                    self.materials.append(getModelFilename(m[val["texture"][1:]]))
                #try:
                #    self.materials = (getModelFilename(m[faces["east"]["texture"][1:]]), getModelFilename(m[faces["up"]["texture"][1:]]), 
                #                    getModelFilename(m[faces["south"]["texture"][1:]]), getModelFilename(m[faces["north"]["texture"][1:]]),
                #                    getModelFilename(m[faces["west"]["texture"][1:]]), getModelFilename(m[faces["down"]["texture"][1:]]))
                #except KeyError:
                #    print(faces)

#recursive function for 
def getModel(name:str, modelFolder:str, parentTextures:dict=None) -> list[cube]:
    """Returns all cubes that make up a model. Returns None if the model is a simple cube"""
    if getModelFilename(name) == "cube":
        return None
    result = []
    filePath = os.path.join(modelFolder, name + ".json")
    with open(filePath, "r") as f:
        contents = json.load(f)
        if "textures" not in contents.keys():
            contents["textures"] = dict()
        if parentTextures != None:
            contents["textures"].update(parentTextures)
        if "parent" in contents.keys():
            parent = getModel(getModelFilename(contents["parent"]), modelFolder, contents["textures"])
            if parent == None:
                return parent
            result.extend(parent)
        if "elements" in contents.keys():
            for el in contents["elements"]:
                c = cube(el["from"], el["to"], contents["textures"], el["faces"])
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
                    keys = [(k, v) for k, v in contents["variants"].items()]
                    for key, v in keys:
                        if not isinstance(v, list):
                            v = [v]
                        models[filename[:-5] + ";" + key] = [getModelFilename(v[0]["model"])]
                else:
                    multipart = contents["multipart"]
                    #sometimes the multipart model has two variants. For now we discard all but one
                    for v in multipart:
                        if isinstance(v["apply"], list):
                            v["apply"] = v["apply"][0]
                    #first we get the base model parts
                    base = [getModelFilename(v["apply"]["model"]) for v in multipart if "when" not in v.keys()]
                    #then we get the parts that differ between blocks
                    variants = {getKey(v["when"]):[getModelFilename(v["apply"]["model"])] for v in multipart if "when" in v.keys()}
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
            tmpStr = "o %s\n" % k
            vertexCounter = 1 #how many vertices we have
            for modelName in v:
                cubes = getModel(modelName, modelFolder)
                if not (cubes == None and len(v) == 1):
                    for c in cubes:
                        simpleTexture = False
                        for vertex in c.vertices:
                            tmpStr += "v %.6f %.6f %.6f\n" % vertex 
                            vertexCounter += 1
                        if isinstance(c.materials, str):
                            simpleTexture = True
                            tmpStr += "usemtl %s\n" % c.materials
                        i = 0
                        for face in c.faces:
                            if not simpleTexture and c.materials != None:
                                tmpStr += "usemtl %s\n" % c.materials[i]
                            tmpStr += "f %d %d %d %d\n" % (vertexCounter - face[0], vertexCounter - face[1], vertexCounter - face[2], vertexCounter - face[3])
                            i += 1
                    resultStr += tmpStr
        f.write(resultStr)
