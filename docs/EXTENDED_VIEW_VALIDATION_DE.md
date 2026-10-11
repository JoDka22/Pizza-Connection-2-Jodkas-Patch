# Ansichtswechsel und Restaurant: Full-HD-Regression

**Historischer Stand von PR #8.** Die spaetere Debuggeranalyse widerlegt die
unten vermutete Ursache des Rueckweg-Absturzes: Ein Maus-Hit-Test liest das bereits
geloeschte Weltobjekt, nicht nachgewiesene Gitterbeschaedigung durch Kameramasse.
Die nachgewiesene Korrektur steht in [FURNITURE_TRANSITION_FIX_DE.md](FURNITURE_TRANSITION_FIX_DE.md).
Der inzwischen erweiterte Innenraum mit separatem Culling-Fix und aktuellen
Fenstertests steht in [RESTAURANT_WIDESCREEN_DE.md](RESTAURANT_WIDESCREEN_DE.md).
Die folgenden Beobachtungen beschreiben ausdruecklich den damaligen Zwischenstand.

Stand: 11.10.2026. Getestet wird die separate, ignorierte Spielkopie mit
Release-x86-DLL, `Widescreen=1`, `1920x1080`, `Presentation=1`, `Windowed=0`.
Originalinstallation und Original-Spielstand werden nicht bearbeitet.

## Fehler und Korrektur

Ein Restaurant verwendet ebenfalls Canvas-ID 42, Typ 7. Diese Kennung allein
identifiziert deshalb keine Stadtansicht. Beim ersten Restaurantwechsel stuerzte
die bisherige Implementierung ab: Windows-Ereignis 1000, Zugriffverletzung
`0xc0000005`, EXE-Offset `0xedb4a` (Weltgitter-Zugriff `0x4edb4a`).
Disassemblierung zeigt im Szenen-Dispatcher `0x485bb0`: Szenen 1, 2 und 3
verwenden die Stadt, Szene 4 den Restaurant-Innenraum. Der Zustand liegt am
Szenenobjekt bei `+0x290`. Beide Pfade rufen `0x4859c0` zum Canvas-Aufbau auf.

Der neue Hook setzt die Kamerabasis **in `0x4859c0`**, nach dem Abbau der alten
Ansicht und vor Canvas-Aufbau und originaler Pufferallokation. Stadt: 1716x1031;
Innenraum: originale 596x551. Der gesamte Innenraum-Bildschirm bleibt 800x600
und wird proportional auf 1440x1080 mit je 240 Pixel Seitenrand ausgegeben.
Nur Stadtansichten erhalten erweiterte Container, Hintergrundstreifen und die
Korrektur fuer uebergrosse Stadt-Kameraachsen.

Die Reihenfolge ist wesentlich: Ein zwischenzeitlicher Ansatz am Einstieg des
Dispatchers erlaubte den Restaurantbesuch und die Moebelplatzierung, verursachte
aber beim Rueckweg eine Zugriffverletzung bei Offset `0xed5ac`. Das ist der Grund
fuer den spaeteren Hook. Die beiden Fehleradressen sind nachgewiesen; die genaue
Beschaedigung einzelner Gitterobjekte wurde ohne Prozessdump nicht nachgewiesen.
Es werden keine Nullzeiger-Abfragen als Umgehung in die Spielmechanik eingebaut.

## Automatisierte Pruefung

`tools/ci-windows.cmd`: Release und Debug x86, eigener Code `/W4 /WX`.
29 Layoutpruefungen, 11 Praesentationspruefungen, sieben Kamerapruefungen und
eine Abweisung unbekannter Hosts: insgesamt 48 erfolgreich. Sechs neue
Layoutpruefungen decken Stadt-/Personen-/Fahrzeugprofil, Restaurantprofil,
unbekannte Szenen und die Rueckkehr von einer Legacy-Ansicht ab.
Diese Tests ersetzen keinen echten Szenenwechsel im Spiel.

## Beobachtungen in der Testkopie

- Stadt: Gebaeude bei Bildschirmposition (1540,875) ausgewaehlt; korrektes
  Wohnhaus-Panel am rechten Rand. Kauf-, Bauauswahl- und Bestaetigungsdialog
  bedienbar. Kleine Filiale durch normale Spielaktionen gebaut.
- Rezeptansicht F3: vollstaendige proportionale Darstellung, Klick auf Salami
  waehlt Salami und aktualisiert Name, Pizza und Zutatenpreis.
- Speichermenue und Einstellungen: vollstaendige Darstellung und Rueckkehr
  zur Stadt vor der Restaurantkorrektur beobachtet. Kein Originalsave geschrieben.
- Restaurant im ersten Korrekturversuch: Innenraum und Einrichtungskatalog
  sichtbar, einfacher Stuhl an (1050,575) platziert; Kontostand sinkt um 20.
  Dieser Versuch allein belegt noch keinen sicheren Rueckweg zur Stadt.
- Finaler Canvas-Hook: Statistik F4 vollstaendig, Restaurant betreten, Rueckweg
  zur nativen Stadt erfolgreich; Log bestaetigt erneute Allokation 1716x1031.
  Zweiter Restaurantbesuch ebenfalls erfolgreich. Ein Stuhl an (1050,700)
  wird korrekt platziert und kostet 20. **Der anschliessende direkte Wechsel
  F1 bei noch aktivem Platzierungsmodus stuerzt weiterhin ab**, Offset `0xed5ac`,
  vor Aufruf des neuen Canvas-Hooks. Dies ist nicht als behoben zu betrachten.
- Automatisch angelegten Testspielstand mit Filiale erneut geladen und
  Restaurant erfolgreich betreten. Der Teststand bleibt ausschliesslich lokal.

## Noch reproduzierbarer Fehler

Restaurant -> Einrichtung -> Stuhl waehlen -> Boden anklicken -> direkt F1:
Zugriffverletzung in `0x4ed5a0` beim Lesen des Weltobjekts bei `+0x1c`.
Ob die Platzierungsvorschau oder ein anderer Abbaupfad beteiligt ist, ist noch
nicht bewiesen. Vergleich ohne Widescreen und ein Test nach bewusstem Abbrechen
der Platzierung sind erforderlich. Die neue Kamera-Trennung behebt deshalb
nicht saemtliche Restaurantwechsel. Der Branch bleibt ein Draft.
Im Innenraum sind zusaetzliche schwarze Flaechen oberhalb/links des Kartenbilds
beobachtet worden; Restaurant-Kamerazentrierung bleibt ebenfalls offen.

## Weitere Prioritaeten

1. Wiederholte Wechsel Stadt/Restaurant/Rezept/Statistik und platzierte Objekte
   nach erneutem Laden eines separaten Testspielstands pruefen.
2. Dieselben Wechsel im Fenster bei mehreren Groessen und nach Minimierung
   testen, inklusive Randscrollen und Mauspositionen auf der Karte.
3. Kartenraender und verschiedene Staedte bei allen Zoomstufen pruefen.
4. Restaurantflaeche erst nach eigener Analyse von Gitter, Kamera und
   Innenraum-Hit-Tests erweitern. Bisher wird dort keine zusaetzliche Weltflaeche
   behauptet; Full-HD-Ausgabe ist proportional, das interne Layout bleibt original.

Langzeitverhalten, DPI/Mehrmonitorbetrieb, alle Missionen, Personal-, Liefer- und
Wirtschaftsdialoge bleiben offen. Spieltests laufen mit Release; Debug wird gebaut.
Keine Performanceverbesserung oder vollstaendige Windows-/Treiber-Matrix behauptet.
