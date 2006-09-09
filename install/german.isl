; ******************************************************
; ***                                                ***
; *** Inno Setup version 5.1.0+ German messages      ***
; ***                                                ***
; *** Original Author:                               ***
; ***                                                ***
; ***   Michael Reitz (innosetup@assimilate.de)      ***
; ***                                                ***
; *** Contributors:                                  ***
; ***                                                ***
; ***   Roland Ruder (info@rr4u.de)                  ***
; ***                                                ***
; ******************************************************
;
; Diese Ubersetzung halt sich an die neue deutsche Rechtschreibung.
;
; $jrsoftware: issrc/Files/Languages/German.isl,v 1.10 2005/03/22 11:01:49 mreitz Exp $

[LangOptions]
LanguageName=Deutsch
LanguageID=$0407
LanguageCodePage=1252

[Messages]

; *** Application titles
SetupAppTitle=Setup
SetupWindowTitle=Setup - %1
UninstallAppTitle=Entfernen
UninstallAppFullTitle=%1 entfernen

; *** Misc. common
InformationTitle=Information
ConfirmTitle=Bestatigen
ErrorTitle=Fehler

; *** SetupLdr messages
SetupLdrStartupMessage=%1 wird jetzt installiert. Mochten Sie fortfahren?
LdrCannotCreateTemp=Es konnte keine temporare Datei erstellt werden. Setup abgebrochen
LdrCannotExecTemp=Die Datei konnte nicht im temporaren Ordner ausgefuhrt werden. Setup abgebrochen

; *** Startup error messages
LastErrorMessage=%1.%n%nFehler %2: %3
SetupFileMissing=Die Datei %1 fehlt im Installations-Ordner. Bitte beheben Sie das Problem, oder besorgen Sie sich eine neue Kopie des Programms.
SetupFileCorrupt=Die Setup-Dateien sind beschadigt. Besorgen Sie sich bitte eine neue Kopie des Programms.
SetupFileCorruptOrWrongVer=Die Setup-Dateien sind beschadigt oder inkompatibel zu dieser Version des Setups. Bitte beheben Sie das Problem, oder besorgen Sie sich eine neue Kopie des Programms.
NotOnThisPlatform=Dieses Programm kann nicht unter %1 ausgefuhrt werden.
OnlyOnThisPlatform=Dieses Programm muss unter %1 ausgefuhrt werden.
OnlyOnTheseArchitectures=Dieses Programm kann nur auf Windows-Versionen installiert werden, die folgende Prozessor-Architekturen unterstutzen:%n%n%1
MissingWOW64APIs=Ihre Windows-Version enthalt nicht die Funktionen, die vom Setup fur eine 64-bit Installation benotigt werden. Installieren Sie bitte Service Pack %1, um dieses Problem zu beheben.
WinVersionTooLowError=Dieses Programm benotigt %1 Version %2 oder hoher.
WinVersionTooHighError=Dieses Programm kann nicht unter %1 Version %2 oder hoher installiert werden.
AdminPrivilegesRequired=Sie mussen als Administrator angemeldet sein, um dieses Programm zu installieren.
PowerUserPrivilegesRequired=Sie mussen als Administrator oder als Mitglied der Hauptbenutzer-Gruppe angemeldet sein, um dieses Programm zu installieren.
SetupAppRunningError=Das Setup hat entdeckt, dass %1 zur Zeit ausgefuhrt wird.%n%nBitte schlie?en Sie jetzt alle laufenden Instanzen, und klicken Sie auf "OK", um fortzufahren, oder auf "Abbrechen", um zu beenden.
UninstallAppRunningError=Die Deinstallation hat entdeckt, dass %1 zur Zeit ausgefuhrt wird.%n%nBitte schlie?en Sie jetzt alle laufenden Instanzen, und klicken Sie auf "OK", um fortzufahren, oder auf "Abbrechen", um zu beenden.

