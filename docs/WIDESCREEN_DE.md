# Native 1080p-Stadtansicht

Stand: 11.10.2026. Experimenteller, standardmäßig deaktivierter Patch für die
verifizierte deutsche GOG-EXE. Originaldateien und Spielmechaniken bleiben erhalten.
Der lokale Vollbildtest zeigt tatsächlich mehr Stadtfläche bei unverzerrten UI-Pixeln.
Eine Freigabe für alle Ansichten, Treiber und Windows-Versionen steht noch aus.

## Layout und Implementierung

`display_layout.h` liefert bei 1920x1080:

| Bereich | Rechteck / Größe |
| --- | --- |
| Stadt | (0,0)..(1716,1031), 1716x1031 |
| Sidebar | (1716,0)..(1920,1031), 204 Pixel breit |
| Status | (0,1031)..(1920,1080), 49 Pixel hoch |

Seitlich überstehende Tabs und der Panelrahmen bleiben absichtlich vor der
Sidebar-Kante, wie im Original. Pixelgrafiken werden verschoben und freie Rahmen-
und Panelflächen mit Ausschnitten des Originalhintergrunds gekachelt, nicht gestreckt.
Gebäudefilter bleiben zusammen; nur die identifizierte Stadtinfo-Fußleiste wandert nach unten.

Die separate `ddraw.dll` initialisiert vor dem ersten SetDisplayMode/CreateSurface,
außerhalb von DllMain. Sie prüft Datei- und geladenen Code-Hash sowie ursprüngliche
Datenwerte. Erst danach werden Daten geschrieben und MinHook-Detours eingerichtet.

| Adresse | Umsetzung |
| --- | --- |
| 0x574734 / 0x574738, 0x573d38 | Rendermaße und Hauptrechteck vor Surface-Erstellung |
| 0x523100 | Root-Konstruktor erhält native Breite/Höhe |
| 0x523fe0 | Sichtbaren Stadt-Canvas erkennen; Vollneuzeichnung bei Ansichtswechsel |
| 0x52f2e0 | Stadtcontainer erweitern; Sidebar-/Tab-/Minimap-Rechtecke verankern |
| 0x5242da..0x52438d | Bestehendes Clipping liest bereits die angepassten Renderglobals |
| 0x571950..0x57195c | Aktuelle und Basis-Kameramaße gemeinsam setzen |
| 0x485538 / 0x4ed0a0 | Original-Rendering/Culling liest neue Kameramaße; kein Immediate-Patch |
| 0x4ea230 | Originalallocator erhält neue Maße einschließlich Original-Zoomfaktor; Logging-Hook |
| 0x53d570 | Nur identifizierte Rahmen-/Panelgrafiken in Teilrechtecken zeichnen; Clip wiederherstellen |

Der originale Weltallocator behält Ownership, Pitch und Zoomverhalten. Stadt-Canvas
ID 42 bleibt bei (0,0); die bestehende Welttransformation verarbeitet native
Bildschirmkoordinaten. UI-Hitrechteck (+0x28) und Zeichenursprung (+0x38/+0x3c)
werden gemeinsam verschoben. So bleibt auch der relative Masken-Hittest korrekt.
Es gibt keinen pauschalen Maus-Remap, der die vergrößerte Welt wieder auf 800x600 abbildet.
`SidebarToOriginal` bleibt ein geometrischer Hilfstest, kein installierter Eingabehook.

Statistik, Restaurant, Rezepteditor, Hauptmenü und Videos haben weiterhin ihre
ursprüngliche Gestaltung. Beim Verlassen der Stadtansicht werden gemeinsame
Sidebar-Overlays zurückverankert; eigene Layouts für diese Ansichten fehlen noch.

## Schutz und Rückbau

Nur das dokumentierte EXE-Profil wird akzeptiert. Unbekannte Hosts werden vor
festen Adresszugriffen und vor MinHook-Initialisierung abgewiesen. Der Proxy leitet
an die absolute System-DirectDraw-DLL weiter. Der alte IDirectDraw-Import und die
108-Byte-DDSURFACEDESC sind unterstützt; andere DirectDraw-Interfaceversionen nicht.

Scheitert der erste Modus vor der Welt-/Root-Konstruktion, werden Detours und
Kamera-/Renderdaten zurückgesetzt und 800x600 versucht. Ein späterer Fehler löst
keine unsichere Größenänderung bereits existierender Weltpuffer aus. Dieser
Treiberfehler-Fallback ist implementiert, aber nicht praktisch ausgelöst worden.

Der späte Bink-Bootstrap kann bei gleichzeitig installierter Display-DLL geänderten
geladenen Code melden: Die zuvor verifizierten Display-Detours ändern den Prozesscode.
Bink führt dann keine eigenen Hooks aus; die unabhängige frühe Display-Prüfung
bleibt streng. Keine Hashprüfung wurde abgeschwächt.

Nur in einer vollständigen separaten Spielkopie installieren. Vorhandene fremde
`ddraw.dll` sichern und nicht ersetzen; Wrapper-Verkettung ist nicht unterstützt.
`out/Release/ddraw.dll` neben `fastfood2.exe` ablegen und `pc2fix.ini` erstellen:

