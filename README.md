# Math2Music
How to compile
```sh
cd /path_to_src_files
make
```
How to execute
```sh
cd /path_to_src_files
./math_n_music
```

## Convert notes to .wav file
When executing you have two input options 
* Standard input
* File input

Then you have the option to filter one note of the song.

## Notes format
Only one valid format: no hyphen and space to represent silence. Example:
```sh
CDC CAa
```

## Play song
The non-filtered song can be played by python using playsound library but the one with the filter doesn't sound good with it.

## Filtered song
The filtered song was tested using Parole Media Player. Notes with similar frequencies to the one selected may sound louder than the others.
