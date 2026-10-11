# Restaurant: vollstaendige Bildausgabe statt sichtbarer Zwischenschritte

## Ursache und Korrektur

Der bisherige Presenter zeichnete zuerst das native Bild direkt ins Fenster,
loeschte dann den Stadtbereich und zeichnete zuletzt den vergroesserten Raum.
Diese drei sichtbaren Ausgaben koennen das gemeldete Flackern verursachen.
Ausserdem kopierte der originale WM_PAINT-Pfad bei 0x5246e0 ein unkomponiertes
Bild vor unserer erneuten Ausgabe. Die statische Analyse dieses Pfads zeigte
Grafikaktualisierung, keine Simulationslogik.

`PresentationBuffer` setzt das gesamte Bild jetzt in einem wiederverwendeten
32-Bit-DIB zusammen. Erst danach uebertraegt ein einzelnes BitBlt das fertige
Bild ins Fenster. WM_PAINT verwendet denselben Presenter mit BeginPaint/EndPaint;
WM_ERASEBKGND erzeugt keinen zusaetzlichen Hintergrund. Es gibt keine Zusage
von VSync oder einer treiberunabhaengigen atomaren Bildschirmaktualisierung.

Der Puffer wird bei Groessenwechseln ersetzt und beim Freigeben der Praesentation
explizit abgebaut. Ungueltige Groessen und fehlgeschlagene Kompositionen werden
nicht ausgegeben. Eine fehlgeschlagene Restaurantvergroesserung baut zuerst das
vollstaendige native Ersatzbild auf. Die Mausabbildung wird erst nach erfolgreicher
Ausgabe aktualisiert. Fehler behalten die Abbildung des zuletzt ausgegebenen Bilds.
Ein erneuter Bildwechsel waehrend Paint wird verschoben, damit er nicht in die
von Paint gesperrte DirectDraw-Oberflaeche schreibt.

## Verifikation am 11.10.2026

- Release und Debug x86 mit /W4 /WX: 75 automatische Pruefungen bestanden.
- Acht neue echte GDI-Pruefungen: unsichtbare Komposition, vollstaendige Ausgabe,
  stabile Handlezahl bei 20 Bildern, Fehler ohne Teilausgabe, nativer Ersatz,
  Groessenwechsel und schwarze Raender, begrenzte Allokation, Handlefreigabe.
- Gesicherte lokale Spielkopie, normales Fenster 1469x857: Tisch im vergroesserten
  Paradiso bei 820,510 platziert, regulaer 100 Geld abgezogen. Tisch blieb nach
  Wechsel zu Rezepten (F3), Statistik (F4) und zurueck (F2) sichtbar.
- Der links ueberhaengende Informationsreiter reagierte korrekt und zeigte
  Filialinformationen. Maximieren und Minimieren/Wiederherstellen zeigten nach
  Abschluss der Windows-Animation das korrekte vergroesserte Restaurant.
- Beim Fensterwechsel protokollierte der Build vor der zusaetzlichen
  Wiedereintrittssperre einmal DDERR_SURFACEBUSY (887601ae). Die Sperre adressiert
  den belegten Frontpuffer bei verschachteltem Paint/Flip; andere Ursachen dieses
  Fehlercodes sind damit nicht ausgeschlossen.
- Finaler Release-Build mit Wiedereintrittssperre in randlosem 1920x1080:
  Spielstand geladen, Stadt und Restaurant korrekt dargestellt, einfacher Stuhl
  bei 1150,650 am sichtbaren Rasterpunkt platziert; Preisabzug 20 bestaetigt.
- SHA-256 der Original-EXE und Original-Bink-DLL entsprechen weiterhin dem
  vorher gesicherten Stand. Nur die separate Testkopie erhielt die neue DLL.

## Grenzen und manuelle Fortsetzung

Einzelne Screenshots messen kein hochfrequentes Flackern. Der Test beweist die
Entfernung der direkten Zwischenausgaben und korrektes fertiges Bild, keine
allgemeine Flackerfreiheit auf allen Monitoren. Die Engine lieferte unmittelbar
nach F2 auch ein leeres Weltbild, bevor der Raum fertig aufgebaut war; das ist
eine weitere, noch offene Uebergangsquelle. Windows-Animationsbilder sind separat
vom fertigen Spielbild zu beurteilen.

Weitere Tests: laufende Moebelvorschau bewegen/rotieren, alle Raumvarianten und
Personal, wiederholte Fensterwechsel, Zoom und lange Sitzungen. Fuer die genaue
Flackermessung ist eine zeitlich ausreichend schnelle Aufnahme erforderlich.
Rezept- und Statistikansichten behalten ihr proportionales 800x600-Layout.
Keine Originaldateien oder Gameplay-Mechaniken werden durch diese Aenderung ersetzt.