; *** Misc. errors
ErrorCreatingDir=Das Setup konnte den Ordner "%1" nicht erstellen
ErrorTooManyFilesInDir=Das Setup konnte eine Datei im Ordner "%1" nicht erstellen, weil er zu viele Dateien enthalt

; *** Setup common messages
ExitSetupTitle=Setup verlassen
ExitSetupMessage=Das Setup ist noch nicht abgeschlossen. Wenn Sie jetzt beenden, wird das Programm nicht installiert.%n%nSie konnen das Setup zu einem spateren Zeitpunkt nochmals ausfuhren, um die Installation zu vervollstandigen.%n%nSetup verlassen?
AboutSetupMenuItem=&Uber Setup ...
AboutSetupTitle=Uber Setup
AboutSetupMessage=%1 Version %2%n%3%n%n%1 Internet-Seite:%n%4
AboutSetupNote=
TranslatorNote=German translation maintained by Michael Reitz (innosetup@assimilate.de)

; *** Buttons
ButtonBack=< &Zuruck
ButtonNext=&Weiter >
ButtonInstall=&Installieren
ButtonOK=OK
ButtonCancel=Abbrechen
ButtonYes=&Ja
ButtonYesToAll=J&a fur Alle
ButtonNo=&Nein
ButtonNoToAll=N&ein fur Alle
ButtonFinish=&Fertigstellen
ButtonBrowse=&Durchsuchen ...
ButtonWizardBrowse=Du&rchsuchen ...
ButtonNewFolder=&Neuen Ordner anlegen

; *** "Select Language" dialog messages
SelectLanguageTitle=Setup-Sprache auswahlen
SelectLanguageLabel=Wahlen Sie die Sprache aus, die wahrend der Installation benutzt werden soll:

; *** Common wizard text
ClickNext="Weiter" zum Fortfahren, "Abbrechen" zum Verlassen.
BeveledLabel=
BrowseDialogTitle=Ordner suchen
BrowseDialogLabel=Wahlen Sie einen Ordner aus, und klicken Sie danach auf "OK".
NewFolderName=Neuer Ordner

; *** "Welcome" wizard page
WelcomeLabel1=Willkommen zum [name] Setup-Assistenten
WelcomeLabel2=Dieser Assistent wird jetzt [name/ver] auf Ihrem Computer installieren.%n%nSie sollten alle anderen Anwendungen beenden, bevor Sie mit dem Setup fortfahren.

; *** "Password" wizard page
WizardPassword=Passwort
PasswordLabel1=Diese Installation wird durch ein Passwort geschutzt.
PasswordLabel3=Bitte geben Sie das Passwort ein, und klicken Sie danach auf "Weiter". Achten Sie auf korrekte Gro?-/Kleinschreibung.
PasswordEditLabel=&Passwort:
IncorrectPassword=Das eingegebene Passwort ist nicht korrekt. Bitte versuchen Sie es noch einmal.

; *** "License Agreement" wizard page
WizardLicense=Lizenzvereinbarung
LicenseLabel=Lesen Sie bitte folgende, wichtige Informationen bevor Sie fortfahren.
LicenseLabel3=Lesen Sie bitte die folgenden Lizenzvereinbarungen. Benutzen Sie bei Bedarf die Bildlaufleiste oder drucken Sie die "Bild Ab"-Taste.
LicenseAccepted=Ich &akzeptiere die Vereinbarung
LicenseNotAccepted=Ich &lehne die Vereinbarung ab

; *** "Information" wizard pages
WizardInfoBefore=Information
InfoBeforeLabel=Lesen Sie bitte folgende, wichtige Informationen bevor Sie fortfahren.
InfoBeforeClickLabel=Klicken Sie auf "Weiter", sobald Sie bereit sind mit dem Setup fortzufahren.
WizardInfoAfter=Information
InfoAfterLabel=Lesen Sie bitte folgende, wichtige Informationen bevor Sie fortfahren.
InfoAfterClickLabel=Klicken Sie auf "Weiter", sobald Sie bereit sind mit dem Setup fortzufahren.

