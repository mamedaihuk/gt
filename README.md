# Ghost Trick: Phantom Detective RE
This is an attempt to reproduce the source code and make the assets for gt
modifiable.

## Building
### Requirements
* GNU make
* Python 3
* [ndstool](https://github.com/devkitPro/ndstool)
* zsh

Place the ROM in `./src/` as `baserom.nds`.


```sh
make
```

This will do everything; unpacking, building, repacking. If you would like to
do only just one of said instructions, see the section below.

### Main Targets
* `pre` - unpack the ROM and extract its assets
* `post` - build and repack the ROM
* `build` - build the ROM, but don't repack it
	* `sdat` repack the sound data
	* `text` repack the text data
* `clean` - remove all generated files and directories
