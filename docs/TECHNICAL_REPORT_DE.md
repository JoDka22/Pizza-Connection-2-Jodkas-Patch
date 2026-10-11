# Technischer Bericht – Stand 11.10.2026

Dieser Bericht beschreibt die erste Bestandsaufnahme. Die nachfolgenden Bugfixes und inzwischen durchgeführten Spieltests stehen in [RUNTIME_VALIDATION_DE.md](RUNTIME_VALIDATION_DE.md); frühere Angaben zu noch nicht durchgeführten Tests gelten für den damaligen Prüfstand.

## Nachgewiesener Ausgangspunkt

Fork: https://github.com/JoDka22/pc2fix-self, Ausgangscommit `f1ad448`, Hauptbranch `master`.
Arbeitskopie: `D:\Pirat\Pizacon2 test\pc2fix-self`.
Lesbarer Installationsordner: `D:\Pirat\Pizacon2 test\pizza  connection 2\Pizza Connection 2`.
Keine Originaldatei und kein Spielstand wurde verändert. Lokale Testkopien der Bink-DLL liegen ausschließlich im ignorierten `out`-Ordner. Sie gehören niemals in Git oder ein Release.

EXE: `fastfood2.exe`, 1.568.768 Bytes, SHA-256
`18cfd32e0ac5d6d98162ec016e058a2b51808d9ca49ebee933ce3166ff9922fd`.
PE32/x86, ImageBase `0x400000`, EntryPoint `0x54fcd4`, SizeOfImage `0x18e000`, Timestamp `0x3a39d466` (15.12.2000 UTC). Relocations sind entfernt; keine ASLR-/NX-Kompatibilitätsflags im Originalheader. Vier Abschnitte: `.text`, `.rdata`, `.data`, `.rsrc`.
Versionsressource: 1.0.0.1, OriginalFilename Pizza2.exe, Software 2000, Deutsch. Die separate VER-Datei sagt 1.004; das sind verschiedene Versionsangaben, keine zuverlässige Hook-Freigabe.
GOG-Metadaten: GameId 1175067557, deutsche Sprache, BuildId 51199057373375775. Vorhandene Startflags: DWM8And16BitMitigation, DISABLETHEMES, DISABLEDWM, HIGHDPIAWARE. Andere GOG-Sprachen/Builds sind nicht freigegeben.

`local-executable-profile.json` enthält Hashes, PE-Metadaten und Importnamen, keine Originaldateien. Analysewerkzeuge: MSVC dumpbin, pefile 2024.8.26, Capstone 5.0.9. REA/Ghidra waren für diese erste Analyse nicht erforderlich.

## Bestehendes Repository

- `dllmain.cpp`: Bink-Proxy mit x86-Naked-Assembly, 66 manuell aufgelösten Funktionen und 71 Pointer-Slots. Main() lief unter Loader-Lock; ein zusätzlicher Thread füllte erst danach die Pointer. Nullsprünge waren möglich. Threadhandle wurde nicht geschlossen, Startparameter zeigte auf einen lokalen Stackwert. LoadLibrary verwendete nur einen relativen Namen.
- Die lokale GOG-Bink-DLL exportiert 83 Funktionen. Manche bisherigen Ziele fehlen dort, andere haben andere Stackgrößen: z.B. BinkSetPan@8 statt @12, SetVolume@8 statt @12, SetSoundTrack@4 statt @8. Die zehn EXE-Imports sind vorhanden. Der alte Proxy war deshalb kein vollständiger ABI-kompatibler Ersatz für diese Bink-Version.
- `main.cpp`: MinHook-Detours für Fensterprozedur, CallbackTimer, WorldClock und ProcessGameTick, absolute Adressen ohne Versionsprüfung, Intro-NOPs, Fensterkonstanten, Frame-Timer und Wartezeitänderung. Kein DirectDraw-CreateSurface/Blt/Flip-Detour vorhanden. Der README-Widescreen-Punkt war ein TODO.
- `main.h`: nur Main/Quit-Deklarationen. Quit war nicht in den DLL-Abbau integriert.
- `types.h`: unvollständige RE-Strukturen mit vielen Paddingfeldern, ohne sizeof-/offsetof-Prüfungen. DirectDraw-Interfaces werden als Mischung aus IDirectDraw und Surface7 bezeichnet; die Analyse zeigt alte Deskriptorgrößen (0x6c). Die tatsächlichen Interface-Versionen müssen vor COM-Hooks belegt werden. Diese Strukturen sind im neuen Bootstrap nicht im Einsatz.
- `pc2fix.sln`: VS2022, x86 Debug/Release auf Win32-Projektkonfigurationen abgebildet.
- `pc2fix.vcxproj`: v143, Windows SDK 10.0, MultiByte, MinHook-Quellen direkt eingebunden, vormals Ausgabe direkt in fremden Steam-Installationspfad. Keine Tests/CI. Jetzt Ausgabe in out/Debug bzw. out/Release, C++17, statische CRT und explizite Exportdefinition.
- `vendors/minhook-master`: mitgelieferte MinHook-Codebasis 1.3.4 einschließlich x86/x64-Decoder, MIT/BSD-Lizenzdateien erhalten. Die Anwendung bleibt ausdrücklich x86.
- `idapro/*.idc`: IDA-Rekonstruktionsskripte für Typen, Funktionen und Daten; keine ausführbare Original-EXE. Funktionsnamen sind RE-Hinweise, kein Beweis für Laufzeitsemantik.
- `LICENSE`, Bild, README, Projektfilter und .gitignore geprüft; Lizenzhinweise erhalten. Keine weiteren AGENTS.md-Anweisungen im geklonten Repository gefunden.

