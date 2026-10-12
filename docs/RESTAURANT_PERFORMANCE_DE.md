# Restaurant-Komposition und Eingabe nach Geometriewechseln

## Aenderung

Die bisherige Komposition zeichnete zuerst die gesamte native Renderflaeche und
loeschte den Restaurantbereich anschliessend fuer die vergroesserte Darstellung.
Jetzt wird dieser Bereich beim ersten Zeichenschritt ausgespart. Sidebar, deren
ueberhaengende Register, Status und Fensterraender werden weiterhin gezeichnet.
Der Raum ersetzt danach den ausgesparten Bereich im unsichtbaren Bildpuffer.
Die Ausgabe erfolgt unveraendert erst nach erfolgreicher Komposition.

Fehlgeschlagene Ausschnittskomposition baut weiterhin das vollstaendige native
Ersatzbild auf. Stadt-/Menueausgabe und Pufferallokation bleiben unveraendert.

Eine zusaetzliche Eingabekorrektur verwirft Raumklicks, wenn die aktuellen
Grafikgrenzen nicht zum zuletzt ausgegebenen vergroesserten Raum passen.
Zuvor fielen solche Klicks auf untransformierte Koordinaten zurueck. Sidebar
und Status bleiben waehrend dieses kurzen Zustands bedienbar.

## Automatische Verifikation

Release und Debug x86 mit /W4 /WX; 83 Pruefungen bestanden.
Drei neue Eingabepruefungen decken veraenderte/passende Geometrie und weiter
bedienbare UI ab. Drei vollstaendige RGB-Bildvergleiche mit synthetischem Muster
vergleichen die alte und neue Komposition bei 1920x1080, 1467x825 und 1000x1000.
Alle RGB-Pixel stimmen ueberein, einschliesslich UI, Beschnitt und schwarzer Raender.
Die vorhandenen Fehler-/Resize-/GDI-Ressourcentests laufen weiter.

## Synthetischer Benchmark

Optimierter x86-Build, dieselbe RGB565-Quelle und derselbe 32-Bit-DIB als Ziel.
Fuenf Durchlaeufe je Variante, je zehn Aufwaermbilder und 100 Messbilder,
abwechselnde Reihenfolge, GdiFlush pro Bild; angegeben ist der Median.

| Ausgabegroesse | Bisher | Ausgesparter Weltbereich | Aenderung |
| --- | ---: | ---: | ---: |
| 1920x1080 | 11,209 ms | 9,464 ms | -15,6 % |
| 1467x825 | 30,722 ms | 12,484 ms | -59,4 % |
| 1000x1000 | 17,468 ms | 8,855 ms | -49,3 % |

Dies misst ausschliesslich GDI-Komposition auf einem Testsystem, keine Spiel-FPS,
Simulation, Ladezeiten oder Treiber-Testmatrix. Die Groessenordnung ist von
System und Skalierung abhaengig; CI prueft Pixelgleichheit ohne Zeitgrenzwerte.
Original-Spielgrafiken und private Systeminformationen sind nicht Teil des Tests.

Ausfuehrung im x86-Developer-Prompt nach dem normalen CI-Skript:
`out\presentation_benchmark.exe` fuer Zeiten, `--verify-only` fuer Pixelvergleiche.
Der veroeffentlichte v0.1.0-Release wird durch diesen Entwicklungsbranch nicht ersetzt.

## Spielpruefung des neuen Builds

Separate gesicherte GOG-Testkopie, Original-Bink, deutsche Spielversion 1.006:
Im randlosen 1920x1080 wurden Intro/Menue und Spielstand geladen. Stadt und
zentriertes Restaurant erschienen korrekt. Ein Stuhl wurde bei 1150,650 am
sichtbaren Rasterpunkt platziert; regulaer wurden 20 abgezogen. Rezeptansicht
(F3), Statistik (F4) und Rueckkehr (F2) funktionierten, der Stuhl blieb sichtbar.
Der ueberhaengende Informationsreiter schaltete zur Filialinformation um.
Ein separater Neustart im normalen 1469x857-Fenster bestaetigte Menue,
Spielstandladen, Stadt und korrekt zentrierten/vergroesserten Innenraum.

Der seltene Zeitpunkt zwischen Geometrieaenderung und Bildausgabe wurde durch
die automatischen Eingabepruefungen abgedeckt, nicht gezielt im Spiel injiziert.
Weitere Raumvarianten, Rotationen, Personal und laengere Sitzungen bleiben offen.