```ini
[display]
Widescreen=1
Width=1920
Height=1080
```

`Widescreen=0` und `RenderProbe=0` belassen den Originalmodus. `RenderProbe=1`
bei deaktiviertem Widescreen ist weiterhin die frühere Surface-only-Diagnose.
Konfiguration erfordert Neustart. Wertebereich: 800..4096 / 600..2160; daraus folgt
keine Zusicherung unterstützter Monitor-Modi. Fenstermodus wird weiterhin abgelehnt.
Rückbau: hinzugefügte DLL/INI aus der Testkopie entfernen. Originalinstallation,
System-DLLs und ursprüngliche Spielstände nicht überschreiben.

## Nachgewiesene Prüfungen

- Release und Debug, VS2022/v143 x86: Build erfolgreich; eigener Code /W4 /WX.
- 23 native Layouttests: Bereiche, Randbedingungen, überstehende Titel, Status,
  Maskenursprung und gezielt erkannte Sidebar-Geometrien.
- 15 Kompatibilitätstests: Datei-/Codeprüfung, private Codeänderung, NoAccess,
  verstecktes echtes 800x600-Testfenster.
- Zusätzlicher Widescreen-Sicherheitstest: unbekannter Host wird vor Adresszugriff
  und MinHook-Initialisierung abgewiesen.
- DirectDraw-Smoke-Test: unbekannter Host liest den aktuellen Systemmodus ohne Moduswechsel.
- Computer Use auf separater lokaler GOG-Kopie: Laden des vorhandenen Spielstands,
  sichtbare Stadtfläche rechts und unterhalb der bisherigen Grenze, Sidebar rechts,
  Status unten und Gebäudemenüs. Gebäude-Picking an mehreren erweiterten Positionen
  einschließlich (1520,920) funktionierte in den beobachteten Fällen.
- Finaler Build: Stadt → Statistik → Rezepteditor → Stadt ohne versetzte Overlays;
  weiterer Wohnhaus-Klick bei (1535,870) öffnet das passende Gebäudemenü.
- Ein Alt-Tab-Zyklus mit Wiederaktivierung: vollständiges Stadtbild und UI erhalten.
  Während Fokusverlust war das minimierte Fenster nicht als Screenshot erfassbar;
  nach Wiederaktivierung war die Aufnahme wieder möglich. Kein Dauer-/SurfaceLost-Test.
- Der in der Testkopie erzeugte Autosave wurde erneut geladen. Das ersetzt keine
  systematische Prüfung aller bestehenden Spielstände oder manuellen Speicherpfade.
- Zoom-out: Originalallocator meldet 3432x2062 statt 1716x1031; größere Stadtübersicht
  sichtbar. Minimap-Klick verschiebt den Ausschnitt. Kein Nachweis für alle Zoomstufen.
- Auch bei Zoom-out öffnet ein Klick bei (1030,540) das passende Sondergebäude;
  Tooltip des rechts verschobenen Kids-Indikators erscheint am aktuellen Mausort.

Beispiel aus dem lokalen Prozesslog:

```text
ddraw: SetDisplayMode 1920x1080x16 HRESULT=00000000
ddraw: primary surface 1920x1080 pitch=3840 caps=1000c238
ddraw: attached backbuffer 1920x1080 pitch=3840
wide: root ... size=1920x1080
wide: world allocator completed 1716x1031 (viewport 1716x1031)
wide: world allocator completed 3432x2062 (viewport 1716x1031)
```

## Bekannte Grenzen und nächste Tests

- Nicht-Stadtansichten bleiben 800x600 links oben; eigene Zentrierung/Layout fehlt.
- Bei weitem Zoom/kleinen Karten können außerhalb der tatsächlichen Stadtgrenze
  schwarze Randflächen auftreten. Kamera-/Minimap-Randbegrenzung weiter prüfen.
- Rahmenfüllung ist gekachelt; mögliche sichtbare Wiederholungen sind kein Stretching.
- Randscrollen, alle vier Kameraränder, Fahrzeug-Picking und alle UI-Tabs noch nicht
  systematisch geprüft. Beobachtete Gebäudeklicks ersetzen keine vollständige Picking-Matrix.
- 1440p ist nur geometrisch geprüft; kein Spieltest. Fenstermodus/DPI/Multimonitor fehlen.
- Kein SurfaceLost-/Treiberfehler-Injektionstest, Langzeittest oder Windows-10/11-Matrix.
- Kein FPS-/Ladezeitgewinn behauptet; pro-Frame-Geometrieprüfung muss bei Bedarf profiliert werden.
- Speichern/Laden mit komplexen bestehenden Städten und verschiedenen Kampagnen weiter prüfen.

Vor Freigabe: neue/alte Spielstände, alle Ansichten hin/zurück, Zoom und Picking an
Rändern, Alt-Tab wiederholt, vollständige UI-Bedienung, Grafiktreiber/DPI/Monitore
und Memory-/Performance-Messungen in getrennten Testkopien prüfen.