## Adressen und Risiken, nach Priorität

| Priorität | Befund | Folge / Entscheidung |
|---|---|---|
| P0 | Ungeprüfte absolute Adressen | Vollständiger Datei-SHA-256, PE-Layout, tatsächliche Prozessbasis und SHA-256 des geladenen .text vor jedem Spielzugriff. Unbekannt = reine Bink-Weiterleitung. |
| P0 | DLL-Lader-Rennen und ABI-Unterschiede | 82 statische PE-Forwarder und ein typisierter Bootstrap-Wrapper; alle 83 GOG-Exportnamen und Originalordinals erhalten. DllMain bleibt leer. |
| P0 | Spielgeschwindigkeitsänderung | Timer-/WorldClock-/Tick-Detours und Zahlentasten-Steuerung entfernt, um Originalmechaniken zu bewahren. |
| P1 | Eigene WndProc ersetzt Original | Alter Hook ignorierte Fokusverlust (immer true), übergab SetEvent die Adresse statt des Handle-Werts und hatte eine falsche Originalfunktions-Typdeklaration. Ersatz entfernt; Originalprozedur bleibt aktiv. |
| P1 | VirtualProtect über gesamten Codeabschnitt, unchecked; rdata dauerhaft schreibbar | Breite RWX-Freigabe, Intro-NOPs und Konstantschreibungen entfernt. Diese erste Änderung installiert bewusst noch keine Hooks und schreibt keine Spieldaten. MinHook bleibt als bestehende Abhängigkeit für weitere Schritte erhalten. |
| P1 | Feste Rahmenaufschläge und falsche Zentrierungsachsen | Ein vorbereiteter, derzeit nicht installierter CreateWindowExA-Adapter berechnet Rahmen über AdjustWindowRectEx und Monitorarbeitsfläche. Native Tests bestanden. Aktivierung benötigt einen belegten synchronisierten Aufruf vor Fenstererstellung. |
| P1 | Bink-Bootstrap kommt im Spiel vermutlich spät | SetSoundSystem wird im Video-Pfad bei 0x52af3a aufgerufen. Introaufrufe erfolgen nach Start des Fensterthreads. Auch ein noch leeres HWND schließt einen schon laufenden Fensterthread nicht aus. -windowed wird deshalb vorerst immer abgewiesen. Kein verlässlicher Fenstermodus behauptet. |
| P2 | 83-ms-Framecallback und 20-ms-Wartezeit wurden pauschal gekürzt | Alte Änderungen entfernt. Rendering und Simulation erst messtechnisch trennen, bevor Framerate/Ladezeit verändert werden. |
| P2 | Padding-Strukturen, Null-/Lifetime-Risiken | Kein Timerpointer-Caching mehr. Vor späterem Einsatz Layout und Calling Conventions durch Tests und Disassembly belegen. |

