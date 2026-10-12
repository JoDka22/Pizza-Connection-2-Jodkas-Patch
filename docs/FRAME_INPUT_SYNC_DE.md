# Bild und Eingabe beim Szenenwechsel synchronisieren

## Ursache

Der Presenter las Format und Restaurant-Ausschnitt bislang bei jedem Paint aus
der aktuellen Engine-Szene. Der Frontpuffer konnte dabei noch Pixel der vorherigen
Szene enthalten. Die Mausabbildung verwendete ebenfalls das aktuelle native/800x600-
Format. Ein identisches Artwork-Rechteck reichte aus, um zwei unterschiedliche
Raumbilder faelschlich als gleich zu behandeln.

Die statische Pruefung der bekannten EXE zeigt den Renderer-Aufruf bei 0x5244b8
und den spaeteren Flip-Aufruf bei 0x524536. Der Renderpfad 0x523fe0..0x5243b1
beendet den Backbuffer-Aufbau vor diesem Flip. Es werden keine neuen Engine-Hooks
oder neuen versionsabhaengigen Adressen benoetigt.

## Aenderung

`FramePipeline` begleitet die vorhandenen Pixel:

- Nach abgeschlossenem Renderdurchlauf werden Format und Restaurant-Geometrie
  als Backbuffer-Metadaten erfasst.
- Nur eine erfolgreiche Back->Front-Kopie uebernimmt diese Metadaten in den Frontpuffer.
  WM_PAINT verwendet danach dessen gespeicherten Ausschnitt, nicht die neue Szene.
  Eine fehlgeschlagene Kopie sperrt Frontpuffer-Repaints bis zur naechsten
  erfolgreichen Kopie; die zuvor publizierte Eingabeabbildung bleibt erhalten.
- Erst erfolgreiche GDI-Ausgabe aktualisiert die Mausabbildung und Clientgroesse.
- Der bestehende Szenenaufbau-Hook erhoeht eine Generation. Eingaben werden bis
  zum ersten erfolgreich ausgegebenen Bild dieser Generation abgewiesen, auch
  bei zwei Raeumen mit gleichen Rechtecken. Tastaturereignisse bleiben unveraendert.
- Bei Kamerabewegung innerhalb derselben Szene bleibt der bestehende Artwork-
  Vergleich fuer Welt-Picking aktiv; Sidebar/Status sind dabei weiterhin bedienbar.
- Ein Fenster-Groessenwechsel akzeptiert Mauskoordinaten erst nach Ausgabe mit
  passender Clientgroesse. Ein verschachtelter Szenenwechsel kann keine veralteten
  Render-Metadaten uebernehmen.

## Automatische Verifikation

Release/Debug x86 und 125 Pruefungen bestanden. Dreizehn neue Regressionen pruefen
die getrennten Back-/Front-/Ausgabe-Stufen, unveraenderte Eingabe bei fehlender
Ausgabe, native und 800x600-Abbildung, gleiche Raumrechtecke, Kamerageometrie,
Groessenwechsel, verschachtelte Szenenwechsel, fehlgeschlagene Kopie und Abbau. Die Tests verwenden
synthetische Zustandsfolgen und keine Original-Spieldateien.

## Laufzeitpruefung

In der gesicherten GOG-Kopie mit randlosem 1920x1080: Menue und Spielstandladen,
Stadt, F2-Restaurant, Auswahl eines einfachen Stuhls und Platzierung bei 1150,650
geprueft. Danach F3 mit aktiver Vorschau, Salami-Rezept per Maus ausgewaehlt und
F2 zurueck: Beide Stuehle blieben sichtbar. Die proportionale 800x600-Rezeptansicht
und vergroesserte Raumansicht waren jeweils bedienbar. Der Ablauf erzwingt kein
millisekundengenaues Eingabeereignis waehrend einer fehlgeschlagenen Bildausgabe;
diese Faelle sind durch die separaten Zustandsregressionen abgedeckt.

Nach Neustart im normalen Fenster wurden Laden, Stadt, Restaurant, Maximieren,
Stuhlauswahl und Platzierung bei 1100,620 geprueft. Nach Wiederherstellen der
normalen Groesse reagierte der Informationsreiter und zeigte Filialinformationen.
Beide Testlaeufe wurden geordnet beendet. Die Testkonfiguration wurde danach
auf randloses Vollbild zurueckgesetzt.

## Grenzen

Die Aenderung unterdrueckt keine von der Engine gelieferten leeren Weltbilder und
verspricht keine Flackerfreiheit. Sie verhindert, dass deren Bild-/Szenenmetadaten
vermischt werden. Echte Surface-Loss-Ereignisse, unterschiedliche Raumvarianten
und die Steam-EXE brauchen weiterhin gesonderte Laufzeitpruefungen.
Im Restaurant-Test waren teilweise kleine Sprite-Reste am unteren Rand des
Artworks sichtbar. Ihre Ursache und die dynamischen Ausschnittgrenzen sind noch
nicht behoben; dieser Fix verspricht keine vollstaendige Darstellungsfehlerfreiheit.
Die Folgeaenderung [Restaurant-Spritegrenzen](RESTAURANT_SPRITE_BOUNDS_DE.md)
adressiert den zusaetzlichen Ausschnitt durch den Presenter getrennt von der
hier geprueften Bild-/Eingabe-Synchronisation.
