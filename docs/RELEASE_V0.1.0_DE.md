# v0.1.0 – erster oeffentlicher Full-HD-Vorabrelease

Fork von mallgrab/pc2fix, weiterentwickelt in JoDka22/pc2fix-self.
Unterstuetzt wird die analysierte deutsche GOG-EXE 1.006 mit 1920x1080.
Dies ist ein unabhaengiger Patch, kein offizielles Spielupdate.

## Enthalten

- Native Stadtflaeche 1716x1031 in einer 1920x1080-Renderflaeche.
- Rechte Sidebar und untere Statusleiste mit urspruenglichen UI-Proportionen.
- Zentrierter, vergroesserter Restaurantbereich mit inverser Mausabbildung.
- Randloses Full-HD und skalierbarer Fenstermodus; minimierte Fenster wiederherstellbar.
- Sichere EXE-Erkennung, Moebel-/Rasterkorrekturen und komponierte Bildausgabe.
- ZIP mit Display-Proxy, optionalem Bink-Proxy, startfertiger Konfiguration,
  Anleitung, technischer Dokumentation, Lizenzen und Build-/SHA-256-Metadaten.

## Zusaetzliche Codepruefung vor dem Release

Erneut geprueft: DLL-Bootstrap, EXE-Hash/PE-Pruefung, DirectDraw-Weiterleitung,
Hookinstallation und Rueckfall, Welt-/Kamerapuffer, Rasterzugriff, Restaurant-
Mausabbildung, GDI-Komposition und Ressourcenfreigabe sowie Buildkonfiguration.
Eine statische Durchsicht ist kein Beweis fuer die Fehlerfreiheit aller Spielpfade.

Zwei Korrekturen aus der Durchsicht:

1. Die Konfiguration akzeptierte bisher ungetestete native Widescreen-Aufloesungen,
   obwohl nur Full-HD gemeinsam mit Presenter und Kameragrenzen geprueft wurde.
   Widescreen ausserhalb 1920x1080 wird jetzt vor Geometrie-/Hookaenderungen abgewiesen.
2. Fehlgeschlagene GDI-Hintergrundfuellung oder Stretch-Moduswahl wurden nicht
   ausgewertet. Der Presenter verwirft jetzt diese Komposition bzw. baut die
   vollstaendige native Ersatzansicht auf. Leere Letterbox-Rechtecke werden uebersprungen.

Release/Debug x86 mit /W4 /WX und 77 automatische Pruefungen bestanden lokal.
Die zwei neuen Pruefungen decken die erlaubte Full-HD-Aufloesung und die Ablehnung
anderer Modi ab. Die Fehlerpfade fuer GDI-Fuellung wurden statisch geprueft;
eine kuenstliche Treiberfehler-Injektion wurde nicht vorgenommen.
Zusaetzlich bestanden 15 lokale Kompatibilitaetstests mit schreibgeschuetzter
GOG-EXE/privater Codekopie und ein Proxytest bei fehlender Original-Bink-DLL.
Der Bink-ABI-Audit bestaetigte alle 83 Exportnamen, Ordinals und Weiterleitungen.
Der neue Release-Kandidat lief mit Original-Bink und dem empfohlenen Display-
Profil in randlosem 1920x1080: Intro/Menue, Laden eines Autosaves, erweiterte
Stadt, zentriertes Restaurant und Stuhlplatzierung bei 1150,650 funktionierten.
Der sichtbare Stuhl stand am angeklickten Rasterpunkt; regulaer wurden 20 abgezogen.

## Installation und Grenzen

Siehe README fuer Sicherung, Installation und Rueckbau. Im Standardpaket ist
`Windowed=0` randloses Full-HD, `Windowed=1` aktiviert den normalen Fenstermodus.
Die originale Bink-DLL muss fuer den Display-Patch nicht ersetzt werden.
Keine Original-EXE, Original-DLL, Spielgrafik oder Spielstaende sind im Paket.

Weitere Raumvarianten, alle Rotationen, Personal, lange Sitzungen und Kartenraender
bei weitem Zoom bleiben offen. Rezepte/Statistik/Menues behalten ihr proportional
skaliertes 800x600-Layout. Kurze leere Weltbilder bei Ansichtswechseln sind moeglich.
Der Nutzer meldete nach dem Flackerfix kein sichtbares Flackern; hochfrequente
Messungen und allgemeine Treiber-/Windows-Kompatibilitaet sind damit nicht bewiesen.
Der Release verspricht keine FPS-/Ladezeitsteigerung und aendert kein Spielstandformat.