Disassemblierte alte Hook-Einstiege stimmen mit den IDC-Adressen überein:
WndProc `0x525270` (stdcall mit ret 0x10), Timer `0x403ac0`, WorldClock `0x4dd2d0`, Tick `0x525a10`. Das belegt die Positionen, nicht die Sicherheit der alten Hook-Implementierung.
Weitere geprüfte Stellen: Fenstergrößenoperanden `0x404824` (600) und `0x404829` (800), SetWindowResRect `0x404420`, Framecallback-Immediate `0x481713` (83), WaitForMultipleObjects-Timeout `0x404e48` (20). Fullscreen-Datenwert `0x563860` initial 1, Fensterhandle `0x57c5d0`. Die alte Variable hObject zeigte auf `0x57c0d8`, dessen Inhalt laut Originalcode das Eventhandle ist.

## Rendering und Widescreen-Architektur

Nachgewiesener Import: DDRAW.dll!DirectDrawCreate, kein statischer Direct3D-Import. GDI, WinMM und Bink ebenfalls vorhanden. Das schließt dynamische Erweiterungen nicht generell aus.

- `0x537f20`: DirectDrawCreate; exklusiver Vollbildpfad mit SetCooperativeLevel (0x13), SetDisplayMode mit 16 bpp und Dimensionen aus `0x574734/0x574738`.
- CreateSurface-Aufrufe mit 108-Byte-Deskriptoren, primärer Flip-Chain und Backbuffer-Zuordnung (`0x538080`, `0x53833f`); separater Fensterpfad ab `0x53842d` mit Clipper/Offscreen-Surfaces.
- `0x538fa0`: Lock; nach Erfolg liest das Spiel Breite/Höhe, lpSurface und Pitch zurück. Pitch wird für 16-Bit-Pixel durch zwei geteilt. Das ist ein Ansatzpunkt für variable Surface-Größen; Farbmasken müssen noch gemessen werden.
- `0x539860`: Flip mit verlorenen Oberflächen-/Restore-Behandlung. `0x539980`: rechteckbasiertes Fenster-Blitting mit Clipping und vorhandener ganzzahliger Skalierung.
- `0x404420`: feste 800x600-Rechtecke; Mauspfad `0x524990`, Frame-Renderpfad `0x485140`. Kamera/Culling und UI-Hitboxen sind noch nicht vollständig rekonstruiert.

Echtes Widescreen ist weder bewiesen unmöglich noch bereits implementiert. Nur SetDisplayMode oder Fenstergröße zu ändern reicht nicht: World-Renderfläche, Sichtbarkeitslisten, Dirty Rects, Clipping, UI und Eingabe müssen dieselbe Geometrie verwenden. BltFast bietet selbst kein Clipping oder frei skalierendes Blitting.

Vorgeschlagene Schritte auf getrennten Branches:

1. Frühen, deterministischen Initialisierungspunkt außerhalb DllMain schaffen. Ein Launcher mit kontrolliertem Einstieg vor der Fenstererstellung oder ein geeigneter früher Engine-Aufruf muss zuerst belegt werden; keine zeitabhängige Workerthread-Lösung.
2. DirectDraw-Proxy/Wrapper mit belegten Interface-Versionen: CreateSurface, SetCooperativeLevel, SetDisplayMode, Lock/Unlock, Blt/BltFast, Flip und Restore instrumentieren. Diagnose muss Surface-Identität, Format, Maße, Pitch und Fehlercodes aufzeichnen, ohne Gameplayzustand zu ändern.
3. Präsentation von Enginekoordinaten trennen. Zunächst 800x600 in einem unverzerrten 4:3-Rechteck auf 1920x1080 (1440x1080, Seitenränder je 240) und 2560x1440 (1920x1440, je 320) darstellen. Das ist ausdrücklich noch kein echtes Widescreen. Dieselbe inverse Transformation für Maus und Hitboxen, keine Klicks in Seitenrändern.
4. Weltviewport separat verbreitern. Kamera, Culling, Randscrollen, Minimap, Objektpicking und Renderloops rekonstruieren und anschließend 1920x1080/2560x1440-Worldflächen zulassen. UI mit eigenen Ankern, Clips und einheitlicher Skala überlagern. Statische 4:3-Menüs dürfen als zentrierte Panels bleiben.
5. Ein DXGI/D3D11-Präsentationsbackend als spätere Alternative zu Legacy-Exklusivmodus: CPU-generierte 16-Bit-Flächen in Texturen konvertieren, Borderless und Fenster, Surfaceverlust/Alt-Tab/DPI/Monitorwechsel behandeln. Vor Einführung zusätzliches Backend testen; nicht bereits Bestandteil dieser DLL.
6. Framerate und I/O erst nach getrennten Messungen von Präsentation, Simulation und Ladepfaden optimieren. Spielzeit, Autosave, KI und Spielstandsformat müssen unverändert bleiben.

