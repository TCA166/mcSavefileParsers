# mcSavefileParsers

Multiple programs for Minecraft [savefile data extraction](#chunkextractor) and [3D model generation](#radiusgenerator) based on savefiles.  

## Getting started

In order to get started either download the parts of the precompiled release that apply to your system or compile the source code yourself.
For that compile.sh script was created and for crosscompilation for Windows winCompile.sh was created.

## regionFileReader

This program extracts all the chunks in the given region file into nbt files that will be created in the provided directory.

```Bash
regionFileReader <path to region file> <output directory>
```

## chunkExtractor

This program extracts only a single chunk with the given chunk coordinates into an nbt file.

```Bash
chunkExtractor <path to region directory> <x> <z>
```

## modelGenerator

An open customizable chunk to 3d model converter.
Takes in a chunk nbt file, a material file defining the look of blocks, an object file for non cube blocks and creates a 3d model based on that.
Ideally the nbt file will be extracted using one of the tools above.

```Bash
modelGenerator <path to nbt file> ...
```

The program accepts the following additional arguments:

- -l $y+ $y- :limits the result model to the given vertical range
- -f :disables face culling
- -h :displays help
- -s $s :changes the block side in the result side to the given s argument
- -m $filename :sets the given filename as the source mtl file
- -o $filename :sets the given filename as the source special objects file
- -out $filename :sets the given filename as the output filename

### Mtl format

The generator if provided with the -m flag followed by the mtl file filepath will use the provided file as a material source and generate the obj file to support mtl materials. Blocks of minecraft:dirt will use a mtl material called dirt and so on. Feel free to create your own mtl file or use the mtl Gen to create one quickly.

### Obj format

So in order to handle non cube blocks the generator needs an obj file defining models for those "special" blocks.
If that file isn't provided the generator will simply assume everything is a cube.
In order for the special obj file to get interpreted properly the vertex coordinates in each object must be relative to the center of the object.

## radiusGenerator

A multiprocessed version of modelGenerator that can generate models same as modelGenerator, but with multiple chunks at the same time.
This is the program you are going to want to use to generate your model.
You provide coordinates of a chunk that will act as a center for your model, a radius in which surrounding chunks will be added to the model and the radiusGenerator will create a large model for you.
The argument interface is very similar to modelGenerator and all rules about assets and limitations from modelGenerator apply here.
That being said here is how you use radiusGenerator:

```Bash
radiusGenerator <path to region directory> <x> <z> <radius> ...
```

## Asset extractors

Model generator needs to have minecraft assets provided to it to function to it's fullest potential.
Naturally those aren't provided here since I want these programs to be Minecraft version independent.
As such I have developed scripts in python for generating files in correct formats for modelGenerator

### mtlGen

A script for generating complete and valid mtl files for modelGenerator

```Bash
python3 mtlGen.py <path to /assets/minecraft/textures/block directory of a resource pack> ...
```

The script accepts the following additional arguments:

- -s :Generates a simplified mtl file that doesn't use textures, but solid colors generated by averaging out RGBA values of all pixels
- -t :Causes all textures with _top in name to be generated without it
- -c :Enables green color correction to correct for the fact most vegetation textures are stored in greyscale

Distribute the result at your own risk.

### objGen

A script for generating a complete obj file containing special object definition for modelGenerator

```Bash
python3 objGen.py <path to /assets/minecraft directory of a resource pack>
```

Distribute the result at your own risk.

### quickGen

Batch script combining objGen.py and mtlGen.py in case you don't want to figure out how to use the two scripts

```Bash
./quickGen.sh <path to /assets/minecraft directory of a resource pack>
```

### Minecraft default assets

You may generate mtl and obj files based on resource packs, or Minecraft default assets.
In order to find them, extract your minecraft version jar file as if it is a file archive.
Inside you will find default Minecraft textures and models.

## cNBT

For parsing of the nbt files I have chosen to use the [cNBT](https://github.com/chmod222/cNBT/tree/master) library. These files are located in the appropriately named folder and need to be compiled using the script next to them to make the modelGenerator compile properly.

## API

Most of the code has been organized into four "libraries".
regionParser handles nbt extraction from region files, chunkParser extracts data from chunk nbts, hTable is an implementation of a hash table, model handles generating 3d models in the mtl format and finally generator ties together chunkParser and model.
Feel free to use these as libraries in your projects just make sure to read the license before you do so.
The documentation for functions in these libraries should be mainly in header files, and I will gladly expand it should there be a need so just let me know.

## License

[![CCimg](https://i.creativecommons.org/l/by/4.0/88x31.png)](http://creativecommons.org/licenses/by/4.0/)  
This work is licensed under a [Creative Commons Attribution 4.0 International License](http://creativecommons.org/licenses/by/4.0/).  
