# Ghost Trick: Phantom Detective RE
<a href="https://kiwiirc.com/client/irc.gbatemp.net/?nick=user%7C?#gt" alt="GBATemp #gt"><img src="https://img.shields.io/badge/irc.GBATemp.net-%23gt-brightgreen.svg"></a>

This is an attempt to reproduce the source code and make the assets for Ghost
Trick modifiable.

## Building
### Requirements
* [Clang](https://clang.llvm.org)
* [DASH](http://gondor.apana.org.au/~herbert/dash)
* [GNU Make](https://www.gnu.org/software/make)
* [Python](https://www.python.org)
* [lzss](https://github.com/fmlatghor/lzss)
* [ndstool](https://github.com/devkitPro/ndstool)

Copy the base ROM to the root of the project as `baserom.nds`.
Run `./mk`.

This will do everything; unpacking, building, repacking. If you would like to
do only just one of said instructions, see the section below.

### Targets
* `pre` - unpack the ROM and extract its assets
* `post` - build and repack the ROM
	* `sdat` - repack the sound data
	* `src` - repack the binaries
	* `text` - repack the text data
* `clean` - remove all generated files and directories

## Contact
Issues, patches, or anything else may be sent to `leocoogan@tutanota.com` (me!).
Feel free to stop by on the GBATemp IRC chan, `irc.gbatemp.net` `#gt`.