## Erste Implementierung und Status

Branch `compatibility/gog-safe-bootstrap`. Kein Release, kein Merge in master.
Der neue Patch ist eine abgesicherte Basis, kein fertiger Windows-10/11- oder Widescreen-Patch. Default-Vollbild und Simulation bleiben Originalverhalten. Logging schreibt pc2fix.log neben die gestartete EXE; ohne Schreibrecht bleibt OutputDebugString verfügbar.
Die SHA-Prüfung erlaubt exakt die untersuchte deutsche EXE. Weitere Profile werden erst nach eigener Analyse aufgenommen. Andere Patches in .text werden absichtlich abgewiesen.
BinkSetSoundSystem initialisiert über InitOnce. Bei fehlendem originalen Export wird Audioinitialisierung kontrolliert abgewiesen. Da Bink keine sichere Synchronisation vor der Fenstererstellung bietet, wird -windowed bewusst nur mit einer Diagnose beantwortet; keine Hooks und keine Spieldatenänderungen. Das ist eine vorläufige Funktionseinschränkung gegenüber dem alten unsicheren Patch. Verwendung als normale Prozessabhängigkeit.

Für die Weiterleitungen muss eine unveränderte lokale Bink-DLL als binkw32o.dll bereitliegen. Fehlende Original-DLL/Exports führen beim Laden zu einem Windows-Laderfehler, nicht zu Nullpointer-Trampolinen. Dieser Proxy ist auf die Export-ABI der hier untersuchten GOG-Bink-Version zugeschnitten.

## Validierung und verbleibende manuelle Tests

Bestanden: MSVC/v143 Release und Debug Win32, ohne Compilerwarnungen in der beobachteten Ausgabe. Nativer Test mit /W4 /WX: SHA-256-Vektor und Fehlhash, fremder Host, lesend akzeptiertes GOG-Dateiprofil, akzeptiertes nicht ausgeführtes Image, Zurückweisung einer privat im Speicher veränderten Codekopie, Argumentparser, tatsächliche 800x600-Clientgröße eines versteckten Fensters und unveränderte fremde Fenstergröße. Private Mappingänderung schreibt niemals die Datei zurück.
Exportprüfung: exakt 83 Namen und Originalordinals, 82 Forwarder plus ein Wrapper, x86. Lokaler Smoke-Test mit Original-Binkkopie: BinkGetError-Weiterleitung und wiederholte SetSoundSystem-Aufrufe in einem fremden Testhost ohne Absturz; Log belegt einmalige Initialisierung und Versionsablehnung.

Nicht durchgeführt: spielbarer GOG-Lauf, Windows-10/11-Matrix, Alt-Tab, lange Spielsessions, Spielstände, echte 1080p/1440p-Ausgabe, Kameraerweiterung, Performancebenchmarks. Erfolgreiche Kompilierung und Mappingtests sind keine Gameplaytests.

Manuelle Prüfliste in einer separaten Installationskopie mit gesicherten Saves:

- Originalstart als Referenz protokollieren; dann Proxy einsetzen und Intro/Audio/Hauptmenü starten.
- Log muss Profilfreigabe oder begründete Ablehnung zeigen. -windowed auch zusammen mit anderen Argumenten prüfen; 'windowed activation deferred' bedeutet keine aktivierte Fensterfunktion.
- Falls früher Bootstrap funktioniert: Menü und Karte vollständig, 800x600 Client, Maus an allen Ecken, keine abgeschnittenen Panels.
- Vollbild/Fenster, Alt-Tab, Minimieren/Wiederherstellen, Monitorwechsel und 100/125/150/200 Prozent DPI separat testen.
- Alte Saves laden, weiterspielen, unter neuem Namen speichern und im Original wieder laden. Keine bestehenden Saves überschreiben.
- Zeit-/KI-/Wirtschaftsverlauf vergleichen, Input im Fokusverlust und Sound/Videos prüfen; keine beschleunigte Simulation.
- Spätere Präsentations-/Widescreenänderungen auf 1920x1080 und 2560x1440 anhand tatsächlicher Screenshots, Maustreffer und sichtbarer Kartenfläche prüfen.

Referenzen: [Microsoft DLL Best Practices](https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-best-practices), [DirectDraw BltFast](https://learn.microsoft.com/en-us/windows/win32/api/ddraw/nf-ddraw-idirectdrawsurface7-bltfast).
