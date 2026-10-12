# CPU/GPU- und Bildphasen-Diagnose

`PerformanceProbe=1` unter `[display]` aktiviert optionale Zusammenfassungen in
`pc2fix.log`. Standard ist 0. Es gibt keine Timer-, Simulations- oder FPS-Limit-Aenderung.
Die Probe misst nur die bestehenden Frontend-Aufrufe und protokolliert gesammelt
alle fuenf Sekunden; ohne aktivierte Probe werden keine zusaetzlichen QPC-Abfragen
im Bildpfad ausgefuehrt.

Gemessen werden Kopie Backbuffer->Frontbuffer, Oberflaechensperre, Komposition,
GDI-Uebertragung und gesamte Frontendzeit. Jede Phase zeigt Mittel und Maximum.
`gap` ist der Abstand zwischen Flip-Aufrufen, nicht die reine Zeichenzeit und nicht
die Monitor-FPS. `over50ms` zaehlt langsame Frontend-Aufrufe. Nicht fertig ausgegebene
Bilder bleiben als Differenz zwischen frames und completed sichtbar.
Die Zeiten messen CPU-seitige API-Aufrufe; sie sind keine GPU-Fence-/Queue-Messung.

## Erste externe Zaehlerpruefung

Je 30 Sekunden mit einsekundiger Abtastung, normaler Fenstermodus, gleicher
gesicherter Spielkopie mit dem optimierten Restaurant-Compositor:

| Ansicht | CPU-Median, Kapazitaet eines Kerns | CPU-Max, ein Kern | CPU-Max, gesamte CPU |
| --- | ---: | ---: | ---: |
| Stadt | 44,50 % | 53,83 % | 3,36 % |
| Restaurant | 27,66 % | 41,71 % | 2,61 % |

Die dem Spielprozess zugeordneten GPU-Engine-Zaehler lieferten 30 gueltige
Stichproben je Ansicht, jeweils 0 %. Das schliesst GPU-Spitzen des Windows-
Compositors oder anderer Prozesse nicht aus. Insbesondere verwendet dieses
Backend RGB565-Systemspeicher und GDI-Ausgabe. Weitere Messungen erfassen deshalb
auch den Desktop-Compositor; dessen Aktivitaet ist nicht allein dem Spiel zuzuordnen.

Die kurze Messung belegt keine allgemeine Spitzenfreiheit. Einsekundige CPU/GPU-
Zaehler koennen kurze Ruckler verbergen; die neue Bildphasenprobe dient zur
feineren Eingrenzung. Ein Zeitvergleich alter/neuer Spielbinaries wurde hier noch
nicht vorgenommen. Der synthetische A/B-Test des Compositors steht separat in
RESTAURANT_PERFORMANCE_DE.md.

## Laufzeit mit aktivierter Bildphasenprobe

Erneut je 30 Sekunden, normale Fensteransicht mit 1467x825 Clientpixeln:

| Ansicht | CPU-Median, ein Kern | CPU-Max, ein Kern | CPU-Max, gesamte CPU | DWM-GPU-Engine-Max |
| --- | ---: | ---: | ---: | ---: |
| Stadt | 44,53 % | 52,31 % | 3,27 % | 19,44 % |
| Restaurant | 28,99 % | 44,37 % | 2,77 % | 19,14 % |

Die Spielprozess-GPU-Zaehler lagen erneut bei 0 %. DWM-Werte sind die hoechste
Engine-Stichprobe des Desktop-Compositors, keine gesamte GPU-Auslastung und kein
isolierter Spielwert. Andere Desktopfenster liefen waehrend der Messung weiter.

In stabilen Stadt-Zusammenfassungen betrug die Komposition etwa 22,4-22,7 ms im
Mittel, die Kopie etwa 0,4 ms und die Ausgabe etwa 0,4 ms. Im Restaurant lag die
Komposition bei etwa 13,0-13,2 ms. Die beobachteten stabilen Zusammenfassungen
enthielten keine Frontend-Aufrufe ueber 50 ms und keine fehlgeschlagenen Ausgaben.
Flip-Abstaende erreichten trotzdem etwa 95-105 ms: Die Frontendzeit erklaert diese
Abstaende nicht vollstaendig. Daraus folgt keine pauschale FPS-Zusage.

Die Stadt wurde geladen und der Wechsel per F2 in das zentrierte Restaurant mit
korrekter rechter Sidebar visuell geprueft. Ladezeiten, Langzeitpartien, Surface-Loss
und ein isolierter GPU-Trace sind mit diesen Messungen nicht abgedeckt. Als naechster
Performance-Kandidat ergibt sich die RGB565-Konvertierung/Skalierung im GDI-Compositor;
eine alternative Implementierung braucht zuerst vollstaendige Pixelvergleiche.

Release/Debug x86 und 88 automatische Pruefungen bestanden, darunter fuenf neue
Pruefungen fuer Uhrfrequenz, ungueltige Intervalle, Spitzen/Mittelwerte, nicht
fertige Bilder und Ruecksetzen der Zusammenfassung. Die Probe protokolliert keine
Benutzernamen, Dateipfade, Hardware-IDs oder Spielstanddaten.

## Andere Distributionen

Hooks werden nach EXE-/Codeprofil freigegeben, nicht nach einem Store-Login.
Byte-identische EXEs koennen bereits dasselbe Profil verwenden. Eine abweichende
Steam-EXE muss separat analysiert werden, bevor ein weiteres Profil freigegeben
wird. Eine allgemeine Steam-Freigabe wird derzeit nicht behauptet.
Die Pruefung unbekannter Versionen und die unmodifizierte Systemweiterleitung
bleiben aktiv. Es sind keine geratenen Steam-Adressen hinterlegt.
