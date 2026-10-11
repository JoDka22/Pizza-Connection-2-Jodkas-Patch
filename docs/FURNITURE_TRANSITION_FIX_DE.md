# Moebelvorschau: Hit-Test beim Ansichtswechsel

## Nachweis

In der separaten Spielkopie reproduziert: Restaurant, Einrichtung, einfachen
Stuhl waehlen, Boden anklicken, F1. Der lesende x86-Debugger meldet bei
`0x4ed5ac` einen Zugriff auf `0x0000001c`, mit `ESI=0`.
Ruecksprung `0x4891d7` gehoert zum Platzierungs-Hit-Test `0x489180`;
dessen Aufrufer ist der Mauspfad bei `0x406471`. Das Weltobjekt wurde zuvor
vom Szenenwechsel in `scene+0x2fc` auf null gesetzt. Der frueher vermutete
temporäre Zeiger `scene+0x300` war bei aktiver Vorschau null.

## Aenderung

MinHook-Adapter fuer `0x489180`, nur nach erfolgreicher EXE-Pruefung im
aktivierten Widescreen-Pfad. Bei fehlender Welt setzt er das Vorschauflag
`+0x358` auf null und liefert den originalen Ablehnungswert 0. Mit aktiver
Welt werden alle sechs Argumente unveraendert an das Original weitergereicht.
Keine Objektlisten, Preise oder gespeicherten Moebel werden veraendert.
Die Diagnoseausgabe ist auf acht Meldungen begrenzt.

## Pruefungen und Grenzen

- Release und Debug x86 gebaut; 48 vorhandene automatisierte Pruefungen bestehen.
- Im Spiel: Stuhl an (1100,630) platziert; Kontostand 15773 -> 15753.
  Direkter F1-Wechsel erfolgreich, Stadtpuffer erneut 1716x1031.
  Beide Stuehle nach erneutem F2-Besuch sichtbar.
- Stuhlvorschau erneut gewaehlt, anschliessend F3: Rezeptansicht erfolgreich.
- Keine Aussage ueber die Reproduzierbarkeit im ungepatchten Original oder
  saemtliche weiteren Platzierungs-/Abbaupfade. Innenraum bleibt vorerst
  800x600 und wird proportional dargestellt. Echtes Innenraum-Widescreen
  ist eine eigene Folgeaenderung.

Originalinstallation, Originalspielstaende und Original-EXE unveraendert.
