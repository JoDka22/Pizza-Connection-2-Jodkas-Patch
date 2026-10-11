# Restaurant mittig und groesser darstellen

Der native 1716x1031-Innenraum zeichnete seine endliche Hintergrundgrafik links
oben. Der optionale Full-HD-Presenter stellt vollständig sichtbare Restaurantgrafiken
jetzt proportional vergroessert und mittig in der freien Weltflaeche dar.
Sidebar, ueberstehende Register und Statusleiste behalten ihre bisherigen Pixelmasse.
Voraussetzung: `Widescreen=1`, `Width=1920`, `Height=1080`, `Presentation=1`.
`Windowed=1` benutzt ein normales Fenster, `Windowed=0` den randlosen Vollbildmodus.

## Implementierung

Die bereits verifizierte deutsche GOG-EXE bleibt Voraussetzung fuer alle festen
Adressen. `RestaurantArtwork` liest nur die aktive Szene 4 mit Welt-Canvas,
prueft die Objekte und benutzt die originalen Grafik-Mass-/Ursprungsfunktionen
`0x53d090` und `0x53d100`. Der Hintergrundursprung entspricht der Zeichenroutine
`0x4efc2e..0x4efc73`, einschliesslich Kamera und Kachelrand. Es gibt keine Suche
nach nichtschwarzen Pixeln und kein von bewegten Figuren abhaengiges Nachzentrieren.

`restaurant_view.h` berechnet einen Ausschnitt mit 16 Pixeln Rand, passt ihn
proportional in die freie Flaeche ein und begrenzt die Vergroesserung auf 2x.
Die 35 Pixel breiten, in die Welt hineinragenden Sidebar-Register bleiben frei.
`DrawRestaurantView` liest nur den bestehenden RGB565-Frontbuffer und komponiert
den Ausschnitt ueber GDI; Weltpuffer, Kamera, Objekte und Spielstaende behalten ihre
originalen Koordinaten. Die Mausabbildung kombiniert Fenster-/Vollbildanpassung mit
der inversen Restauranttransformation. Schwarze Raender nehmen keine Raumklicks an.
Die Abbildung gilt nur nach erfolgreich gezeichnetem Bild und bei unveraenderten
Grafikgrenzen. Bei GDI-Ausgabefehlern wird die normale Ausgabe wiederhergestellt.

Vollstaendig ausserhalb des verfuegbaren Bereichs liegende, teilweise sichtbare,
unbekannte oder groessere Hintergruende erhalten die bisherige native Ansicht.
Das vermeidet einen geratenen Beschnitt. Dieser Rueckfall ist automatisch getestet;
weitere Raumtypen brauchen zusaetzliche manuelle Pruefung.

## Nachgewiesen

- Release und Debug x86 mit `/W4 /WX`; alle 67 automatischen Pruefungen bestanden.
- Zehn neue Geometrie-, Eingabe- und echte GDI-Pixelpruefungen: Zentrierung,
  vergroesserter Ausschnitt, erstes/letztes Bildsegment, unveraenderte UI-Pixel,
  inverse Mausabbildung auch im kleineren Fenster, Randabweisung und Rueckfall.
- Computer Use in einer separaten, gesicherten Spielkopie: Restaurant Paradiso
  mittig und vergroessert, Katalog bedienbar, Stuhl an sichtbarer Raumposition
  platziert und nach Stadtwechsel weiterhin sichtbar; schwarzer Rand ohne
  zusaetzliches Moebel. Rezeptansicht und Rueckkehr zum Restaurant funktionierten.
- Normales 1469x857-Fenster sowie maximiertes Fenster: proportionaler Innenraum,
  rechte UI weiterhin erreichbar. Minimieren und anschliessendes Aktivieren
  stellten die maximierte Ansicht nach dem Neuzeichnen wieder her.
- Laufzeitprotokoll des ersten Raumaufrufs: Grafik `26,79..825,630`,
  zentrierter Ausschnitt `128,16..1552,1015` im logischen Full-HD-Bild.
  Das entspricht etwa 1,71x Vergroesserung des Raums.
- Abschliessender Release-Build auch mit `Windowed=0` in randlosem 1920x1080
  gestartet: Restaurant mittig, Sidebar am rechten Rand; Katalogauswahl und
  Stuhlplatzierung bei Bildschirmposition `1150,650` funktionierten. Nach dem
  Neuzeichnen stand der Stuhl am angeklickten Rasterpunkt, der Preis von 20
  wurde regulaer abgezogen. Keine manuelle Aenderung von Inventar oder Geld.

## Grenzen und weitere manuelle Tests

Weitere Restaurantgroessen, andere Hintergrundvarianten, volle Gastraeume mit
Personal, alle Moebeltypen/Rotationen, wiederholte Zoomwechsel und laengere
Spielsitzungen sind noch nicht vollstaendig getestet. Die Darstellung vergroessert
Originalgrafik; sie erzeugt keine neuen Details und erweitert keine Restaurantflaeche.
Stadtansichten behalten ihren erweiterten Viewport. Rezept- und Statistikansichten
bleiben proportional dargestellte 800x600-Ansichten.

Ein zusaetzlicher GDI-Ausschnitt pro Restaurantbild kostet Zeichenarbeit; eine
Framerateverbesserung oder allgemeine Windows-10/11-Treiberkompatibilitaet wird
nicht behauptet. Der fruehere experimentelle Weltpuffer-Clear wurde separat lokal
gesichert und ist nicht Bestandteil dieses Branches. Originalinstallation,
Original-EXE und Original-Spieldateien wurden nicht veraendert oder hochgeladen.
