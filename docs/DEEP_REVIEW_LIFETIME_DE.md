# Codepruefung: DirectDraw-Besitz und Fehlerpfade

## Behobene Funde

1. `CreatePresentationSurface` gab die einzige Front-Referenz an das Spiel weiter,
   behielt aber denselben rohen Zeiger fuer Paint/Flip. Eine fruehe Freigabe durch
   das Spiel konnte diesen Zeiger ungueltig machen. Der Presenter besitzt jetzt
   beide Erstellungsreferenzen; die Ausgabe an das Spiel erhaelt ein eigenes AddRef.
2. `Draw::CreateSurface`, `SetCooperativeLevel` und `SetDisplayMode` verwendeten nur
   den globalen Presenter-Schalter. Ein zusaetzliches DirectDraw-Objekt konnte
   dadurch den Presenter der ersten Instanz verwenden. Die drei Methoden pruefen
   jetzt auch, ob genau dieses Objekt den Presenter besitzt. Nichtbesitzer leiten
   an ihr eigenes Systemobjekt weiter und bauen den Presenter nicht ab.
3. Bei erfolgreichem Erstellen, aber fehlgeschlagenem Aktivieren des Attached-
   Detours blieb ein teilweise installierter Hook zurueck. Dieser Pfad entfernt
   jetzt den gerade angelegten Hook. Freigaben trennen beide Oberflaechenzeiger
   vor den COM-Release-Aufrufen, damit Wiedereintritt keine abgebauten Zeiger sieht.

Die ersten beiden Fehler sind durch Codeanalyse und synthetische Regressionen
belegt. Ein entsprechender Absturz im Originalspiel wurde damit nicht reproduziert.
Es gibt keine neuen EXE-Adressen oder veraenderten Gameplay-/Timing-Patches.

## Verifikation

Release/Debug x86 und 112 automatische Pruefungen bestanden. Elf neue Tests
pruefen Spiel-zuerst-/Presenter-zuerst-Freigabe, teilweisen Aufbau, wiederholten
Abbau, vor Release abgetrennte Zeiger sowie Display-Aufrufe und Abbau der Besitzer-
und Nichtbesitzerobjekte. Die Proxy-Tests kompilieren den tatsaechlichen
`ddraw_proxy.cpp` mit kuenstlichen DirectDraw-Objekten; sie verwenden keine Spieldateien.
Die neuen Referenztests ersetzen keinen Surface-Loss-/Treiber-Dauertest.

Mit dem finalen Build in der gesicherten GOG-Kopie: randloses 1920x1080, Menue,
Spielstandladen, Stadt, Restaurant per F2, Rezeptansicht per F3 und Rueckkehr zur
Stadt per F1 visuell geprueft. Danach geordnet per Alt+F4 beendet; kein Spielprozess
blieb zurueck. Dieser Ablauf prueft den normalen Lebenszyklus, erzwingt aber keine
fruehe Front-Freigabe oder fehlgeschlagene Hook-Aktivierung. Diese Sonderfaelle
sind getrennt von den synthetischen Tests zu beurteilen.

## Weitere konkrete Pruefpunkte

- `PresentationCooperativeLevel` merkt sich das HWND derzeit vor Abschluss aller
  Fensteranpassungen. Ein Monitor-/SetWindowPos-Fehler kann einen unvollstaendigen
  Initialisierungszustand hinterlassen. Eine transaktionale Fensterinitialisierung
  braucht eigene Fehlerpfadtests; sie ist hier noch nicht implementiert.
- Die Eingabe prueft Restaurant-Geometrie, aber gleiche Rechtecke identifizieren
  nicht zwingend denselben Raum. WM_PAINT liest aktuelle Szenengeometrie fuer den
  Frontpuffer, der noch aus einer vorherigen Szene stammen kann. Ein belastbarer
  Fix muss Render-/Frontpuffer-Metadaten und Szenengeneration zusammenhalten.
- Nach echter Surface-Wiederherstellung sind Bildinhalte nicht automatisch gueltig.
  Der vorhandene Restore/Blt-Retry ist kein Beweis fuer vollstaendigen Engine-Neuaufbau.
  Dafuer sind gezielte Verlusttests und ein verifizierter Redraw-Pfad erforderlich.
- `main.cpp` prueft Datei und geladenen Code vor den festen Adressen. Die
  RGB565-/GDI-Aenderungen sind storeunabhaengig; eine andere Steam-EXE besitzt
  weiterhin kein freigegebenes Adressprofil.

Originaldateien werden nicht ersetzt. Laufzeitpruefungen erfolgen in einer
separaten, zuvor gesicherten Spielkopie.
