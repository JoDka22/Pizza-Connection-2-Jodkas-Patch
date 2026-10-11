# Nativer Full-HD-Innenraum und Moebelplatzierung

Stand: 11.10.2026, Branch `widescreen/fullhd-restaurant`. Alle Spieltests
verwenden die separate ignorierte Testinstallation mit der geprueften deutschen
GOG-EXE. Originale, Hauptbranch und Originalspielstaende bleiben unveraendert.

## Implementierung

Szene 4 verwendet jetzt ebenso wie die Stadt einen nativen 1716x1031-Viewport
innerhalb der 1920x1080-Renderflaeche. Die originalen Weltpuffer werden mit diesen
Massen alloziert. Die rechte Sidebar behaelt 204 Pixel, die untere Statusleiste
49 Pixel. Container, Draw-Ursprung, Clip- und Hit-Rechtecke wandern gemeinsam.
Weltgrafik und Moebel behalten ihre Originalpixel und ihre Weltkoordinaten.
Rezept, Statistik und Menues behalten vorerst das originale 800x600-Layout mit
proportionaler Ausgabe und dazu inverser Mausabbildung.

Die vergroesserte Kamera legte einen separaten Fehler des Originals offen:
Der untere rechte Culling-Eckpunkt kann auf einem gueltigen leeren Weltknoten
liegen. `0x4edaf0` liest dessen fehlende Nutzdaten; der Debugger zeigte den
Absturz bei `0x4edb4a`. Der eigentliche Zeichenloop ueberspringt leere Knoten
bereits korrekt. Der neue Hook berechnet deshalb ausschliesslich fuer einen
solchen Restaurantknoten den vorhandenen Zellindex aus seiner Position im
zusammenhaengenden 12-Byte-Knotenarray. Die Grenzen, Ausrichtung und Dimensionen
werden geprueft. Fuer andere Faelle bleibt der Originalpfad bestehen.

Eine reine Lesepruefung des laufenden Testprozesses fand in einem 260x260-Gitter
16435 belegte Knoten, deren gespeicherte Zellnummer ausnahmslos mit dem
Arrayindex uebereinstimmt; 51165 Knoten sind leer. Das bestaetigt diese
Interpretation fuer den untersuchten Innenraum, nicht automatisch fuer alle
Weltdaten. Keine Spieldaten wurden ins Repository uebernommen.

Der Platzierungs-Hook weist im Restaurant zusaetzlich Maus-Hit-Tests auf fehlende
oder leere Weltknoten ab. Er verwendet den originalen Ablehnungswert und
veraendert weder Inventar noch Preise. Der zuvor separat behobene
[Abbaufehler bei aktiver Vorschau](FURNITURE_TRANSITION_FIX_DE.md) bleibt erhalten.
Alle festen Adressen werden erst nach der bestehenden EXE- und Code-Pruefung
installiert; unbekannte Versionen erhalten keine Speicherpatches.

## Nachgewiesene lokale Pruefungen

- Release und Debug x86 kompilieren mit `/W4 /WX` fuer eigenen Code.
  34 Layout-/Gitterpruefungen, elf Praesentationspruefungen, sieben
  Kamerapruefungen und eine Abweisung unbekannter Hosts: insgesamt 53.
- Vollbild: Innenraum rechts jenseits der bisherigen 596-Pixel-Weltgrenze
  sichtbar; einfacher Stuhl bei nativer Position etwa (700,420) platziert.
  Preisabzug 20, Rueckwechsel F1 erfolgreich, Stuhl bei erneutem F2 erhalten.
- Fenster: 1469x857 inklusive Rahmen. Klick bei (1000,600) in die schwarze
  Flaeche mit ausgewaehltem Stuhl verursacht weder Absturz noch Kauf. Danach
  Boden-Klick bei (535,345), entsprechend etwa (699,411) in der Renderflaeche,
  platziert einen Stuhl; Kontostand 15494 -> 15474. F1 mit noch aktiver Vorschau
  erfolgreich. Diese Koordinaten gelten nur fuer die beobachtete Fenstergroesse.
- Maximierter Fenstermodus: Stadt proportional und vollstaendig; F2 zeigt den
  erweiterten Innenraum und beide Stuehle. F3 zeigt das vollstaendige Rezeptlayout
  mit seitlichen Balken; Klick auf Salami aktualisiert Name und Zutatenpreis
  auf 3,79. Die Ansichten werden jeweils nach Bildaktualisierung
  geprueft, nicht allein anhand der unmittelbaren Eingabe-Rueckgabe.

## Bekannte Grenzen und naechste Tests

Die vorhandene Restaurantgrafik ist endlich. Mehr Renderflaeche erzeugt keine
neue Boden- oder Raumgrafik; ausserhalb bleibt Schwarz. Der Innenraum steht
noch oben links, einzelne Dekorationen sitzen auf alter Hoehe. Bewegte Figuren
koennen ausserhalb der Hintergrundgrafik Bildreste hinterlassen. Das sind
offene Darstellungsfehler, kein fertig poliertes Widescreen-Layout.

Der obere linke Culling-Eckpunkt und der Fallback bei einem gaenzlich fehlenden
Knoten wurden nicht fuer saemtliche Kamerapositionen erweitert. Verschiedene
Restaurantgroessen, Laden/Speichern aller Moebeltypen, Personal-, Liefer- und
Wirtschaftsansichten, alle Staedte/Zoomstufen, DPI/Mehrmonitorbetrieb und lange
Spielsitzungen bleiben weitere manuelle Tests. Eine vollstaendige Windows-
oder Treibermatrix und Performanceverbesserungen werden nicht behauptet.

Beim Maximieren wurde einmal ein fehlgeschlagener Back-to-front-Blt geloggt
(`887601ae`); anschliessend zeichneten Stadt, Restaurant und Rezept weiter.
Dieser einzelne wiederaufgenommene Test belegt noch keine allgemeine
Alt-Tab-/Surface-Lost-Stabilitaet. Originale bleiben unveraendert, keine Releases.
