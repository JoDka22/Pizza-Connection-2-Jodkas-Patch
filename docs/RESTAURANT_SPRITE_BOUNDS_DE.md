# Restaurant: Vordergrund ueber den Hintergrund-Ausschnitt hinaus zeichnen

## Befund und Umsetzung

`MakeRestaurantView` verwendete Hintergrund plus 16 Pixel Rand als GDI-Quell-
rechteck. Die bereits gerenderten Figuren konnten darueber hinausragen; der
Presenter zeichnete dann nur die im Quellrechteck verbliebenen Sprite-Teile.
Der Randfix trennt deshalb den Hintergrund-Anker von den Grenzen der Weltabbildung.

Hintergrund und bisheriger Ausschnitt bestimmen weiterhin dieselbe stabile
Vergroesserung und Position. `RestaurantCanvasView` erweitert diese Transformation
auf die Weltflaeche ausserhalb der Sidebar-Reiter. GDI zeichnet alle dort verfuegbaren
Weltpixel mit dieser Transformation und clippt am aeusseren Viewport. Die Ansicht
folgt keinen einzelnen Figuren; es gibt kein hektisches Neu-Zentrieren oder Zoomen.
Ganzzahlige Rechtecke koennen die bisherige Ankerposition um einen Pixel runden.

Die Maus verwendet die inverse erweiterte Transformation. Neu sichtbare Weltpixel
ausserhalb des alten Hintergrund-Ausschnitts erhalten damit passende Engine-
koordinaten. Bereiche ausserhalb der transformierten Welt bleiben gesperrt.
Leere Weltzellen werden weiterhin vom bestehenden Platzierungs-Hit-Test abgewiesen;
es gibt keine neuen Moebel-, Inventar-, Geld- oder Spielstandoperationen.
Sidebar, ueberhaengende Reiter und Status behalten ihre bisherigen Pixel und
Koordinaten. Das GDI-Clipping wird nach dem Zeichnen geprueft wiederhergestellt.

Ein Laufzeittest zeigte zusaetzlich alte Sprite-Pixel unter dem Raumhintergrund.
Die originale Hintergrund-Wiederherstellung zeichnet dort keine Ersatzpixel.
Der neue Hook bei `0x53d600` erkennt ausschliesslich den Hintergrund des
Restaurant-Weltpuffers (Szene 4, ungekachelte 16-Pixel-Welt, identischer Hintergrund
und Zielpuffer). Vor dem originalen Hintergrundzeichnen werden nur die Teile
des aktuellen Dirty-Rechtecks ausserhalb des Hintergrund-Bitmaps schwarz gefuellt.
Die anschliessenden originalen Vordergrund-Paesse zeichnen die aktuellen Figuren.
Unveraenderte Pufferbereiche bleiben erhalten; es wird kein kompletter Weltpuffer
pro Frame geloescht.

Groesse und Ursprung stammen aus `0x53d090`/`0x53d100`. Der Zwischenpuffer enthaelt
pro Pixel zwei Farbbytes und ein Tag-Byte; `0x53b300` schreibt in Dreier-Schritten
und verwendet den Zeilenabstand in Bytes. Das Fuellen erfolgt deshalb direkt
mit diesem Format, dem originalen Hintergrund-Tag (`0x585518` bei Flag -1) und
gepruefter vollstaendiger Schreibbarkeit des Puffers. Die allgemeine
16-Bit-Bitmap-Fuellroutine ist fuer diesen Zwischenpuffer ungeeignet. Der
Palette-Sprite-Hook behaelt alle fuenf Stackargumente (`ret 0x14`) bei. Die Analyse
des Aufrufes `0x4efc73` und der nachfolgenden Zeichenpaesse bezieht sich nur auf
das bereits per Datei- und geladenem Text-Hash verifizierte deutsche GOG-Profil.
Unbekannte Hosts werden weiterhin vor festen Speicherzugriffen abgewiesen.

## Automatische Pruefungen

Release/Debug x86 und 160 Pruefungen bestanden. 21 neue echte GDI-/Geometrie-
pruefungen zeigen synthetische Sprites rechts und unter dem alten Ausschnitt,
kontrollieren inverse Mausabbildung und UI-Erhaltung bei 1920x1080, 1467x825 und
1000x1000. Bestehende Eingabetests pruefen weiterhin Szenengenerationen; ihre
Anker-Erwartungen beruecksichtigen einen Pixel Ganzzahlrundung. Die fruehere
Ausschnittkante ist absichtlich keine Eingabegrenze mehr.

