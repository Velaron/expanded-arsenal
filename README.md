# Expanded Arsenal [![Build Status](https://github.com/Velaron/expanded-arsenal/actions/workflows/build.yml/badge.svg)](https://github.com/Velaron/expanded-arsenal/actions) <img align="right" width="128" height="128" src="https://github.com/Velaron/expanded-arsenal/raw/main/android/app/src/main/ic_launcher-playstore.png" alt="Expanded Arsenal" />
Expanded Arsenal android port.

## Donate
[![Buy Me A Coffee](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://www.buymeacoffee.com/velaron)

[Support](https://www.buymeacoffee.com/velaron) me on Buy Me A Coffee, if you like my work and would like to support further development goals, like porting other great mods.

## Download
You can download a build at the [Releases](https://github.com/Velaron/expanded-arsenal/releases/tag/continuous) section, or use these links:
* [Android](https://github.com/Velaron/expanded-arsenal/releases/download/continuous/expanded-arsenal.apk)
* [Linux](https://github.com/Velaron/expanded-arsenal/releases/download/continuous/expanded-arsenal_linux.tar.gz) (not recommended, just use the Steam version)
* [Windows](https://github.com/Velaron/expanded-arsenal/releases/download/continuous/expanded-arsenal_win32.zip) (not recommended, same as above)

## Installation
To run Expanded Arsenal you need the latest developer build of Xash3D FWGS, which you can get [here](https://github.com/FWGS/xash3d-fwgs/releases/tag/continuous).
You have to download [the mod from Mod DB](https://www.moddb.com/mods/half-life-expanded-arsenal) and copy `hl_expanded_arsenal` folder into your Xash3D FWGS directory.
After that, just install the APK and run.

## Building
Clone the source code:
```
git clone https://github.com/Velaron/expanded-arsenal --recursive
```
### Windows
```
cmake -A Win32 -S . -B build
cmake --build build --config Release
```
### Linux
```
cmake -S . -B build
cmake --build build --config Release
```
### Android
```
cd android
./gradlew assembleRelease
```