; *** "User Information" wizard page
WizardUserInfo=Benutzerinformationen
UserInfoDesc=Bitte tragen Sie Ihre Daten ein.
UserInfoName=&Name:
UserInfoOrg=&Organisation:
UserInfoSerial=&Seriennummer:
UserInfoNameRequired=Sie mussen einen Namen eintragen.

; *** "Select Destination Location" wizard page
WizardSelectDir=Ziel-Ordner wahlen
SelectDirDesc=Wohin soll [name] installiert werden?
SelectDirLabel3=Das Setup wird [name] in den folgenden Ordner installieren.
SelectDirBrowseLabel=Klicken Sie auf "Weiter", um fortzufahren. Klicken Sie auf "Durchsuchen", falls Sie einen anderen Ordner auswahlen mochten.
DiskSpaceMBLabel=Mindestens [mb] MB freier Speicherplatz ist erforderlich.
ToUNCPathname=Das Setup kann nicht in einen UNC-Pfad installieren. Wenn Sie auf ein Netzlaufwerk installieren mochten, mussen Sie dem Netzwerkpfad einen Laufwerksbuchstaben zuordnen.
InvalidPath=Sie mussen einen vollstandigen Pfad mit einem Laufwerksbuchstaben angeben; z.B.:%n%nC:\Beispiel%n%noder einen UNC-Pfad in der Form:%n%n\\Server\Freigabe
InvalidDrive=Das angegebene Laufwerk bzw. der UNC-Pfad existiert nicht oder es kann nicht darauf zugegriffen werden. Wahlen Sie bitte einen anderen Ordner.
DiskSpaceWarningTitle=Nicht genug freier Speicherplatz
DiskSpaceWarning=Das Setup benotigt mindestens %1 KB freien Speicherplatz zum Installieren, aber auf dem ausgewahlten Laufwerk sind nur %2 KB verfugbar.%n%nMochten Sie trotzdem fortfahren?
DirNameTooLong=Der Ordnername/Pfad ist zu lang.
InvalidDirName=Der Ordnername ist nicht gultig.
BadDirName32=Ordnernamen durfen keine der folgenden Zeichen enthalten:%n%n%1
DirExistsTitle=Ordner existiert bereits
DirExists=Der Ordner:%n%n%1%n%n existiert bereits. Mochten Sie trotzdem in diesen Ordner installieren?
DirDoesntExistTitle=Ordner ist nicht vorhanden
DirDoesntExist=Der Ordner:%n%n%1%n%nist nicht vorhanden. Soll der Ordner erstellt werden?

; *** "Select Components" wizard page
WizardSelectComponents=Komponenten auswahlen
SelectComponentsDesc=Welche Komponenten sollen installiert werden?
SelectComponentsLabel2=Wahlen Sie die Komponenten aus, die Sie installieren mochten. Klicken Sie auf "Weiter", wenn sie bereit sind fortzufahren.
FullInstallation=Vollstandige Installation
CompactInstallation=Kompakte Installation
CustomInstallation=Benutzerdefinierte Installation
NoUninstallWarningTitle=Komponenten vorhanden
NoUninstallWarning=Das Setup hat festgestellt, dass die folgenden Komponenten bereits auf Ihrem Computer installiert sind:%n%n%1%n%nDiese nicht mehr ausgewahlten Komponenten werden nicht vom Computer entfernt.%n%nMochten Sie trotzdem fortfahren?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Die aktuelle Auswahl erfordert min. [mb] MB Speicherplatz.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Zusatzliche Aufgaben auswahlen
SelectTasksDesc=Welche zusatzlichen Aufgaben sollen ausgefuhrt werden?
SelectTasksLabel2=Wahlen Sie die zusatzlichen Aufgaben aus, die das Setup wahrend der Installation von [name] ausfuhren soll, und klicken Sie danach auf "Weiter".

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Startmenu-Ordner auswahlen
SelectStartMenuFolderDesc=Wo soll das Setup die Programm-Verknupfungen anlegen?
SelectStartMenuFolderLabel3=Das Setup wird die Programm-Verknupfungen im folgenden Startmenu-Ordner anlegen.
SelectStartMenuFolderBrowseLabel=Klicken Sie auf "Weiter", um fortzufahren. Klicken Sie auf "Durchsuchen", falls Sie einen anderen Ordner auswahlen mochten.
MustEnterGroupName=Sie mussen einen Ordnernamen eingeben.
GroupNameTooLong=Der Ordnername/Pfad ist zu lang.
InvalidGroupName=Der Ordnername ist nicht gultig.
BadGroupName=Der Ordnername darf keine der folgenden Zeichen enthalten:%n%n%1
NoProgramGroupCheck2=&Keinen Ordner im Startmenu erstellen

