# Pizza Connection 2 Jodkas Patch

**Pizza Connection 2 Widescreen Fix · Pizza Connection 2 1080p Patch · GOG Fix**

## Deutsch

Ein Fork von [mallgrab/pc2fix](https://github.com/mallgrab/pc2fix) für die deutsche
GOG-Version **1.006** auf **Windows 10/11**. Für Suchanfragen auch als
*Pizza Tycoon 2 Resolution Patch* auffindbar; andere Spielversionen sind nicht verifiziert.

**Features:** Natives 1920×1080 Widescreen mit mehr Stadtfläche, zentrierte
Restaurant-Ansicht und Double-Buffering gegen Flackern. Behebt Fehler in der
Kamera-Mathematik und den Renderpuffern der **25 Jahre alten Engine**: ungültige
Kamera-/Raumgrenzen, Sprite-Ghosting und Restpixel am Bildrand im erweiterten Viewport.
Ein konfigurierbarer FPS-Limiter begrenzt unnötige Bildausgabe; keine Gameplay-Zeitgeber werden ersetzt.

**Installation:** Vorhandene Wrapper, Konfiguration und Spielstände sichern.
`ddraw.dll` und `pc2fix.ini` aus
[Pizza _Connection 2_Jodkas_Patch_v1.0.zip](https://github.com/JoDka22/Pizza-Connection-2-Jodkas-Patch/releases/download/v1.0/Pizza._Connection.2_Jodkas_Patch_v1.0.zip)
neben `fastfood2.exe` entpacken. Die originale Bink-DLL bleibt erhalten.

**Einstellungen** unter `[display]` in `pc2fix.ini`: `TargetFPS=60` (Standard),
`120` oder `0` (ohne Limit); `Windowed=1` für Fenster, `0` für randloses Vollbild.
Das FPS-Limit erhöht keine enginebedingt niedrigere Framerate und garantiert keine Beseitigung von Spulenfiepen.

**Deinstallation:** Hinzugefügte `ddraw.dll` und `pc2fix.ini` löschen;
zuvor gesicherte Wrapper bei Bedarf wiederherstellen.

## English

A fork of [mallgrab/pc2fix](https://github.com/mallgrab/pc2fix) for the **German GOG
version 1.006**, **Windows 10/11**. Also discoverable as *Pizza Tycoon 2 Resolution
Patch*; other game versions have not been verified.

**Features:** Native 1920×1080 widescreen with more city area, centered restaurants
and double-buffered presentation to prevent flicker. Fixes camera math and render
buffer errors in the **25-year-old engine**, including invalid room/camera bounds,
sprite ghosting and edge residue exposed by the expanded viewport. Configurable
FPS limiting reduces unnecessary frame output without replacing gameplay timers.

**Install:** Back up existing wrappers, configuration and saves. Extract
`ddraw.dll` and `pc2fix.ini` from the
[v1.0 ZIP](https://github.com/JoDka22/Pizza-Connection-2-Jodkas-Patch/releases/download/v1.0/Pizza._Connection.2_Jodkas_Patch_v1.0.zip)
beside `fastfood2.exe`. Keep the original Bink DLL.

**Configure:** `[display]` → `TargetFPS=60` (default), `120`, or `0` (uncapped).
`Windowed=1` selects a window; `0` selects borderless fullscreen. The limiter does
not raise the engine's frame rate or guarantee elimination of coil whine.

**Uninstall:** Delete the added `ddraw.dll` and `pc2fix.ini`; restore any backed-up wrappers.

[Validation and known limitations / Tests und Grenzen](docs/RELEASE_V1.0.md) ·
[Technical documentation / Technik](docs/TECHNICAL_REPORT_DE.md) ·
[Tests](tests/README.md). Build: `tools\ci-windows.cmd` (VS 2022, v143, x86).

Patch: [MIT](LICENSE); [MinHook license](vendors/minhook-master/LICENSE.txt).
Original game files and copyright belong to their respective owners and are not distributed.
