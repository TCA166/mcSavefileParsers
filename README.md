# mcSavefileParsers

Multiple programs for extracting data from MC save files written in C for speed.

## Compilation

In order to compile first run cNBTcompile.sh and then run compile.sh or if you wish to cross-compile for Windows run winCompile.sh.  

## cNBT

For parsing of the nbt files I have chosen to use the [cNBT](https://github.com/chmod222/cNBT/tree/master) library. These files are located in the appropriately named folder and need to be compiled using the script next to them to make the modelGenerator compile properly.

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

This program generates a waveform(.obj) 3d model file from an nbt file containing data about a specific chunk. Ideally this nbt file will be extracted using one of the tools above.

```Bash
modelGenerator <path to nbt file> <args>
```

The program accepts the following additional arguments:

- -l $y+ $y- :limits the result model to the given vertical range
- -f :disables face culling
- -h :displays help
- -s $s :changes the block side in the result side to the given s argument

## API

Most of the code has been organized into three "libraries". regionParser which handles nbt extraction from region files, chunkParser which extracts data from chunk nbts and finally model which handles generating models from chunk data.

## License

[![CCimg](https://i.creativecommons.org/l/by/4.0/88x31.png)](http://creativecommons.org/licenses/by/4.0/)  
This work is licensed under a [Creative Commons Attribution 4.0 International License](http://creativecommons.org/licenses/by/4.0/).  
