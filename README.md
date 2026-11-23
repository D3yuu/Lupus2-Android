# Lupus2 Android Native Client

Native Android port of the Lupus2 game client. The renderer and gameplay path
are implemented in C/C++ and run through an Android Java host with touch,
multitouch, virtual controls, text input, lifecycle handling and downloadable
content support.

## Download

Download the ARM64 Full APK from [GitHub Releases](https://github.com/D3yuu/Lupus2-Android/releases).
The APK contains the game packs. An internet connection is required to log
in to the game server.

The public APK uses the Lupus2 Android release certificate. If you previously
installed a build signed with a different certificate under the same app ID,
Android will require uninstalling that build first. Uninstalling removes its
local settings and downloaded files.

## Scope

This repository is for the Android client only. It does not contain server
files, databases, credentials or a distributable Windows/PC client.

The native Android target still uses common engine code inherited from the PC
client because that code is required to load the original game formats. Visual
Studio projects, PC executables and server sources are not part of the Android
release.

## Current target

- Android 5.0 or newer (`minSdk 21`)
- 64-bit ARM devices (`arm64-v8a`)
- OpenGL ES 3.0
- Android SDK 33
- Android NDK 28.0.13004108
- JDK 17

See [BUILDING.md](BUILDING.md) for build instructions. Editable Python scripts
used by the mobile root pack are in `game-scripts/root/`.

## Important licensing note

See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for the treatment of
middleware, engine source and game assets. The presence of public source here
does not grant a blanket license for third-party code or assets.