; *** "Ready to Install" wizard page
WizardReady=Installation durchfuhren
ReadyLabel1=Das Setup ist jetzt bereit, [name] auf Ihrem Computer zu installieren.
ReadyLabel2a=Klicken Sie auf "Installieren", um mit der Installation zu beginnen, oder auf "Zuruck", um Ihre Einstellungen zu uberprufen oder zu andern.
ReadyLabel2b=Klicken Sie auf "Installieren", um mit der Installation zu beginnen.
ReadyMemoUserInfo=Benutzerinformationen:
ReadyMemoDir=Ziel-Ordner:
ReadyMemoType=Setup-Typ:
ReadyMemoComponents=Ausgewahlte Komponenten:
ReadyMemoGroup=Startmenu-Ordner:
ReadyMemoTasks=Zusatzliche Aufgaben:

; *** "Preparing to Install" wizard page
WizardPreparing=Vorbereitung der Installation
PreparingDesc=Das Setup bereitet die Installation von [name] auf diesen Computer vor.
PreviousInstallNotCompleted=Eine vorherige Installation/Deinstallation eines Programms wurde nicht abgeschlossen. Der Computer muss neu gestartet werden, um die Installation/Deinstallation zu beenden.%n%nStarten Sie das Setup nach dem Neustart Ihres Computers erneut, um die Installation von [name] durchzufuhren.
CannotContinue=Das Setup kann nicht fortfahren. Bitte klicken Sie auf "Abbrechen" zum Verlassen.

; *** "Installing" wizard page
WizardInstalling=Installiere ...
InstallingLabel=Warten Sie bitte wahrend [name] auf Ihrem Computer installiert wird.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Beenden des [name] Setup-Assistenten
FinishedLabelNoIcons=Setup hat die Installation von [name] auf Ihrem Computer abgeschlossen.
FinishedLabel=Setup hat die Installation von [name] auf Ihrem Computer abgeschlossen. Die Anwendung kann uber die installierten Programm-Verknupfungen gestartet werden.
ClickFinish=Klicken Sie auf "Fertigstellen", um das Setup zu beenden.
FinishedRestartLabel=Um die Installation von [name] abzuschlie?en, muss das Setup Ihren Computer neu starten. Mochten Sie jetzt neu starten?
FinishedRestartMessage=Um die Installation von [name] abzuschlie?en, muss das Setup Ihren Computer neu starten.%n%nMochten Sie jetzt neu starten?
ShowReadmeCheck=Ja, ich mochte die LIESMICH-Datei sehen
YesRadio=&Ja, Computer jetzt neu starten
NoRadio=&Nein, ich werde den Computer spater neu starten
RunEntryExec=%1 starten
RunEntryShellExec=%1 anzeigen

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Nachste Diskette einlegen
SelectDiskLabel2=Legen Sie bitte Diskette %1 ein, und klicken Sie auf "OK".%n%nWenn sich die Dateien von dieser Diskette in einem anderen als dem angezeigten Ordner befinden, dann geben Sie bitte den korrekten Pfad ein oder klicken auf "Durchsuchen".
PathLabel=&Pfad:
FileNotInDir2=Die Datei "%1" befindet sich nicht in "%2". Bitte Ordner andern oder richtige Diskette einlegen.
SelectDirectoryLabel=Geben Sie bitte an, wo die nachste Diskette eingelegt wird.

