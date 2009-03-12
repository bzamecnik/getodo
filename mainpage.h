/**
* \mainpage
* \section architecture Architecture (Czech)
* O architektuře programu GeToDo se můžete dočíst na samostatné stránce
* \ref architecture-cs .
* 
* \page architecture-cs Architektura
*
* Projekt GeToDo je koncipován jako desktopová aplikace. Je rozdělen do několika modulů:
*
*   - základní knihovna Libgetodo
*   - grafické uživatelské rozhraní Getodogui
*   - embedded databáze	SQLite  
* 
* Libgetodo zajišťuje objektovou reprezentaci základních typů a jejich databázovou persistenci.
* 
* Getodogui využívá funcionality Libgetodo a poskytuje k ní GUI.
* 
* Schéma databáze je možno nalézt v souboru doc/sql/getodo-sqlite.sql. ER-diagram v doc/sql/diagram.png nebo na samostatné stránce \ref database-scheme .
* 
* Celý projekt je psán v jazyce C++ a buildován pomocí MS Visual Studia.
* Použité knihovny:
* 
*   - Boost - utility
*   - SQLite - embedded databáze
*   - SQLite3x - C++ wrapper nad SQLite
*   - GTK - GUI
*   - Gtkmm	- C++ wrapper nad GTK
* 
* Poznámka: Zdrojové kódy knihovny SQLite3x jsou zahrnuty do stromu projektu GeToDo z jednoho prostého důvodu: SQLite3x je v originálně buildována pomocí dost obskurně řešeného Makefile a v MinGW na Windows se mi ji nepodařilo samostatně sestavit. Nicméně poté, co jsem pro ni vytvořil projekt ve Visual Studiu, lze sestavil bez problémů. Proto bylo technicky jednoduší ji zahrnout přímo do stromu zdrojových kódů projektu GeToDo.
* 
* \section Libgetodo
* 
* Třídy v tomto modulu náleží do namespace getodo.
* 
* \subsection Task
* 
* Datová třída reprezentující jeden úkol. Odpovídá řádku v SQL tabulce Tasks, ale navíc umožňuje práci s tagy, nadřazenými úkoly i podúkoly. Změny na instanci této třídy se neprojeví přímo v databázi, to tomu slouží třída TaskPersistence. Z toho důvodu změny na instanci třídy Task se hodí spíš pro hromadné úpravy, než úpravy jednotlivých položek. Na task může být často odkazováno pomocí jeho identifikátoru z databáze. 
* 
* Třída task poskytuje rovněž i jednoduché objektově relační mapování:
*   - toDatabaseRow() - převod instance Task na řádek tabulky Tasks
*   - fromDatabaseRow()	- tovární metoda, jež vytvoří instanci Task podle řádku z tabulky Tasks
* 
* Je zde také komfortnější práce s tagy - lze je číst a nastavovat nejen podle identifikátoru, ale i podle řetezce s názvy tagů.	
* 
* \subsection TaskPersistence
* 
* Persistence pro třídu Task realizující objektově-relační mapování. Uvnitř si drží odkaz na task a databázové připojení. Pracuje s databázovovou knihovnou SQLite3x a nabízí základní operace jako načtení úkolu, vložení, úpravu a vymazání. Navíc je možné provádět úpravy pouze některých položek, takže není nutné např. pro přepnutí příznaku, zda-li je úkol splněn, přepisovat kompletně celý úkol i se všemi tagy a podúkoly.
* 
* Instance této třídy pro konkrétní úkol se vytváří nejčastěji tovární metodou getPersistentTask() třídy TaskManager. Je však možné do ní po použití vložit odkaz na jiný úkol a provádět persistenci s ním. 
* 
* \subsection Tag
* 
* Datová třída reprezentující jeden tag. Tag má podobně jako v jiných aplikacích význam kategorie, avšak jednomu úkolu je možno přiřadit libovolně mnoho tagů.
* 
* \subsection TagPersistence
* 
* Persistence pro Tag. Má podobný význam i použití jako TaskPersistence, avšak neumožňuje individuální úpravu položek (nemělo by to ani velký smysl).
* 
* \subsection TaskManager
* 
* Důležitá třída, která funguje jako objektová reprezentace údajů z databáze, ale navíc poskytuje velkou funkcionalitu. Uvnitř obsahuje zejména připojení do databáze a kolekce tasků, tagů a filtrovacích pravidel. Ve svém rozhraní má operace s tasky, tagy i filtry. Kromě toho umožňuje nastavit aktivní filtrovací pravidlo a podle něj filtrovat tasky.
* 
* Jelikož instance TaskManageru se přímo používá v Getodogui a to nutně potřebuje dostávat informace, že se právě něco změnilo, obsahuje TaskManager i signály pro reportování změn. Používají se signály knihovny Sigc++ z GTK. Například je-li přidán nový task, GUI se to ihned dozví a může vykreslit nový řádek ve svém seznamu tasků.
* 
* Pokud je databáze ještě neinicializovaná, TaskManager vytvoří sktrukturu jejích tabulek.
* 
* \subsection FilterRule
* 
* Datová třída reprezentující pravidlo pro filtrování úkolů. Jedinou zásadní metodou je filter(), která provede filtrování a vrátí množinu úkolů, které vyhovují.
* 
* \subsection FilterRulePersistence
* 
* Persistence pro FilterRule. Funguje podobně jako TaskPersistence. Poskytuje metody pro individuální úpravu datových položek filtrovacícho pravidla.
* 
* \subsection FilterBuilder
* 
* Tovární třída pro výrobu filtrovacích pravidel.
* 
* \subsection Date
* 
* Datová třída reprezentující datum (bez času). Funguje jako obal nad boost::gregorian::date. Důvodem k použití vlastního obalu byla serializace s vlastním formátem stringu.
* 
* \subsection DateTime
* 
* Datová třída reprezentující datum s časem. Funguje jako obal nad boost::posix_time::ptime. Důvodem k použití vlastního obalu byla stejně jako u Date serializace s vlastním formátem stringu.
* 
* \subsection Duration
* 
* Datová třída reprezentující dobu trvání úkolu. (FIXME - zatím neimplementováno.)
* 
* \subsection Recurrence
* 
* Abstraktní datová třída reprezentující opakování úkolu. Pomocí metody next() vrátí příští datum výskytu po zadaném datu. (De)serializace řeší i typ rekurence. Různé implementace jsou ve třídách:
* 
*   - RecurrenceDaily
*   - RecurrenceIntervalDays
*   - RecurrenceMonthly
*   - RecurrenceOnce
*   - RecurrenceWeekly
*   - RecurrenceYearly
* 
* \subsection GetodoError
* 
* Obecná výjimka v programu GeToDo.
* 
* 
* \section Getodogui
* 
* GUI pro GeToDo vytvořené s pomocí toolkitu GTK a jeho C++ wrapperu Gtkmm. Pro stavbu GUI se používá nástroj Glade (součást GTK), pomocí kterého se vytvoří XML soubor s definicí GUI. Tento XML soubor se pak za běhu načte a instanciují se z něj jednotlivé widgety.
* 
* Eventy se řeší pomocí signálové knihovny Sigc++ (distribuované jako součást balíku GTK).
* 
* \subsection GeToDoApp
* 
* Singleton třída reprezentující aplikaci GeToDo. Zpracovává argumenty z příkazové řádky, vytváří instanci TaskManageru, natahuje Glade soubory, z nichž se za běhu vytvoří GUI a to spouští.
* 
* \subsection MainWindow
* 
* Hlavní okno aplikace, potomek Gtk::Window. Asi nejdůležitějšími prvky jsou TreeView pro úkoly, jedno ListView pro filtry a druhé pro tagy. Každý tento view má pod sebou ještě model. Modely jsou propojeny s TaskManagerem právě pomocí výše zmíněných signálů. Praxi to vypadá tak, že když se něco změní v TaskManageru, ten vyšle signál, který pak zpracuje model. Ten zase automaticky vyšle signál pohledu, tj. TreeView nebo ListView.
* 
* \subsection RecurrenceDialog
* 
* Dialog pro editaci rekurencí.
* 
* \subsection RecurrenceDialog::MonthsModelColumns
* 
* \subsection TaskTreeStore
* 
* Model pro widget taskTreeView. Jedná se o poděděný TreeStore. Tuto možnost jsem zvolil, jednak abych mohl zpracovávat signály z TaskManageru, ale abych mohl mít k dispozici i další funkce. Původně myšlená vlastní implementace rozhraní TreeModel, kde by byla optimalizována manipulace s tasky v modelu, se ukázala být příliš složitá, těkžopádná.
* 
* \subsection FilterListStore
* 
* Jednoduchý obal nad ListStore. Funguje podobně jako TaskTreeStore, jen je jednodušší, protože nemusí řešit stromovou strukturu.
* 
* \subsection TagListStore
* 
* Skoro to samé jako FilterListStore.
* 
* 
* \page database-scheme Database scheme
* 
* \image html doc/sql/er-diagram.png
*
* \include doc/sql/getodo-sqlite.sql
*/
