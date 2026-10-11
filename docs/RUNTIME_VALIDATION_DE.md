# Laufzeitprüfung und Bugfixes – 11.10.2026

Dieser Bericht ergänzt den ursprünglichen Analysebericht. Branch: bugfix/proxy-runtime-validation, aufbauend auf compatibility/gog-safe-bootstrap (Draft-PR #1). Die Änderungen sind ein weiterer Prüfstand, kein Release.

## Reproduzierter und behobener Fehler

Der ursprüngliche Smoke-Test löste zuerst BinkGetError auf. Dadurch lud Windows binkw32o.dll über einen PE-Forwarder, bevor der Bootstrap-Wrapper getestet wurde. Wenn eine Anwendung zuerst nur BinkSetSoundSystem dynamisch auflöste, fand der Wrapper dagegen keine Original-DLL und lehnte die Audioinitialisierung ab. Der erweiterte Test scheiterte reproduzierbar mit 'first wrapper call did not load original Bink'.

Korrektur: DllMain merkt sich ausschließlich die eigene Moduladresse. Beim ersten Wrapperaufruf lädt der Bootstrap bei Bedarf binkw32o.dll über den absoluten Pfad neben dem Proxy, außerhalb DllMain. LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR und LOAD_LIBRARY_SEARCH_SYSTEM32 vermeiden die Suche im Arbeitsordner für diesen Fallback. Ein Ladefehler wird mit Windows-Fehlernummer protokolliert. Das zusätzlich geladene Original bleibt für die Prozesslebensdauer verfügbar; dynamische wiederholte Load/Unload-Zyklen sind kein unterstützter Einsatzmodus. Bereits durch Forwarder geladenes Bink wird weiterverwendet.

Zusätzliche Härtung: Die Versionsprüfung prüft mit VirtualQuery, ob Header und kompletter Codebereich lesbar und committed sind. Nullpointer, Guard-/NoAccess-Seiten und ungeeignete Schutzflags werden vor dem Hashen abgewiesen. Das vermeidet Zugriffsverletzungen bei einer bereits veränderten Speicherabbildung. Gleichzeitige Fremdeingriffe in Seitenschutz oder Code während der Prüfung bleiben außerhalb der unterstützten Konfiguration.

## Automatisierte Prüfungen

- Release und Debug Win32 mit MSVC v143 erfolgreich, keine beobachteten Compilerwarnungen.
- 15 native Prüfungen bestanden. Neu: Nullimage, private NoAccess-Codepage, kontrollierte Ablehnung statt Absturz und Wiederherstellung des Schutzes. Bestehende Hash-, Layout-, Codekorruptions-, Argument- und echte Fenstergrößentests bestehen.
- Proxy-Smoke-Test: Wrapper als allererster Exportaufruf lädt jetzt das Original; BinkGetError-Weiterleitung und wiederholte SetSoundSystem-Aufrufe bestehen.
- Derselbe Smoke-Test aus einem anderen Arbeitsordner besteht.
- Separate Testkopie ohne binkw32o.dll: Wrapperaufruf liefert kontrolliert 0; Log meldet Windows-Fehler 126. Keine Nullpointer-Trampolinsprünge.
- dllmain.cpp zusätzlich mit /W4 /WX kompiliert. main.cpp wird über den nativen Test mit /W4 /WX geprüft.
- Exportaudit: alle 83 Originalnamen/-ordinals, 82 Forwarder und ein Wrapper unverändert korrekt.

## Tatsächlicher Spieltest

Zwei separate vollständige Kopien der lokal vorhandenen Installation wurden unter out/game-validation angelegt. Die eine enthält das Original-Bink, die andere den Proxy und eine unveränderte lokale Binkkopie. Dieser Ordner wird nicht versioniert. Originalinstallation und vorhandene Original-Spielstände wurden nicht verändert.

Die Spieltests erfolgten auf einem einzelnen Windows-System; dies ist keine Windows-10/11-Testmatrix. Spiel-Fenstertitel: Version 1.006. Dateiressource und VER-Datei nennen andere Werte; die Freigabe bleibt deshalb hashbasiert.

Beobachtungen:

- Original-Testkopie: Intro und Hauptmenü sichtbar, reguläres Beenden über Menü.
- Proxy vor den neuen Fixes: Intro und Hauptmenü sichtbar, Profilfreigabe im Log, Freies Spiel/Charakter-/Szenarioauswahl per Maus erreichbar, Berlin-Karte geladen, Fahrzeuge/Fußgänger und Uhr bewegen sich.
- Ein Alt-Tab-Zyklus während dieses Laufs: Spiel blieb vorhanden; Screenshot während Hintergrundzustand nicht verfügbar, nach Reaktivierung korrekte Stadtansicht. Das bestätigt einen Wiederherstellungsversuch, keine allgemeine Alt-Tab-Stabilität.
- Final gebaute DLL mit neuen Fixes: Neustart, Intro, Hauptmenü, Charakter-/Szenarioauswahl und Berlin-Karte erneut erfolgreich. Profilfreigabe im Log.
- F5 öffnet die Optionen, Speicherseite zeigt einen neuen Speicherplatz. Ein neuer Teststand wurde unter dem unveränderten Standardnamen angelegt; Save/Games/save_000.piz (614327 Bytes) liegt ausschließlich in der Testkopie.
- Der neue Stand lässt sich im Proxy-Spiel laden. Nach Rückkehr zur Stadtansicht erscheint wieder die gespeicherte frühe Nachtphase mit laufender Uhr; der vorherige Tages-/Optionszustand wird ersetzt.
- Derselbe Teststand wurde anschließend ohne Überschreiben vorhandener Daten in die unveränderte Referenzkopie übertragen. Die Original-Bink-DLL dort hat weiterhin SHA-256 9d84028986f1638a5f64f2f310fd6e9fce20eea05f3af362c27f0444e10248d2. Das Originalspiel listet den Teststand auf und lädt ihn erfolgreich bis zur animierten Berlin-Karte mit laufender Uhr. Das belegt die Rückwärtskompatibilität dieses einen neu angelegten Spielstands, nicht sämtlicher älterer Saves.
- Automatisierte Unicode-Texteingabe ins legacy Namensfeld zeigte keine sichtbare Änderung, auch nach erneuter Fokussierung. Deshalb wurde der leere Testordner und Standardname verwendet. Das ist keine nachgewiesene allgemeine Tastaturstörung; Funktionstasten, Maus und Alt-F4 funktionierten. Physische Texteingabe muss separat geprüft werden.

## Grenzen

Noch kein vollständiges 'alles läuft': kein Langzeittest, keine Kampagne, Restaurant-/Wirtschafts-/KI-Vollprüfung, kein belegter Audio-Hörtest, kein Windows-Versions-/GPU-/DPI-/Monitorvergleich und keine Performance-Messung. Alte vorhandene Spielstände lagen nicht vor. Fenstermodus ist weiterhin bewusst deaktiviert, echtes 1080p/1440p-Widescreen fehlt. Es werden keine Gameplay-/Timerpatches aktiviert.

Die UI-Navigation verwendete die Tastaturbelegung aus dem Original-[Handbuch](https://www.mogelpower.de/manuals/Pizza_Connection_2_Handbuch.pdf), insbesondere F5 für Optionen und F1 für Stadtansicht. Screenshots, Original-Bink und Testspielstand werden nicht in GitHub hochgeladen.
