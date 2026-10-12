# Pizza Connection 2 Jodkas Patch v1.0

## Deutsch

Release-Kandidat fuer die deutsche GOG-Version 1.006 auf Windows 10/11.
Die Versionspruefung verlangt den bekannten EXE- und geladenen Code-Hash;
unbekannte Versionen erhalten keine festen Speicherpatches. Steam ist nicht getestet.

- Natives 1920x1080 mit erweitertem Stadt-Viewport (1716x1031), unverzerrter UI
  und zentrierter Restaurant-Ansicht mit passender Mausabbildung.
- Kamera-Grenzen, leere Culling-Zellen und veraltete Restaurant-Randpixel korrigiert.
- Komposition im separaten Ausgabepuffer, synchronisierte Bild-/Eingabemetadaten,
  korrigierte COM-Oberflaechen-Lebensdauer und wiederverwendete RGB565-Konvertierung.
- `TargetFPS=60` als Standard; `120` oder `0` ohne Limit. Gueltig sind 0 und
  1..1000; ungueltige Werte deaktivieren den Limiter mit Loghinweis. Die Datei
  wird beim Spielstart eingelesen. Die Engine kann weiterhin deutlich weniger
  FPS liefern. Der Limiter greift im gepatchten Presenter einschliesslich Menues.

Der Limiter verwendet QPC und einen Waitable Timer (hohe Aufloesung, sofern
verfuegbar; sonst normaler Timer oder Sleep). Er verwendet keine Warteschleife,
keine globale Timer-Aufloesung und ersetzt keine Simulations-Zeitgeber.
Verspaetete Frames starten ein neues Intervall; es gibt keine Aufhol-Bursts.
Minimierte Fenster setzen den Termin zurueck. Die optionalen Performance-
Messwerte messen die Ausgabe nach dem Warten und sind keine GPU-Leistungsmessung.

## English

Release candidate for German GOG 1.006, Windows 10/11. Unknown executable
versions are forwarded without fixed-address patches; Steam is unverified.
Native 1080p city viewport, centered restaurants, matching input, camera/culling
corrections and dirty-background restoration are combined with buffered output
and configurable pacing. `TargetFPS=60`, `120`, or `0` (uncapped) is read at startup.
This is an output ceiling, not a frame-rate boost or simulation timer replacement.

## Validation / Pruefungen

Release/Debug x86 builds and all 172 checks passed locally without compiler or
MSBuild warnings. Runtime timing in the backed-up GOG copy with `TargetFPS=10`
reported 50 completed frames per five-second window and 100.3-100.4 ms average
frame gaps. Output work took about 2.4 ms; limiter waiting is excluded from that
figure. This confirms the output ceiling, not a GPU power measurement.
At `TargetFPS=60`, the menu retained approximately 40 ms frame gaps and the
loaded city approximately 80 ms: the cap does not force the engine to run at 60 FPS.
Loading, city/restaurant transition and clean restaurant margins were observed
with the standard 60 FPS cap. Startup and menu output with 120 FPS in the framed
window also succeeded, retaining the engine's approximately 25 FPS menu rate.
`TargetFPS=0` startup and framed menu output succeeded with the limiter disabled.
These observations do not demonstrate actual 60/120 FPS gameplay; the engine
remains the limiting factor. Previous restaurant placement/window tests are
documented in [restaurant sprite bounds](RESTAURANT_SPRITE_BOUNDS_DE.md).
The automated suite includes real Windows timer waits at 60 FPS, deterministic
deadline cases at 60/120/0, invalid settings, reset and late-frame behavior;
all existing pixel, input, lifetime and unknown-host checks remain included.

Known limits: busy restaurants with concurrent guests, large/partly visible room
backgrounds, further scene-transition polish and other game versions require more
manual coverage. No guarantee of universal glitch removal, increased simulation
FPS, reduced GPU power or elimination of coil whine is made.

## Package / Paket

Exactly one uploaded asset: `Pizza _Connection 2_Jodkas_Patch_v1.0.zip`.
It contains the patch DLL, INI, documentation, licenses and an internal SHA-256
manifest. No original EXE, game assets, saves or original/optional Bink DLL.
GitHub's automatically generated source downloads are not additional uploaded assets.
