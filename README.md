# Pizza Connection 2 – safe GOG compatibility foundation

Work in progress based on the original pc2fix fork. Read [the technical report](docs/TECHNICAL_REPORT_DE.md) before deploying.

See [runtime validation and follow-up fixes](docs/RUNTIME_VALIDATION_DE.md) for the actual local game tests and their limits.

This branch recognises one analysed German GOG executable using full-file and loaded-code SHA-256 checks. Unknown or modified executables receive Bink forwarding only. Original simulation, frame timers, intro and focus handling are retained. **Native widescreen and verified Windows 10/11 gameplay compatibility are not implemented.**

The Bink proxy preserves the 83 exports and ordinals of the analysed original DLL. Initialisation occurs once on BinkSetSoundSystem, outside DllMain. A prepared window sizing adapter calculates an 800x600 client area using Windows APIs, tested in a hidden window. It is not installed as a hook yet: Bink initialisation cannot prove that the window thread has not started. `-windowed` is therefore currently refused and logged. This deliberately narrows the original patch to a safe foundation until a synchronised early bootstrap is available.

## Build

Visual Studio 2022 C++ Build Tools (v143) and Windows 10/11 SDK, x86 Developer Command Prompt:

```bat
msbuild pc2fix.sln /p:Configuration=Release /p:Platform=x86 /m
msbuild pc2fix.sln /p:Configuration=Debug /p:Platform=x86 /m
```

Outputs: out/Release/binkw32.dll and out/Debug/binkw32.dll. Building never writes into a game installation. Release uses static CRT. See [test instructions](tests/README.md).

## Reversible manual deployment

Use a separate copy of your installation first. Keep your original installation and saves untouched.

1. Back up the original binkw32.dll outside the test installation, recording its SHA-256.
2. Copy that original DLL as binkw32o.dll into the test game directory. Do not replace an existing binkw32o.dll unless you have independently checked it.
3. Only after backup, replace the test directory's binkw32.dll with the newly built proxy.
4. Launch and inspect pc2fix.log. This build is for validation, not a release.
5. To revert, restore the backed-up original binkw32.dll. Never rename the proxy to binkw32o.dll.

Do not upload game executables, assets, saves or original Bink DLLs to GitHub. The executable profile contains metadata only. MIT and vendor copyright notices are retained.
