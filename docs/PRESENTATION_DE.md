# Full-HD-Ausgabe und Fenstermodus

Stand: 11. Oktober 2026. Opt-in-Prototyp für die streng verifizierte deutsche
GOG-EXE. Nur 1920x1080 wird für diesen Backend aktiviert. Kein Release.

## Konfiguration und Rückbau

Nur in einer separaten vollständigen Spielkopie verwenden; fremde ddraw.dll nicht
überschreiben. Die gebaute out/Release/ddraw.dll neben fastfood2.exe ablegen.
pc2fix.ini im selben Verzeichnis:

```ini
[display]
Widescreen=1
Width=1920
Height=1080
Presentation=1
Windowed=1
```

Windowed=1 aktiviert ein verschiebbares, skalierbares und maximierbares Fenster.
Windowed=0 mit Presentation=1 aktiviert randlosen Vollbildmodus auf dem aktuellen
Monitor. Presentation=0 und Windowed=0 erhalten den bisherigen nativen Backend.
Bei deaktiviertem Widescreen wird der Presenter nicht aktiviert. Änderungen erst
nach Neustart. Das alte Kommandozeilenargument -windowed bleibt vom späten
Bink-Bootstrap abgelehnt; der neue frühe DirectDraw-Pfad verwendet ausschließlich
die INI. Rückbau durch Entfernen der hinzugefügten DLL und INI aus der Testkopie.

## Architektur

Die Engine zeichnet weiterhin in zwei 1920x1080-RGB565-Systemspeicherflächen.
Ein gezielter GetAttachedSurface-Hook stellt den erwarteten Backbuffer bereit;
der Flip-Hook kopiert Backbuffer nach Frontbuffer. Die ursprüngliche anschließende
Dirty-Region-Synchronisierung bleibt erhalten. DirectDraw verwendet DDSCL_NORMAL;
SetDisplayMode wird virtuell beantwortet und ändert den Desktopmodus nicht.

Stadtansichten nutzen den kompletten nativen Widescreen-Puffer mit erweitertem
1716x1031-Viewport. Andere Ansichten nutzen ihre ursprünglichen 800x600-Koordinaten.
Die Ausgabe vergrößert diese proportional und zentriert sie. Bei Full HD ergibt
das 1440x1080 mit je 240 Pixeln Seitenrand. Sie erhalten keinen zusätzlichen
Inhalt und keine neu entworfene UI. Fenstergrößen werden mit demselben Verfahren
angepasst; die Renderflächen werden beim Resizing nicht neu alloziert.

Der reale GDI-Ausgabecode ist gleichzeitig der getestete Pixeltransferpfad.
Der DIB-Header beschreibt nur die tatsächlich genutzten Zeilen und berücksichtigt
den 1920-Pixel-Stride auch bei 800x600-Inhalt. Mausnachrichten werden vor der
originalen Verarbeitung invers auf die jeweilige Renderansicht abgebildet;
Wheel-Nachrichten berücksichtigen Bildschirmkoordinaten. Balken aktivieren keine
Bedienelemente. ClipCursor wird im normalen Fenster aufgehoben, im randlosen
Vollbild auf den tatsächlichen Clientbereich begrenzt. Fremde/inaktive Fenster
werden durch diesen Clip-Hook nicht verändert.

Fokusverlust speichert die Fensterplatzierung; nach originaler Wiederaktivierung
wird sie wiederhergestellt. Ein fehlgeschlagener Frame wird übersprungen und
einmal protokolliert. Bei DDERR_SURFACELOST werden die Oberflächen wiederhergestellt
und die Kopie erneut versucht; die Wiederherstellung verlorener Bildinhalte bleibt
noch gezielt zu testen. Kein FPS-Gewinn zugesichert, keine Timer-/Gameplay-Patches.

## Nachgewiesene Prüfungen

- VS2022/v143 x86 Release und Debug erfolgreich gebaut, eigener Code /W4 /WX.
- 11 neue Ausgabe-/Maus-/GDI-Tests, 23 bestehende Layouttests, unbekannter Host
  vor festen Adresszugriffen und MinHook-Initialisierung abgewiesen, DirectDraw-
  Smoke-Test mit unverändertem aktuellen 1920x1080-Systemmodus bestanden.
- Computer Use in separater Spielkopie: Fensterstart, proportionaler Intro-/Menü-
  Inhalt, Laden des vorhandenen Spielstands und Stadt mit rechts verankerter UI.
- Maximierung und ein Alt-Tab-Zyklus: nach Rückkehr weiterhin maximiertes Fenster,
  vollständiges Stadtbild und Statusleiste. Kurzzeitig wurde bei Fokuswechsel ein
  fehlgeschlagener Front-Lock geloggt; Folgeframes wurden korrekt ausgegeben.
- Statistik (F4), Rezepteditor (F3), proportional vollständige Darstellung.
  Klick bei (1410,504) im maximierten Fenster wählt sichtbar Salami aus.
- Finaler Release-Build im randlosen 1920x1080-Modus: Menü/Spielstand geladen,
  Wohnhaus bei (1550,875) ausgewählt; ein Alt-Tab-Zyklus mit Wiederaktivierung
  zeigt vollständiges Stadtbild und ausgewählte Gebäude-UI. Log bestätigt
  virtuelle Renderauflösung, erhaltenen Desktopmodus und beide RGB565-Puffer.

## Offene Prüfungen

Alle Ansichten einschließlich Restaurant, Modal-/Speicherdialoge, Randscrollen,
Fahrzeuge, mehrfaches Resizing/Minimieren, alle Zoomstufen, Performance/Timing,
SurfaceLost-Injektion, DPI und mehrere Monitore. Einzelne Beobachtungen ersetzen
keinen Langzeittest und keine Windows-/Treiber-Matrix. Debug wurde kompiliert,
Spieltests verwenden Release. Originalinstallation wurde nicht verändert.

Kartenränder: 0x4ed0a0 berechnet Weltgrenzen bereits unter Berücksichtigung der
aktuellen Kameraabmessungen; 0x4ed360/0x4ed3c0 begrenzen Kameraachsen mit zusätzlichen
Welt-Margins. Ein weiterer pauschaler Abzug des Viewports wäre falsch. Schwarze
Bereiche außerhalb der tatsächlichen isometrischen Karte sind noch untersucht;
dieser Ausgabe-Backend behauptet keinen Kartenrand-Fix.
