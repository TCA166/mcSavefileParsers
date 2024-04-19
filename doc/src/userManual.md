# User Manual

This is a bit of a guide on how to accomplish different things using the utilities provided.
This is not a developer guide and doesn't assume you have any programming knowledge.
I will also be assuming you are using Windows, as I can reasonably assume Linux users will have no trouble adapting these instructions.

## How do I find my Minecraft savefiles

Utilities provided in this repo operate on Minecraft savefiles.
Finding these can be a challenge if you haven't done that before.
Usually they are located in the ```saves``` directory in the Minecraft folder.
By default on Windows you can find your your Minecraft folder under the following path: ```C:\Users\*your username*\AppData\Roaming\.minecraft```.
If you enter the ```saves``` subdirectory in your Minecraft folder you will see folders corresponding to your Minecraft savefiles.

## How do I get individual chunks out of my savefile

Having located a save directory of your choice enter it and go to the ```region``` subdirectory.
Inside you will find your savefile [region files](https://minecraft.fandom.com/wiki/Region_file_format).
The Minecraft chunks are grouped into regions 32x32 chunk big and you can find your chunks in these files.
Region file named ```r.0.0.mca``` will contain chunks from 0x0, 0x1, 0x2, 1x1 ... 32x32.
Now pick a region file and copy it next to where ```regionFileReader.exe``` is located.
Next open the terminal in that location and type:

```cmd
start regionFileReader.exe *your region file name* .
```

And ```regionFileReader``` should extract all chunks into this directory

## How do I get a specific chunk out of a region file

This time instead of using ```regionFileReader``` we are going to be using ```chunkExtractor```.
Get the absolute filepath of the regions folder of the save of your choice.
That would usually take the form of ```C:\Users\*your username*\AppData\Roaming\.minecraft\saves\*save name*\region```.
Then in the directory where ```chunkExtractor.exe``` is located input the following into the terminal:

```cmd
start chunkExtractor.exe *absolute filepath* 0 0
```

And ```chunkExtractor``` should extract chunk 0 0 from your savefile.

## What can I do with an extracted chunk?

Whatever you want!
It's encoded in a proprietary Mojang format 'nbt' but there are plenty of free editors and viewers of that format.
So download one, and play around with your world outside of the game.
Perhaps find some kind of a converter or something

## How can I generate a 3d model out of my world

For this we are going to be using ```radiusGenerator```.
It functions similarly to chunkExtractor but with a few more options.
Having kept the absolute path from before in the directory where ```radiusGenerator.exe``` is located input the following into the terminal:

```cmd
start radiusGenerator.exe *absolute filepath* 0 0 2 
```

This will create a rudimentary 3d model called 'out.obj' next to the executable.
It's entirely white and treats every block as solid.
If you want to have a more detailed model you will need two things:

- a material file
- a special objects file

The first is a file defining what colors different blocks have, the second a file defining the shape of blocks that aren't cubes.
I created two handy Python scripts for generating these files, but you don't need to generate the material file.
I have included one for your convenience in the repo.
You will need to install Python to run them, but they shouldn't require any non standard libraries.
When you install Python be sure to check the box that adds it to PATH.
Having installed Python, you will need to find a Minecraft resource pack.
Get the absolute path of **the /assets/minecraft** and run the following command in the repository directory:

```cmd
python src/scripts/objGen.py *path to /assets/minecraft directory of a resource pack*
```

This will generate a special obj file in the repository directory.
Now we are ready to generate the 3d model for real.

```cmd
start radiusGenerator.exe *absolute filepath of region* 0 0 2 -m out.mtl -o obj.obj
```
