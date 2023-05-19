# mcSavefileParsers

Multiple programs for extracting data from MC save files written in C for speed.

## Compilation

In order to compile run compile.sh on linux or WSL or if you wish to cross-compile for Windows run winCompile.sh.  

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

## License

[![CCimg](https://i.creativecommons.org/l/by/4.0/88x31.png)](http://creativecommons.org/licenses/by/4.0/)  
This work is licensed under a [Creative Commons Attribution 4.0 International License](http://creativecommons.org/licenses/by/4.0/).  
