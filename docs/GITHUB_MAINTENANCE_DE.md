# GitHub-Pruefung und Branch-Schutz

Der Integrationsbranch maintenance/github-ready enthaelt die aufeinander
aufbauenden Aenderungen aus PR #1 bis #6 plus GitHub Actions. Sein PR gegen
master dient zur gemeinsamen Pruefung; die bisherigen Einzel-PRs bleiben als
Entwicklungsdokumentation erhalten. Nicht zusaetzlich beide Wege mergen.

Der aktive Schutz fuer master und den jeweiligen Standardbranch sperrt
Force-Push und Loeschung. Aenderungen muessen ueber Pull Requests erfolgen;
Diskussionen muessen vor dem Merge aufgeloest sein. Nach dem ersten erfolgreichen
Actions-Lauf ist `Windows x86 build and tests` als Pflichtpruefung aktiviert,
mit aktuellem Zielbranch und festgelegter GitHub-Actions-App. Keine Ausnahmen
fuer Administratoren. Es wird keine
fremde Review-Freigabe erzwungen, die einen allein gepflegten Fork blockiert.

GitHub Actions baut Release und Debug mit VS2022/v143 x86. Anschliessend laufen
34 Layout-/Gitter-, 15 Darstellungs-/Fokus- und sieben Kamerapruefungen sowie die Abweisung
unbekannter Hosts anhand der echten Widescreen-Implementierung. Der identische
Ablauf ist lokal mit `cmd /d /c tools\ci-windows.cmd` ausfuehrbar.

Die Pipeline laedt keine Spieldateien herunter, braucht keine Secrets und erstellt
keine Releases. GOG-Fingerprints mit Originaldateien, Bink-Weiterleitung gegen
die Original-DLL und echte Spieltests bleiben lokale Pruefungen. Ein gruener
Build belegt weder allgemeine Spielstabilitaet noch korrekte Darstellung aller
Karten, Zoomstufen und Treiber. Bekannte Grenzen stehen in WIDESCREEN_DE.md,
PRESENTATION_DE.md und CAMERA_BOUNDS_DE.md. Der Integrations-PR bleibt bis zur
abschliessenden Bewertung dieser Grenzen ein Draft.