Zehn weitere Pruefungen vergleichen die Hintergrund-Differenz pixelgenau:
vollstaendige und partielle Dirty-Bereiche, ueberstehende und leere Rechtecke,
keine Doppel-Loeschungen, keine Zugriffe ausserhalb des Puffers und Erhaltung
unveraenderter sowie nach der Wiederherstellung gezeichneter Figuren.
Vier zusaetzliche Pruefungen sichern das Drei-Byte-Pixelformat mit Tag 0 und 17,
Zeilenpadding, End-Guard und vollstaendiger Ablehnung fehlerhafter Groessen,
Zeilenabstaende und Rechtecke vor Schreibzugriffen ab.

Die vollstaendigen RGB16/RGB32-Bildvergleiche bestehen auch mit der erweiterten
Quelle. Historische Compositor-Zeitmessungen anderer Dokumente betreffen deren
damalige Ausschnittversion und sind keine neue Performance-Zusage.

## Laufzeitpruefung

Gesicherte GOG-Kopie, randloses 1920x1080: Laden, Stadt und Restaurant, Wechsel
zwischen Filialinformationen und Einrichtung, Stuhlauswahl und Platzierung auf
freiem Boden bei 1050,700 geprueft. Ein vorheriger Klick auf die Wand bei 1250,730
platzierte keinen Stuhl. Die Ansicht blieb zentriert und die Sidebar am bisherigen
Rand. In stabilen Probe-Zusammenfassungen betrug die Komposition etwa 2,3-2,4 ms;
die beobachteten Zusammenfassungen enthielten keine Frontend-Ausgabe ueber 50 ms.
Das ist keine kontrollierte A/B- oder GPU-Spitzenmessung.

Mit der zusaetzlichen Engine-Korrektur verschwand der zuvor reproduzierte
gesprenkelte Rest am unteren Hintergrundrand. Filialinformation, Einrichtung,
die drei vorhandenen Stuehle und eine weitere Bodenplatzierung bei 950,650
blieben erhalten. Die finalen stabilen Probe-Zusammenfassungen lagen bei etwa
2,5-2,6 ms Komposition und enthielten keine Frontend-Ausgabe ueber 50 ms.
Die Frame-Abstaende lagen im beobachteten Spielzustand teils um 80 ms; diese
Messung belegt keine Verbesserung der Simulation oder GPU-Auslastung.

Im normalen Fenster (1467x825 Client) wurden Laden, Stadt/Restaurant und
Stuhlauswahl geprueft. Eine weitere Platzierung am Fensterpunkt 760,500
erschien an der entsprechenden Bodenposition; die alten Stuehle und sauberen
Hintergrundraender blieben sichtbar.
Maximieren und eine weitere Bodenplatzierung bei 950,670 funktionierten mit
der neu berechneten Mausabbildung. Das Bild behielt seine Proportionen.

Ein verworfener Zwischenstand stuerzte beim Restaurantwechsel ab, weil die
allgemeine 16-Bit-Fuellroutine den Drei-Byte-Puffer mit falschem Zeilenabstand
behandelte. Der Debugger lokalisierte den Schreibzugriff; die finale Variante
nutzt den nachgewiesenen Byte-Abstand und prueft den kompletten Speicherbereich.

## Grenzen

Figuren am aeusseren Viewport koennen weiterhin normal abgeschnitten werden.
Der Presenter kann nur von der Engine gelieferte Pixel zeichnen; fehlende oder
bereits in der Engine abgeschnittene Teile rekonstruiert er nicht. Grosse oder
nicht vollstaendig sichtbare Raumhintergruende behalten den bisherigen nativen
Rueckfall. Leere Szenen-Uebergangsbilder und Steam-Kompatibilitaet bleiben offen.
Die vollstaendige Darstellung bewegter Figuren ueber dem alten Ausschnittrand
ist synthetisch geprueft. Eine voll besetzte Filiale mit mehreren gleichzeitig
ein- und ausgehenden Gaesten ist noch als laengerer manueller Test erforderlich.
