# Building the Android client

## Requirements

Install JDK 17, Android SDK 33, Android Build Tools, CMake and Android NDK
`28.0.13004108`. Set `ANDROID_HOME` or create an untracked
`android/local.properties` containing your local SDK path.

The repository uses Gradle 8.13 through the included wrapper.
The current build has only been verified on Windows. Some legacy C++ include
paths depend on a case-insensitive filesystem.
For deeply nested checkouts on Windows, use a short, dedicated CMake output
path such as `-Plupus2NativeBuildDirectory=C:/l2-cxx` if Ninja reports a path
length or directory creation error.

## Lite APK

The lite package contains the native client and bootstrap assets. A clean
installation downloads the remaining game packs from a configured endpoint:

```powershell
cd android
.\gradlew.bat :app:assembleDebug `
  -Plupus2Lite=true `
  -Plupus2ContentRoot=https://example.com/android/v1/
```

The endpoint must end in `/` and expose `manifest.txt` and its listed files.
The client verifies downloaded files against the SHA-256 values in the manifest.
Release builds require HTTPS for any configured content endpoint. Developers
can set `lupus2.debugContentRoot` in their untracked `android/local.properties`
for a debug-only endpoint.

## Full APK

The Full APK on GitHub Releases carries the game assets. Restore them locally
from the matching APK before building Full. From the checkout root on Windows:

```powershell
$apk = (Resolve-Path 'C:\Downloads\Lupus2-Android-Full-1.0.0-arm64.apk').Path
Push-Location android/app/src/main
try {
    jar xf $apk assets/
} finally {
    Pop-Location
}
```

Use an empty `android/app/src/main/assets` directory or a fresh checkout to
avoid mixing versions. The assets directory is ignored by Git. From the
`android` directory build without the Lite flag:

```powershell
.\gradlew.bat :app:assembleDebug
```

## Signed Full release APK

Keep the signing keystore outside the repository and provide credentials only
through environment variables:

```powershell
$env:LUPUS2_KEYSTORE='D:\secure\lupus2-release.jks'
$env:LUPUS2_KEYSTORE_PASSWORD='...'
$env:LUPUS2_KEY_ALIAS='lupus2'
$env:LUPUS2_KEY_PASSWORD='...'

.\gradlew.bat :app:assembleRelease `
  -Plupus2VersionCode=1 `
  '-Plupus2VersionName=1.0.0'
```

For a signed Lite build, also pass `-Plupus2Lite=true` and set
`-Plupus2ContentRoot` to the HTTPS URL of the content directory.

Never commit the keystore or its passwords. Back up the release key offline;
future updates must be signed with the same key.

A release build with no signing variables produces an unsigned APK for local
verification. If you choose a signing key different from the one used by the
current test APK, Android will not accept it as an in-place update under the
same application ID.

`game-scripts/root/` contains the editable Python sources. The native build
consumes the packed root override. Edits in the script directory must be
repacked into a compatible `root.data` and `root.index` before they affect the APK.

