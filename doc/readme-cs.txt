====== GeToDo - uživatelská dokumentace ======

Program GeToDo má za cíl pomoci èlovìku pøi organizaci osobních úkolù a projektù.

===== Instalace =====

Nejdøíve je tøeba nainstalovat runtime knihoven GTK a Gtkmm:
	- gtk-2.12.9-win32-2.exe
	- gtkmm-win32-runtime-2.10.11-1.exe

Pak je možno nainstalovat program GeToDo:
	- getodo-setup-<verze>.exe

Poznámka: Knihovny GTK využívají promìnné prostøedí GTK_BASEPATH a GTKMM_BASEPATH. Zároveò jejich adresáø bin musí být v promìnné PATH. Instalátor by to mìl nastavit automaticky. Zpravidla se se zmìny promítnou hned, ale je možné, že je obèas tøeba restart systému. Napøíklad bìžící instance Total Commanderu nebo pøíkazové øádky Windows si zmìny nenaète, ale pøi spuštìní GeToDo z nabídky Start funguje, nebo Windows Explorer si asi PATH obnoví sám.

===== Odinstalování =====

V nabídce Start je odkaz na program uninstall.exe, který provede odinstalování. Uživatelské soubory zùstanou nedotèeny.

===== Spuštìní =====

Program GeToDo je možno spustit kliknutím na odkaz, který se vytvoøil v nabídce Start->GeToDo, nebo pøímo na soubor getodo.exe v adresáøi, kam se program nainstaloval (napø. C:\Program Files\getodo\getodo.exe).

Pro uložení informací používá program GeToDo databázi SQLite uloženou v souboru (s pøíponou .db). Pøi spuštìní bez parametrù se automaticky použije soubor getodo.db v aktuálním adresáøi (napø. C:\program files\getodo\getodo.db). Chcete-li použít jiný soubor, zadejte cestu k nìmu jako parametr pøi spuštìní pogramu GeToDo.

===== Ovládání =====

Po spuštìní se objeví okno, které se skládá z nìkolika èástí:
	- stromový pøehled úkolù
	- panel pro editaci úkolù
	- panel s funkèními tlaèítky
	- filtrovací panel
	
Kontextová nápovìda funguje u vìtšiny ovládacích prvkù. Zobrazí se, jakmile najedete kurzorem na daný prvek a chvíli jej podržíte na místì.

==== Panel s funkèními tlaèítky ====

- New top-level task
	- vytvoøí nový úkol na nejvyšší úrovni (takový úkol není podúkolem jiného)
- New task on the same level
	- vytvoøí nový úkol na stejné úrovni jako je úkol právì oznaèený v pøehledu úkolù
	- tagy se zkopírují z pøedka, pokud nový úkol nìjakého má 
	- pokud žádný oznaèení není, vytvoøí úkol na nejvyšší úrovni
- New subtask
	- vytvoøí nový úkol jako podúkol úkolu právì oznaèeného v pøehledu úkolù
	- tagy se zkopírují z pøedka, pokud nový úkol nìjakého má
	- pokud žádný oznaèení není, vytvoøí úkol na nejvyšší úrovni
- Delete task
	- vymaže úkol právì oznaèený v pøehledu úkolù a také všechny jeho potomky
	- nejdøíve se zeptá
- Generate next task by recurrence
	- vytvoøí novou instanci úkolu vybraného a nastaví nové datum deadline podle definice opakování
- Toggle filtering
	- zapne/vypne právì aktivní filtr (viz sekci Filtrovací panel)

==== Stromový pøehled úkolù ====

Zde se zobrazuje pøehled úkolù uspoøádaný do stromu.

Zobrazeno je nìkolik sloupcù:
	- Id - èíselný identifikátor úkolu
	- Description - popis úkolu
	- Done - pøíznak, zda-li je úkol hotov
	- % - kolik procent je hotovo
	- ! - priorita
	- Deadline - datum, dokdy má být úkol hotov 

Kliknutím na øádek úkolu se vybraný úkol otevøe v panelu pro editaci úkolu.

Úkoly v stromu je možné øadit podle zobrazených sloupcù, a to kliknutím na hlavièku sloupce. Je možno pøepínat mezi tøemi režimy (vzestupnì, sestupnì, vypnuto).

Oznaèit lze pouze jeden úkol. CTRL-kliknutí na vybraný úkol jej odznaèí.

Pokud je aktivní nìjaký filtr, zobrazí se v pøehledu jen vyfiltrované úkoly.

==== Panel pro editaci úkolù ====

Zde se zobrazuje obsah úkolu právì vybraného v pøehledu úkolù a je ho možné upravovat. Úprava se obecnì uloží do databáze až pøi opuštìní právì upravovaného políèka.

