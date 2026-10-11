# Native Renderfläche: erster Widescreen-Baustein

Stand: 11.10.2026. **Diagnostischer Prototyp, kein fertiger Widescreen-Patch.**
Die deutsche GOG-EXE wird wie bisher anhand der Datei und des geladenen Codes
geprüft. Spiel und Assets werden weder verändert noch ins Repository aufgenommen.

## Nachgewiesenes Ergebnis

Die neue optionale `ddraw.dll` wird am ersten `DirectDrawCreate` initialisiert,
außerhalb von DllMain. Das liegt vor SetDisplayMode/CreateSurface, anders als
der späte Bink-Einstieg. Der Originalimport verwendet IDirectDraw und die alte
108-Byte-DDSURFACEDESC, nicht IDirectDraw7.

Im lokalen Vollbildversuch mit der GOG-Spielkopie:

```text
ddraw: SetDisplayMode 1920x1080x16 HRESULT=00000000
ddraw: primary surface 1920x1080 pitch=3840 caps=1000c238
ddraw: attached backbuffer 1920x1080 pitch=3840
```

Intro, Hauptmenü und Laden des vorhandenen Testspielstands wurden bedient.
Die Inhalte bleiben unverzerrt im ursprünglichen 800x600-Bereich links oben;
der übrige Bereich bleibt schwarz. **Mehr sichtbare Stadtfläche ist noch nicht
implementiert.** Die native Oberfläche allein genügt dafür nicht.

Ein lokaler Kamera-Versuch änderte zusätzlich die vier Kamera-/Zoomgrößen an
0x571950..0x57195c von 596x551 auf 1716x1031. Der Minimap-Ausschnitt wurde größer,
aber die Ausgabe blieb bei 800x600 abgeschnitten und überschrieb UI-Hintergründe.
Diese Änderung wurde wieder entfernt; auch ein `CameraProbe`-Eintrag bewirkt
im aktuellen Build nichts. Damit ist ein separater Container-/Clippingpfad
als Hindernis praktisch nachgewiesen. Es werden keine Kameraadressen beschrieben.

## Implementierung und Schutzmaßnahmen

- Separates Visual-Studio-Projekt `pc2display`, Win32, C++17, /W4 /WX.
- Absolute Auflösung des System-DDraw-Pfades vermeidet Rekursion in die Proxy-DLL.
- Ohne Konfiguration: nur Modus-/Surface-Logging, keine Auflösungsänderung.
- Unbekannte EXE: unveränderte Weiterleitung an System-DirectDraw, keine festen
  Adressen gelesen oder beschrieben. Wrapper nur beim bekannten Spiel.
- Opt-in-Probe beansprucht den ersten Aufruf atomar und prüft ursprüngliche
  Vollbild-, Objekt- und Rechteckwerte vor dem Schreiben.
- Nur Daten im Prozess werden geändert; keine EXE-Codepatches, keine Timeränderung.
- Falls der erste native Modus scheitert: Daten auf 800x600 zurücksetzen und
  Originalmodus versuchen. Ein späterer Moduswechsel löst diese Rücksetzung
  bewusst nicht aus, da dann bereits Renderpuffer existieren können.
- COM-Wrapper besitzt eigene Referenzzählung; Oberfläche und Pixeltransfer
  werden unverändert vom System ausgeführt. IDirectDraw/IUnknown sind unterstützt;
  andere COM-Interfaces werden vom Wrapper abgelehnt. Er ist gezielt für den
  verifizierten Importpfad gebaut, kein allgemeiner DirectDraw-Ersatz.

## Installation ausschließlich für Diagnose

Zuerst eine vollständige lokale Spielkopie einschließlich Spielständen anlegen.
Falls dort schon eine `ddraw.dll` liegt (z.B. ein anderer Wrapper), nicht ersetzen:
dieser Prototyp unterstützt keine Verkettung mit anderen DirectDraw-Proxys.
`out/Release/ddraw.dll` in die Kopie neben `fastfood2.exe` legen. Eine bestehende
Bink-Installation muss dafür nicht geändert werden. `pc2fix.ini.example` als
`pc2fix.ini` neben der EXE kopieren. Für normales Spielen `RenderProbe=0` belassen.

```ini
[display]
RenderProbe=1
Width=1920
Height=1080
```

Breite 800..4096, Höhe 600..2160 sind begrenzt; ungültige Angaben lassen den
Originalmodus bestehen. Das ist keine Zusicherung, dass Monitor/Treiber jede
Kombination unterstützen. Änderungen benötigen einen Neustart des Spiels.
Probe im Fenstermodus wird abgelehnt. Rückbau: neue DLL und INI aus der Kopie
entfernen. Keine System-DLL und keine Originaldatei überschreiben.

