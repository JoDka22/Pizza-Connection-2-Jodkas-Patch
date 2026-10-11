# Pizza Connection 2 – safe GOG compatibility foundation

**WIP** — Work in progress based on the original pc2fix fork. Read [the technical report](docs/TECHNICAL_REPORT_DE.md) before deploying.

See [runtime validation and follow-up fixes](docs/RUNTIME_VALIDATION_DE.md) for the actual local game tests and their limits.

The optional [native city widescreen prototype](docs/WIDESCREEN_DE.md) creates a
1920x1080 DirectDraw primary/backbuffer and a 1716x1031 city viewport. Camera bases,
root bounds, world buffers and city UI geometry are integrated. Original UI pixels
retain their size; the sidebar moves right and the status bar moves down. The feature
is disabled by default and intended for a separate test installation.

The optional [Full-HD presenter](docs/PRESENTATION_DE.md) adds a resizable framed
window or borderless fullscreen without changing the desktop display mode. City
rendering stays 1920x1080; other views retain their authored 800x600 layout and are
enlarged proportionally, centred with black bars. Mouse coordinates use the inverse
output transform. This is opt-in and restricted to 1920x1080.

The [camera bounds follow-up](docs/CAMERA_BOUNDS_DE.md) anchors oversized views
inside the original renderer's safe range and records remaining map-edge limits.

This branch recognises one analysed German GOG executable using full-file and loaded-code SHA-256 checks. Unknown or modified executables receive forwarding only. No simulation or frame timer memory patches are applied. Local gameplay observations are documented separately from compile/tests; complete UI, performance and Windows 10/11 compatibility across drivers remain unverified.

The Bink proxy preserves the 83 exports and ordinals of the analysed original DLL. Initialisation occurs once on BinkSetSoundSystem, outside DllMain. A prepared window sizing adapter calculates an 800x600 client area using Windows APIs, tested in a hidden window. It is not installed as a hook yet: Bink initialisation cannot prove that the window thread has not started. `-windowed` is therefore currently refused and logged. This deliberately narrows the original patch to a safe foundation until a synchronised early bootstrap is available.

## Build

Visual Studio 2022 C++ Build Tools (v143) and Windows 10/11 SDK, x86 Developer Command Prompt:

```bat
msbuild pc2fix.sln /p:Configuration=Release /p:Platform=x86 /m
msbuild pc2fix.sln /p:Configuration=Debug /p:Platform=x86 /m
```

Outputs: `out/Release/` and `out/Debug/`, each containing `binkw32.dll` and the optional
`ddraw.dll`. Building never writes into a game installation. Release uses static CRT.
See [test instructions](tests/README.md).

## Reversible manual deployment

Use a separate copy of your installation first. Keep your original installation and saves untouched.

1. Back up the original binkw32.dll outside the test installation, recording its SHA-256.
2. Copy that original DLL as binkw32o.dll into the test game directory. Do not replace an existing binkw32o.dll unless you have independently checked it.
3. Only after backup, replace the test directory's binkw32.dll with the newly built proxy.
4. Launch and inspect pc2fix.log. This build is for validation, not a release.
5. To revert, restore the backed-up original binkw32.dll. Never rename the proxy to binkw32o.dll.

Do not upload game executables, assets, saves or original Bink DLLs to GitHub. The executable profile contains metadata only. MIT and vendor copyright notices are retained.

## GitHub validation

See [CI and branch protection](docs/GITHUB_MAINTENANCE_DE.md). The integrated review
contains the complete patch stack; known gameplay limits remain documented above.

## Future work

Complete remaining widescreen/manual checks. Multiplayer remains an upstream idea
and has not been implemented or tested.