- Title - nadpis úkolu
- Description - delší popis
- Tags
	- seznam tagù oddìlený èárkami, tagy mohou obsahovat mezery
	- tagy slouží jako kategorie, jen je možno úkolu pøiøadit tagù více najednou
- Done - hotovo
- % - kolik procent je hotovo (0-100)
- Priority - èíselná priorita (0-10)
- Recurrence - opakování úkolu
	- je možné opakování upravit pomocí dialogu pro editaci opakování nebo pøímo zapsat v textovém tvaru
- Id - èíselná identifikace
- Deadline - datum, kdy má být úkol hotov - formát RRRR-MM-DD
- Start - datum, kdy byla zapoèata práce na úkolu - formát RRRR-MM-DD
- Completed - datum, kdy byl úkol splnìn - formát RRRR-MM-DD
- Created - datum, kdy byl úkol vytvoøen - formát RRRR-MM-DD HH:MM:SS
- Last Modified - datum, kdy byl úkol naposledy upraven - formát RRRR-MM-DD HH:MM:SS

=== Dialog pro editaci opakování úkolù ===

Opakování úkolu (rekurence) je nastavení, jakým zpùsobem se má opakovat výskyt úkolu (konkrétnì datum deadline). Je nìkolik možností, jak má opakování vypadat.

Další výskyt úkolu je možné vytvoøit pomocí tlaèítka Generate next task by recurrence.

Opakování má i textový zápis. Po editaci v dialogu se zobrazí v pøíslušném políèku, ale je možné jej editovat i pøímo. Pøi chybì se automaticky nastaví opakování Once.

- Once - opakování pouze jednou
	- žádný další výskyt není
	- textový zápis: <prázdný øetìzec>
- Daily - opakování každý n-tý den
	- textový zápis: D <perioda v dnech>, pø. D 2
- Weekly - opakování každý n-tý týden
	- existují dva režimy:
	- každý n-tý týden od aktuálního data deadline
		- textový zápis: W <perioda v týdnech>, pø. W 3
	- každý n-tý týden po nastavených dnech v týdnu
		- textový zápis: W <perioda v týdnech> <vybrané dny>, pø. W 1 Mon Thu Fri
			- vybrané dny se zapisují pomocí tøípísmenných zkratek anglických názvù dní v týdnu
- Monthly - opakování každý n-tý mìsíc
	- existují dva režimy:
	- každý n-tý mìsíc od aktuálního data deadline
		- textový zápis: M <perioda v mìsících>, pø. M 1
	- každý n-tý mìsíc v nastavený den v mìsíci
		- textový zápis: M <perioda v mìsících> <den v mìsíci>, pø. M 1 15
- Yearly - opakování každý rok
	- existují dva režimy:
		- každý rok od aktuálního data deadline
			- textový zápis: Y
		- každý rok v nastavený den a mìsíc
			- textový zápis: M <den v mìsíci> <mìsíc>, pø. M 3 Feb
				- mìsíc se zapisuje pomocí tøípísmenné zkratky anglického názvù mìsíce
- Interval of days - každý den v intervalu mezi dvìma daty
	- textový zápis: I [<datum zaèátku>/<datum konce>]
		- pø. I [2008-Jan-02/2009-Mar-14]
	- datum se zapisuje ve formátu RRRR-MM-DD nebo not-a-date-time
	- pokud se jako hranice uvede not-a-date-time

==== Filtrovací panel ====

Úkoly je možno filtrovat, a to hned dvìma zpùsoby - pomocí tagù a/nebo pomocí filtrovacích pravidel. Filtrovací panel obsahuje proto dvì èásti - seznam tagù a seznam pravidel. Tlaèítka All a Any znaèí, jak se mají spojit vybrané položky seznamù (prùnikem resp. sjednocením). Položek je možno vybrat/odznaèit pomocí CTRL-kliknutí.

Tlaèítkem Filter by tags se zapíná/vypíná filtrování pomocí tagù. Tlaèítkem Filter by rules se zapíná/vypíná filtrování pomocí filtrovacích pravidel. Dohromady jsou tyto dvì èásti spojeny prùnikem.

Celkovì se filtrování zapíná/vypíná pomocí tlaèítka Toggle filtering.

Tlaèítko New v èásti pro filtrovací pravidla otevøe dialog, kde lze vytvoøit nové pravidlo. Pomocí tlaèítka Edit lze vybrané pravidlo upravit a pomocí Delete smazat.

=== Dialog pro editaci filtrovacích pravidel ===

Zde lze upravit název a definici filtrovacího pravidla. Definice pravidla nyní obsahuje holý SQL dotaz. Pøíklady:

	SELECT taskId FROM Task WHERE done = 0
	SELECT taskId FROM Tagged WHERE tagId = 11

Viz programátorskou dokumentaci, oddíl Databázové schéma.

Pokud je dotaz neplatný nebo obsahuje nìjakou chybu, filtrování se vypne.