; *** Installation phase messages
SetupAborted=Setup konnte nicht abgeschlossen werden.%n%nBeheben Sie bitte das Problem, und starten Sie das Setup erneut.
EntryAbortRetryIgnore=Klicken Sie auf "Wiederholen" fur einen weiteren Versuch, "Ignorieren", um trotzdem fortzufahren, oder "Abbrechen", um die Installation abzubrechen.

; *** Installation status messages
StatusCreateDirs=Ordner werden erstellt ...
StatusExtractFiles=Dateien werden ausgepackt ...
StatusCreateIcons=Verknupfungen werden erstellt ...
StatusCreateIniEntries=INI-Eintrage werden erstellt ...
StatusCreateRegistryEntries=Registry-Eintrage werden erstellt ...
StatusRegisterFiles=Dateien werden registriert ...
StatusSavingUninstall=Deinstallations-Informationen werden gespeichert ...
StatusRunProgram=Installation wird beendet ...
StatusRollback=Anderungen werden ruckgangig gemacht ...

; *** Misc. errors
ErrorInternal2=Interner Fehler: %1
ErrorFunctionFailedNoCode=%1 schlug fehl
ErrorFunctionFailed=%1 schlug fehl; Code %2
ErrorFunctionFailedWithMessage=%1 schlug fehl; Code %2.%n%3
ErrorExecutingProgram=Datei kann nicht ausgefuhrt werden:%n%1

; *** Registry errors
ErrorRegOpenKey=Registry-Schlussel konnte nicht geoffnet werden:%n%1\%2
ErrorRegCreateKey=Registry-Schlussel konnte nicht erstellt werden:%n%1\%2
ErrorRegWriteKey=Fehler beim Schreiben des Registry-Schlussels:%n%1\%2

; *** INI errors
ErrorIniEntry=Fehler beim Erstellen eines INI-Eintrages in die Datei "%1".

; *** File copying errors
FileAbortRetryIgnore=Klicken Sie auf "Wiederholen" fur einen weiteren Versuch, "Ignorieren", um diese Datei zu uberspringen (nicht empfohlen), oder "Abbrechen", um die Installation abzubrechen.
FileAbortRetryIgnore2=Klicken Sie auf "Wiederholen" fur einen weiteren Versuch, "Ignorieren", um trotzdem fortzufahren (nicht empfohlen), oder "Abbrechen", um die Installation abzubrechen.
SourceIsCorrupted=Die Quelldatei ist beschadigt
SourceDoesntExist=Die Quelldatei "%1" existiert nicht
ExistingFileReadOnly=Die vorhandene Datei ist schreibgeschutzt.%n%nKlicken Sie auf "Wiederholen", um den Schreibschutz zu entfernen, "Ignorieren", um die Datei zu uberspringen, oder "Abbrechen", um die Installation abzubrechen.
ErrorReadingExistingDest=Lesefehler in Datei:
FileExists=Die Datei ist bereits vorhanden.%n%nSoll sie uberschrieben werden?
ExistingFileNewer=Die vorhandene Datei ist neuer als die Datei, die installiert werden soll. Es wird empfohlen die vorhandene Datei beizubehalten.%n%n Mochten Sie die vorhandene Datei beibehalten?
ErrorChangingAttr=Fehler beim Andern der Datei-Attribute:
ErrorCreatingTemp=Fehler beim Erstellen einer Datei im Ziel-Ordner:
ErrorReadingSource=Fehler beim Lesen der Quelldatei:
ErrorCopying=Fehler beim Kopieren einer Datei:
ErrorReplacingExistingFile=Fehler beim Ersetzen einer vorhandenen Datei:
ErrorRestartReplace="Ersetzen nach Neustart" fehlgeschlagen:
ErrorRenamingTemp=Fehler beim Umbenennen einer Datei im Ziel-Ordner:
ErrorRegisterServer=DLL/OCX konnte nicht registriert werden: %1
ErrorRegisterServerMissingExport="DllRegisterServer Export" nicht gefunden
ErrorRegisterTypeLib=Typen-Bibliothek konnte nicht registriert werden: %1

