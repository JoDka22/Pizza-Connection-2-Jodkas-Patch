# Schnellere RGB565-Bildaufbereitung

Der bisherige GDI-Pfad konvertiert und skaliert 16-Bit-Bilder in einem Aufruf.
Besonders im verkleinerten Fenster ist dieser Pfad teuer. Die neue Ausgabe
konvertiert bekannte RGB565-Oberflaechen mit einer Farbtabelle in einen wiederverwendeten
32-Bit-Puffer und laesst GDI danach skalieren. Engine-Oberflaechen, Gameplay-Timer,
Kamera, UI-Koordinaten und Maus-Transformationen bleiben gleich.

Die Tabelle bildet alle 65536 RGB565-Farben mit Bitreplikation ab. Menues lesen nur
die tatsaechlich verwendeten 800x600 Pixel, auch bei breiterem Quellstride. Andere
Farbmasken verwenden weiterhin den bisherigen 16-Bit-Pfad. Ungueltige Abmessungen
werden abgewiesen; bei Allokationsfehlern und fehlgeschlagener 32-Bit-Ausgabe dient
der Originalpfad als Rueckfall. Der zusaetzliche Quellpuffer braucht bei 1080p etwa
7,9 MiB, die Tabelle 256 KiB. Bei gleichem Quellformat wird der Puffer wiederverwendet.

## Automatische Pruefung

Release/Debug x86 mit Warnungen als Fehler und insgesamt 101 Pruefungen bestanden.
Der Debug-Build des Display-Projekts verwendet jetzt passend zu `_DEBUG` die
statische Debug-CRT; die Release-CRT bleibt statisch.
Zwölf vollstaendige RGB-Bildvergleiche pruefen Menue, Stadt, Restaurant und RGB555-
Rueckfall bei 1920x1080, 1467x825 und 1000x1000. Quellpadding und alle RGB565-Farben
sind enthalten. Ausserdem werden ungueltige Konvertierungsargumente abgewiesen.

Synthetischer A/B-Test mit alternierender Reihenfolge, fuenf Aufwaermbildern,
50 Bildern pro Durchlauf und Median aus drei Durchlaeufen, jeweils mit GdiFlush:

| Ansicht | Fenster | GDI16 vorher | Konvertierung + gepufferte Ausgabe |
| --- | --- | ---: | ---: |
| Menue | 1920x1080 | 8,893 ms | 1,399 ms |
| Stadt | 1920x1080 | 2,148 ms | 1,345 ms |
| Restaurant | 1920x1080 | 9,602 ms | 2,053 ms |
| Menue | 1467x825 | 8,705 ms | 0,881 ms |
| Stadt | 1467x825 | 22,071 ms | 1,393 ms |
| Restaurant | 1467x825 | 13,399 ms | 2,293 ms |

Der neue Vergleich enthaelt auch den finalen BitBlt, der alte zeichnet direkt in
das synthetische Ziel. Es handelt sich um Ausgabekosten, keine Spiel-FPS-Messung.
CI fordert Bildgleichheit, keine hardwareabhaengige Mindestbeschleunigung.

## Laufzeitpruefung

In der gesicherten GOG-Spielkopie wurden Intro, Menue, Laden, Stadt und der Wechsel
per F2 in das vergroesserte Restaurant visuell geprueft. Die Stadtkomposition im
normalen 1467x825-Fenster liegt in stabilen Probe-Zusammenfassungen jetzt bei etwa
1,84-1,88 ms statt zuvor 22,4-22,7 ms. Die gesamte Frontendzeit lag dort bei etwa
2,69-2,73 ms; weiterhin traten laengere Flip-Abstaende auf. Die Engine bleibt also
ein eigenstaendiger Engpass.

Ein weiterer 30-Sekunden-Zaehlerlauf ergab fuer die Stadt 19,79 % CPU-Median und
26,12 % Maximum relativ zu einem Kern (gesamte CPU maximal 1,63 %), Spiel-GPU 0 %,
DWM-Engine-Maximum 19,22 %. Zum Vergleich betrug der vorherige CPU-Median 44,53 %.
Die Messungen verwenden denselben Spielstand und dieselbe Fenstergeometrie, aber
laufen nacheinander bei fortschreitender Simulation und unterschiedlicher Spielzeit.
Sie sind kein kontrollierter FPS- oder GPU-Benchmark.

Im Restaurant ergab ein separater 30-Sekunden-Lauf 12,04 % CPU-Median, 26,05 %
Maximum relativ zu einem Kern, 1,63 % gesamtes CPU-Maximum und 19,16 % DWM-
Engine-Maximum. Die Spiel-GPU-Zaehler lagen bei 0 %. Die Komposition lag in
stabilen Zusammenfassungen bei etwa 2,11-2,15 ms statt zuvor 13,0-13,2 ms.
Ein weiterer einfacher Stuhl wurde ueber den rechten Katalog ausgewaehlt und
auf der vergroesserten Restaurantflaeche platziert. Das belegt diese konkrete
Eingabesequenz, nicht jede Moebelart oder jede Spielsituation.
Der Wechsel zur Rezeptansicht, Maximieren des Fensters und Rueckkehr ins Restaurant
wurden geprueft; beide platzierten Stuehle blieben sichtbar. Der kurzzeitig leere
Weltbereich beim Ansichtswechsel ist weiterhin beobachtbar und noch nicht behoben.
Nach Neustart mit Windowed=0 wurden Menue, Laden, die native 1920x1080-Stadt und
das zentrierte Restaurant ebenfalls visuell geprueft. Die Restaurantkomposition
lag dort in stabilen Zusammenfassungen bei etwa 2,46-2,58 ms. Diese kurze Pruefung
deckt keine Alt-Tab-/Surface-Loss-Dauerschleife ab.

## Noch zu pruefen

Langzeitpartien, echte Surface-Loss-Ereignisse und die Steam-EXE sind weiterhin
offen. Es gibt keine pauschale Freigabe fuer unbekannte EXE-Versionen.
