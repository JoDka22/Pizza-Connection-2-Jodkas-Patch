# Full-HD-Kameragrenzen

Stand: 11. Oktober 2026. Auf dem separaten Branch widescreen/fullhd-camera-bounds.
Ergänzt den Full-HD-Presenter; keine Änderung der Originaldateien oder Simulation.

## Nachgewiesene Ursache

0x4ed0a0 speichert bei vorhandenem Hintergrund die rechten/unteren physischen
Grenzen in Welt+0x10160/+0x10164. Danach zieht es die aktuelle Kamera ab und setzt
ein Maximum unterhalb des Minimums wieder auf das Minimum. Diese Korrektur verliert
die Information, dass der Viewport größer als der Hintergrund ist. Die Funktionen
0x4ed360 und 0x4ed3c0 begrenzen anschließend X/Y mit den originalen Margins;
0x4ed440 kombiniert beide und meldet, ob die Position geändert wurde.

Beobachteter Berliner Spielstand: Zoom 0 mit 1716x1031 und zulässigem Y-Bereich
1474..2487. Zoom 1 nutzt 3432x2062; die physische Unterkante liegt bei 3488.
Nach Originalberechnung erscheint Y=1474..1534 gültig, obwohl die physische
Obergrenze mit Margin nur 3488-2062+30=1456 beträgt. Am unteren Anschlag zeigt
die Stadt einen deutlich sichtbaren schwarzen Streifen. Das ist kein fehlender
DirectDraw-Puffer: der Originalallocator hat die größere Fläche angelegt.

## Sichere Implementierung

Nur bei 1920x1080 und einem original gültigen Intervall, dessen rekonstruierte
physische Obergrenze unter dem Minimum liegt, wird der angeforderte Kameraursprung
auf das originale Minimum gesetzt. Das bedeutet im beobachteten Fall Y=1474.
X bleibt normal scrollbar. Bei ausreichender Hintergrundfläche werden die
Anforderungen unverändert an die Originalfunktion weitergegeben. Bereits
invertierte Originalintervalle kleiner Weltbereiche bleiben unangetastet.

Die originalen Achsenfunktionen übernehmen weiterhin Begrenzung, Schreiben der
Position und Changed-Rückgabe. Keine direkten Kamerapositions-Writes im Hook.
Welt+0xc==16 verwendet die von der Originalroutine gewählten Basisabmessungen;
andere Welten die aktuellen Zoomabmessungen. Die originale Zoom-2-Korrektur von
56 wird erhalten. Speicherbereiche werden vor Objektzugriff geprüft; Installation
erst nach vollständiger EXE-Prüfung. Ein begrenztes Diagnoseprotokoll (maximal 48
Einträge mit je zwei Zeilen) zeigt Anforderungen, Ergebnis, Grenzen und Margins.

## Verworfenes Experiment und Grenzen

Ein zunächst erprobtes Zentrieren zwischen den physischen Grenzen ergab Y=1465,
unterhalb des originalen Minimums 1474. Die Testkopie stürzte bei Zoom 1 ab:
Windows-Anwendungsereignis 1000, Ausnahme 0xc0000005, fastfood2.exe+0x13779f.
Die disassemblierte Instruktion gehört zum komprimierten Sprite-Leser und liest
eine Quellzeile. Die genaue defekte Asset-Zeile wurde nicht mit einem Dump bewiesen;
ein Zusammenhang mit dem neuen Ursprung ist die Arbeitshypothese. Diese Variante
ist verworfen und nicht Bestandteil des Branches.

Die sichere Verankerung kann kein fehlendes Karten-/Hintergrundmaterial erzeugen.
Restflächen, isometrische Ecken und kleine andere Weltbereiche bleiben möglich.
Ein vollständig randfreier extremer Zoom verlangt weitere Analyse des Sprite-
Clippings oder einen anderen sichtbaren Zoom-Ausschnitt. Kein allgemeiner Fix für
alle Karten, Zoomstufen, Fahrzeug-Picking oder Kampagnen zugesichert.

## Tests

Release/Debug VS2022/v143 x86 erfolgreich gebaut. Sieben native Tests prüfen
normale Bereiche, beobachteten übergroßen Ausschnitt, Anfragen unter/über Grenzen,
unveränderte bereits invertierte Bereiche, Grenzgleichheit und extreme Werte.
Der neu kompilierte Test mit der tatsächlichen Widescreen-Implementierung weist
einen unbekannten Host vor festen Speicherzugriffen und MinHook-Initialisierung ab.

## Lokaler Spieltest der finalen Variante

Release-DLL ausschließlich in der separaten Testkopie, randlos bei 1920x1080:
Berliner Spielstand geladen und auf Zoom 1 herausgezoomt. Das Protokoll bestätigt
request=3703,1534 und actual=3703,1474. Der zuvor breite schwarze untere Streifen
ist deutlich reduziert; ein dünner Rest bleibt am unteren Kartenrand sichtbar.
Mehrere anschließende Bildaufnahmen zeigen laufende Personen/Fahrzeuge und Uhr.
In dieser Sequenz trat kein Absturz auf; das ist kein Langzeitnachweis.

Minikartenklicks zum unteren und oberen Rand behalten Y=1474 bei; die obere
Anforderung verschiebt X von 3703 auf 3041. Ein Klick auf das Wohnhaus bei
(1530,550), außerhalb des früheren 800-Pixel-Bereichs, öffnet die passende
Wohnhaus-Ansicht rechts. Die Rückkehr zu Zoom 0 wurde ebenfalls geprüft.
Andere Karten, Zoom 2, Fahrzeugauswahl und Kampagnen bleiben ungeprüft.
