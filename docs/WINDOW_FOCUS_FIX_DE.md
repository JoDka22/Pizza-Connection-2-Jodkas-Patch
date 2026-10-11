# Wiederherstellung nach Minimierung

Stand: 11.10.2026. Beim Spieltest im maximierten Fenstermodus liess sich das
minimierte Spiel durch Aktivierung nicht wieder sichtbar machen. Der Prozess
blieb vorhanden. Der Fokus-Hook speicherte auf `WM_ACTIVATEAPP(false)` eine
`WINDOWPLACEMENT`, deren `showCmd` bereits minimiert war. Beim Aktivieren wurde
genau dieser Zustand erneut gesetzt. Das erklaert die wiederholte Minimierung.

Vor der Wiederherstellung werden die drei minimierten Show-Kommandos auf normal
oder maximiert normalisiert. `WPF_RESTORETOMAXIMIZED` bestimmt dabei den vorherigen
Maximierungszustand. Position, Normalrechteck und alle anderen Felder bleiben
erhalten. Nicht minimierte Snapshots bleiben unveraendert. Der bestehende
Fokusadapter bleibt auf den optionalen gerahmten Presenter beschraenkt.

Release und Debug x86 bauen erfolgreich. Insgesamt 57 automatisierte Pruefungen
bestehen, davon vier neue Fokus-Regressionen fuer Normalfenster, maximierte
Fenster, alternative Minimierungskommandos und unveraenderte normale Snapshots.
Diese Tests allein belegen kein Verhalten im Spiel.

In der separaten Testkopie wurde zusaetzlich das normale Menuefenster mit
1469x857 Pixeln ueber die Titelleiste minimiert. Die Aufnahme bestaetigte den
minimierten Zustand. Aktivieren machte das Fenster wieder sichtbar; eine zweite
Aufnahme nach Abschluss der Animation bestaetigte dieselbe Groesse und das
vollstaendige Menue. Der vorherige Build blieb beim entsprechenden Versuch
minimiert. Original-EXE und Original-Bink-DLL stimmen weiterhin mit den zu Beginn
erfassten SHA-256-Werten ueberein.

Der gleiche Ablauf wurde anschliessend mit maximiertem Menuefenster wiederholt:
Minimierung bestaetigt, Aktivierung erfolgreich, zweite Aufnahme zeigt wieder
den maximierten Zustand und vollstaendige proportionale Ausgabe. Der Klick auf
Spiel fortsetzen funktioniert danach. Beide Tests erfolgen mit dem gebauten
Release-Fix; Debug wurde kompiliert.

Die Restaurant-Bildreste, Kamerazentrierung und dekorativen UI-Anker sind eigene
offene Aufgaben. Dieser Fix behauptet keine allgemeine Alt-Tab-, Surface-Lost-,
DPI- oder Mehrmonitor-Stabilitaet.