; *** Post-installation errors
ErrorOpeningReadme=Fehler beim Offnen der LIESMICH-Datei.
ErrorRestartingComputer=Setup konnte den Computer nicht neu starten. Bitte fuhren Sie den Neustart selbst durch.

; *** Uninstaller messages
UninstallNotFound=Die Datei "%1" existiert nicht. Entfernen der Anwendung fehlgeschlagen.
UninstallOpenError=Die Datei "%1" konnte nicht geoffnet werden. Entfernen der Anwendung fehlgeschlagen.
UninstallUnsupportedVer=Das Format der Deinstallations-Datei "%1" konnte nicht erkannt werden. Entfernen der Anwendung fehlgeschlagen
UninstallUnknownEntry=In der Deinstallations-Datei wurde ein unbekannter Eintrag (%1) gefunden
ConfirmUninstall=Sind Sie sicher, dass Sie %1 und alle zugehorigen Komponenten entfernen mochten?
UninstallOnlyOnWin64=Diese Installation kann nur unter 64-bit Windows-Versionen entfernt werden.
OnlyAdminCanUninstall=Diese Installation kann nur von einem Benutzer mit Administrator-Rechten entfernt werden.
UninstallStatusLabel=Warten Sie bitte wahrend %1 von Ihrem Computer entfernt wird.
UninstalledAll=%1 wurde erfolgreich von Ihrem Computer entfernt.
UninstalledMost=Entfernen von %1 beendet.%n%nEinige Komponenten konnten nicht entfernt werden. Diese konnen von Ihnen geloscht werden.
UninstalledAndNeedsRestart=Um die Deinstallation von %1 abzuschlie?en, muss Ihr Computer neu gestartet werden.%n%nMochten Sie jetzt neu starten?
UninstallDataCorrupted="%1"-Datei ist beschadigt. Entfernen der Anwendung fehlgeschlagen.

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Gemeinsame Datei entfernen?
ConfirmDeleteSharedFile2=Das System zeigt an, dass die folgende gemeinsame Datei von keinem anderen Programm mehr benutzt wird. Mochten Sie diese Datei entfernen lassen?%nSollte es doch noch Programme geben, die diese Datei benutzen, und sie wird entfernt, funktionieren diese Programme vielleicht nicht mehr richtig. Wenn Sie unsicher sind, wahlen Sie "Nein" um die Datei im System zu belassen. Es schadet Ihrem System nicht, wenn Sie die Datei behalten.
SharedFileNameLabel=Dateiname:
SharedFileLocationLabel=Ordner:
WizardUninstalling=Entfernen (Status)
StatusUninstalling=Entferne %1 ...

[CustomMessages]

NameAndVersion=%1 Version %2
AdditionalIcons=Zusatzliche Symbole:
CreateDesktopIcon=&Desktop-Symbol anlegen
CreateQuickLaunchIcon=Symbol in der Schnellstartleiste anlegen
ProgramOnTheWeb=%1 im Internet
UninstallProgram=%1 entfernen
LaunchProgram=%1 starten
AssocFileExtension=&Registriere %1 mit der %2-Dateierweiterung
AssocingFileExtension=%1 wird mit der %2-Dateierweiterung registriert...
