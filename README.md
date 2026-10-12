# Pizza Connection 2 – GOG Full-HD Patch

Dies ist ein unabhängiger **Fork von [mallgrab/pc2fix](https://github.com/mallgrab/pc2fix)**,
weiterentwickelt in [JoDka22/pc2fix-self](https://github.com/JoDka22/pc2fix-self).
Danke an mallgrab für die ursprüngliche Grundlage. Der Patch ist kein offizielles
Update von GOG oder den Rechteinhabern des Spiels; eine eigene Spielinstallation
wird benötigt. Lizenz- und Copyright-Hinweise bleiben erhalten.

## Erster Release: v0.1.0

[Downloads und Versionshinweise](https://github.com/JoDka22/pc2fix-self/releases).
Der erste öffentliche Vorabrelease richtet sich an die **deutsche GOG-Version
1.006** und **1920×1080**. Andere EXE-Versionen werden vor Speicherpatches
abgewiesen. Windows 10/11 sind das Entwicklungsziel; die bisherigen Spieltests
erfolgten auf dem lokalen Windows-PC, nicht auf allen Treibern und Systemen.

## Was dieser Fork ergänzt

- Echte Stadt-Renderfläche von 1920×1080 mit **1716×1031 Stadt-Viewport**:
  mehr sichtbare Stadt statt gestrecktem 4:3-Bild.
- Rechte Sidebar mit 204 Pixeln und untere Statusleiste mit 49 Pixeln;
  ursprüngliche UI-Proportionen bleiben erhalten.
- Restaurant bei passenden Hintergrundgrößen mittig und proportional vergrößert,
  einschließlich angepasster Mauskoordinaten für Möbelplatzierung.
- Randloses Full-HD und veränderbarer Fenstermodus ohne Desktop-Moduswechsel.
- Korrekturen für Möbelvorschau beim Ansichtswechsel, leere Rasterzellen,
  Kameragrenzen und Wiederherstellung minimierter Fenster.
- Fertige Bilder werden in einem wiederverwendeten Puffer zusammengesetzt;
  sichtbare Zwischenausgaben beim Vergrößern werden vermieden.
- Prüfung der Original-EXE und geladenen Codebytes per SHA-256 vor den Hooks.
  Gameplay, Geld, Inventar und Spielstandformat werden nicht absichtlich geändert.

Ungeprüfte Timing-/Framerate-Hacks aus dem ursprünglichen Ansatz sind nicht aktiv.
Es gibt keine zugesagte FPS- oder Ladezeitverbesserung und keinen Multiplayer.

## Entwicklung nach v0.1.0

Die Entwicklungsbranches ergänzen schnellere RGB565-Bildaufbereitung und eine
standardmäßig ausgeschaltete Bildphasen-Diagnose (`PerformanceProbe=1`).
Diese Änderungen sind noch nicht im oben verlinkten v0.1.0 enthalten.
Messwerte, Prüfungen und Grenzen stehen in
[RGB565-Performance](docs/RGB565_PERFORMANCE_DE.md) und
[CPU/GPU-Diagnose](docs/FRAME_SPIKES_DE.md).

Steam ist eine weitere Zielversion. Die Freigabe hängt vom geprüften EXE-Profil
ab, nicht vom Store: Eine byte-identische EXE kann dasselbe Profil nutzen;
abweichende Versionen brauchen eine eigene Adressanalyse. Die Steam-Version ist
noch nicht getestet. Unbekannte EXEs erhalten weiterhin keine Speicherpatches.

## Installation

1. Spiel beenden. Zuerst eine **separate Kopie der Installation und Sicherung der
   Spielstände** anlegen. Vorhandene `ddraw.dll` und `pc2fix.ini` sichern; diese
   können zu einem anderen Grafikwrapper gehören und dürfen nicht blind ersetzt werden.
2. Das Release-ZIP herunterladen und entpacken. `ddraw.dll` und `pc2fix.ini` aus
   dem ZIP neben `fastfood2.exe` in der Spielkopie ablegen.
3. Spiel starten. Das mitgelieferte Profil aktiviert Full-HD und randloses Vollbild.
   Für ein normales Fenster in `pc2fix.ini` **`Windowed=1`** setzen.
4. Bei Problemen `pc2fix.log` im Spielordner ansehen. EXE-Version, Windows-Version,
   Modus und Schritte zum Nachstellen bei einer Fehlermeldung angeben.

```ini
[display]
Widescreen=1
Width=1920
Height=1080
Presentation=1
Windowed=0
RenderProbe=0
```

`Windowed=0` bedeutet mit diesem Profil **randloses Vollbild**, nicht exklusives
DirectDraw-Vollbild. Änderungen gelten nach einem Neustart. Der alte Parameter
`-windowed` aktiviert diesen Presenter nicht. Ohne Konfigurationsdatei bleiben
die optionalen Darstellungspatches deaktiviert. Andere Widescreen-Auflösungen
als 1920×1080 werden in diesem Release abgewiesen.

Die optionale `optional/binkw32.dll` ist ein separater Bink-Proxy für Diagnose
und sichere Weiterleitung; **für den Full-HD-Patch ist sie nicht erforderlich**.
Wer sie verwenden möchte, muss zuerst die Original-Bink-DLL sichern und eine
unveränderte Kopie als `binkw32o.dll` neben der EXE ablegen. Erst danach den Proxy
als `binkw32.dll` installieren. Nie den Proxy als Original umbenennen.

Zum Entfernen die hinzugefügte `ddraw.dll` und `pc2fix.ini` entfernen und zuvor
vorhandene Dateien aus der Sicherung zurückspielen. Bei installiertem Bink-Proxy
die originale `binkw32.dll` wiederherstellen. Die EXE wird nicht auf der Platte gepatcht.

## Geprüfter Umfang und bekannte Grenzen

Builds für Release und Debug x86 sowie automatische Geometrie-, Eingabe-,
GDI- und Sicherheitsprüfungen laufen lokal und in GitHub Actions. Spieltests in
einer separaten GOG-Kopie bestätigten Stadt/Restaurant, Möbelplatzierung,
Sidebar, Rezept-/Statistikwechsel sowie Fensterwiederherstellung.

- Menüs, Rezept- und Statistikansichten behalten ihr ursprüngliches 800×600-Layout
  und werden proportional mit schwarzen Rändern dargestellt.
- Restaurantvergrößerung nutzt vorhandene Grafik, keine zusätzlichen Raumdetails.
  Große oder teilweise sichtbare Hintergründe behalten die native Ansicht.
- Beim Ansichtswechsel kann die Engine kurz eine leere Weltfläche liefern.
- Kartenränder bei weitem Zoom, weitere Raumvarianten, alle Möbelrotationen,
  volle Restaurants und lange Sitzungen benötigen weitere Spieltests.
- Vollständige Flackerfreiheit und Kompatibilität mit jedem Treiber sind nicht nachgewiesen.
- Andere Grafikwrapper/Mods, andere Sprachversionen und 2560×1440 sind nicht unterstützt.

Erkannte EXE: `fastfood2.exe`, 1.568.768 Bytes, SHA-256
`18cfd32e0ac5d6d98162ec016e058a2b51808d9ca49ebee933ce3166ff9922fd`.
Bei unbekannten Hosts leitet der Display-Proxy an System-DirectDraw weiter;
die Versionsprüfung wird nicht durch einen Konfigurationsschalter umgangen.

## Entwicklung und technische Dokumentation

Visual Studio 2022 C++ Build Tools (v143), Windows SDK und x86:

```bat
tools\ci-windows.cmd
```

Die DLLs entstehen unter `out/Release/` und `out/Debug/`; der Build schreibt
nichts in die Spielinstallation. Release verwendet die statische C++-Laufzeit.
`python tools/package_release.py v0.1.0` erstellt das geprüfte ZIP unter `out/`.

- [Release-Prüfung und Änderungen](docs/RELEASE_V0.1.0_DE.md)
- [Technischer Aufbau](docs/TECHNICAL_REPORT_DE.md) und [Tests](tests/README.md)
- [Stadt-Widescreen](docs/WIDESCREEN_DE.md) und [Presenter](docs/PRESENTATION_DE.md)
- [Restaurant und Eingabe](docs/CENTERED_RESTAURANT_DE.md)
- [Bildpuffer und Flackerfix](docs/RESTAURANT_FLICKER_DE.md)
- [GitHub-Prüfungen und Branchschutz](docs/GITHUB_MAINTENANCE_DE.md)

Patch-Code: [MIT](LICENSE). MinHook: eigene [Lizenz](vendors/minhook-master/LICENSE.txt).
Releases enthalten keine Original-EXE, Spielgrafiken, Spielstände oder originalen
Bink-DLLs. Die Rechte am Spiel und dessen Dateien verbleiben bei den Rechteinhabern.