## Verifizierte Ansatzpunkte für die nächste Implementierung

| Adresse | Funktion / Beobachtung |
| --- | --- |
| 0x404420 | Initiale 800x600-Rechtecke; zweites Rechteck 800..860 ist separat zu untersuchen |
| 0x574734 / 0x574738 | Globale Renderbreite/-höhe, ursprünglich 800/600 |
| 0x573d38 | Initiales Hauptrechteck (0,0,800,600) |
| 0x537f20 | DirectDrawCreate, anschließend CooperativeLevel/DisplayMode und Surfaces |
| 0x53805f | Alter IDirectDraw::SetDisplayMode, vtable+0x54, 16 Bit |
| 0x5380bf / 0x538359 | Primary/Flipchain und GetAttachedSurface |
| 0x538fa0 | Surface-Lock übernimmt Maße, Pixeladresse und Pitch; Pitch wird durch 2 geteilt |
| 0x539860 / 0x539980 | Vollbild-Flip / Fenstermodus-Blitting |
| 0x5242da..0x52438d | Globales Rendering-Clipping und Zeichnung der UI-Container |
| 0x485538..0x485567 | Kamerarechteck aus 0x571950/0x571954 und Stadt-Renderaufruf |
| 0x4ea230 | Weltpuffer werden anhand der Kameramaße angelegt |
| 0x4ed0a0 | Kamera-/Cullingberechnung nutzt aktuelle und Basis-Zoomgrößen |

Adressen gelten nur für den bereits dokumentierten EXE-Hash. Die Bezeichnungen
der alten `types.h` sind keine verifizierte Beschreibung der UI-Container.

## Vorgeschlagene Zielarchitektur

1. Eine gemeinsame Auflösungsstruktur bestimmt Renderfläche und Stadtviewport.
   `display_layout.h` berechnet bereits die Zielrechtecke: 1080p-Welt 1716x1031,
   Sidebar 204 Pixel breit, Statusbereich 49 Pixel hoch. Dies ist ein Entwurf;
   die Sidebar enthält z.B. seitlich überstehende Icons, die beim tatsächlichen
   Verankern zusätzlich berücksichtigt werden müssen.
2. Root-Container, Welt-Ausgabe, Weltpuffer, Kamera-Culling und Dirty-Rectangles
   gemeinsam erweitern. Zuerst Containerkonstruktoren und Eingabegrenzen anhand
   des Originalcodes belegen. Nur die Kameravariablen zu ändern ist ungeeignet.
3. UI-Container rechts bzw. unten verschieben. Pixelgrafiken behalten ihre Maße;
   freie Panelbereiche auffüllen. Menüs, Restaurantansicht, Rezepteditor und
   Videos benötigen jeweils eine eigene Layout-/Zentrierungsregel.
4. Maus-Hit-Tests aus demselben Layout ableiten. Die getestete Sidebar-Transformation
   zeigt das Verfahren, ist noch kein installierter Eingabehook. Überstehende
   Buttons, Weltpicking, Minimap und Randscrollen separat abdecken.
5. Erst danach Fenstermodus, Alt-Tab/Restore, Displaywechsel, DPI und 1440p im
   Spiel prüfen. Die aktuelle Probe ist auf initiales Vollbild beschränkt.

## Prüfung und verbleibende Arbeit

Release und Debug wurden mit VS2022/v143 gebaut; neues Projekt /W4 /WX ohne
Warnungen. 13 Layout-/Koordinatentests, 15 bestehende Kompatibilitätstests und
ein DirectDraw-Smoke-Test im unbekannten Host bestehen. Der Smoke-Test liest
den aktuellen Modus, ohne ihn zu ändern. 1440p ist ausschließlich im Layouttest
geprüft, nicht als Spielbetrieb.

Der Modus-Fallback ist implementiert, aber ohne nachgewiesenen Treiberfehler
nicht als praktisch getestet zu bezeichnen. Keine FPS-/Ladezeitmessung, kein
Langzeittest, keine Zusicherung für Windows-10/11- oder Treibermatrizen.
Weitere manuelle Tests vor Freigabe: tatsächlicher erweiterter Stadtblick mit
korrektem Picking, alle UI-Ansichten, vier Kameraränder/Zoomstufen, Speichern und
Laden, Fenstermodus, Alt-Tab, Fokusverlust, SurfaceLost, mehrere Monitore und DPI.
