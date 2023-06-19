import sys
import os
import json

#python script for generation of special object files

extractedProperties = ["direction", "half", "shape", "color", "part", "open", "snowy", "north", "west", "east", "south"]

def getKey(d:dict) -> str:
    res = []
    for k in d.keys():
        if k in extractedProperties:
            res.append("%s=%s" % (k, d[k]))
    return ",".join(res)

if __name__ == "__main__":
    path = sys.argv[1] #path to assets/minecraft
    with open("obj.obj", "w") as f:
        blockstates = os.path.join(path, "blockstates")
        models = {}
        for filename in os.listdir(blockstates):
            filePath = os.path.join(blockstates, filename)
            if filePath[-5:] == ".json":
                with open(filePath, "r") as j:
                    contents = json.loads(j.read())
                    if "variants" in contents.keys():
                        for key, v in contents["variants"].items():
                            if not isinstance(v, list):
                                v = [v]
                            if key in extractedProperties:
                                models[filename[:-5] + ";" + key] = [v[0]["model"]]
                    else:
                        multipart = contents["multipart"]
                        base = [v["apply"]["model"] for v in multipart if "when" not in v.keys()]
                        variants = {getKey(v["when"]):[v["apply"]["model"]] for v in multipart if "when" in v.keys()}
                        for k, v in variants.items():
                            v += base
                            models[filename[:-5] + ";" + k] = v
        print(models)
        # ok so now we have the information what all the block variants are, and where they are stored    


