====== GeToDo - u�ivatelsk� dokumentace ======

Program GeToDo m� za c�l pomoci �lov�ku p�i organizaci osobn�ch �kol� a projekt�.

===== Instalace =====

Nejd��ve je t�eba nainstalovat runtime knihoven GTK a Gtkmm:
	- gtk-2.12.9-win32-2.exe
	- gtkmm-win32-runtime-2.10.11-1.exe

Pak je mo�no nainstalovat program GeToDo:
	- getodo-setup-<verze>.exe

Pozn�mka: Knihovny GTK vyu��vaj� prom�nn� prost�ed� GTK_BASEPATH a GTKMM_BASEPATH. Z�rove� jejich adres�� bin mus� b�t v prom�nn� PATH. Instal�tor by to m�l nastavit automaticky. Zpravidla se se zm�ny prom�tnou hned, ale je mo�n�, �e je ob�as t�eba restart syst�mu. Nap��klad b��c� instance Total Commanderu nebo p��kazov� ��dky Windows si zm�ny nena�te, ale p�i spu�t�n� GeToDo z nab�dky Start funguje, nebo� Windows Explorer si asi PATH obnov� s�m.

===== Odinstalov�n� =====

V nab�dce Start je odkaz na program uninstall.exe, kter� provede odinstalov�n�. U�ivatelsk� soubory z�stanou nedot�eny.

===== Spu�t�n� =====

Program GeToDo je mo�no spustit kliknut�m na odkaz, kter� se vytvo�il v nab�dce Start->GeToDo, nebo p��mo na soubor getodo.exe v adres��i, kam se program nainstaloval (nap�. C:\Program Files\getodo\getodo.exe).

Pro ulo�en� informac� pou��v� program GeToDo datab�zi SQLite ulo�enou v souboru (s p��ponou .db). P�i spu�t�n� bez parametr� se automaticky pou�ije soubor getodo.db v aktu�ln�m adres��i (nap�. C:\program files\getodo\getodo.db). Chcete-li pou��t jin� soubor, zadejte cestu k n�mu jako parametr p�i spu�t�n� pogramu GeToDo.

===== Ovl�d�n� =====

Po spu�t�n� se objev� okno, kter� se skl�d� z n�kolika ��st�:
	- stromov� p�ehled �kol�
	- panel pro editaci �kol�
	- panel s funk�n�mi tla��tky
	- filtrovac� panel
	
Kontextov� n�pov�da funguje u v�t�iny ovl�dac�ch prvk�. Zobraz� se, jakmile najedete kurzorem na dan� prvek a chv�li jej podr��te na m�st�.

==== Panel s funk�n�mi tla��tky ====

- New top-level task
	- vytvo�� nov� �kol na nejvy��� �rovni (takov� �kol nen� pod�kolem jin�ho)
- New task on the same level
	- vytvo�� nov� �kol na stejn� �rovni jako je �kol pr�v� ozna�en� v p�ehledu �kol�
	- tagy se zkop�ruj� z p�edka, pokud nov� �kol n�jak�ho m� 
	- pokud ��dn� ozna�en� nen�, vytvo�� �kol na nejvy��� �rovni
- New subtask
	- vytvo�� nov� �kol jako pod�kol �kolu pr�v� ozna�en�ho v p�ehledu �kol�
	- tagy se zkop�ruj� z p�edka, pokud nov� �kol n�jak�ho m�
	- pokud ��dn� ozna�en� nen�, vytvo�� �kol na nejvy��� �rovni
- Delete task
	- vyma�e �kol pr�v� ozna�en� v p�ehledu �kol� a tak� v�echny jeho potomky
	- nejd��ve se zept�
- Generate next task by recurrence
	- vytvo�� novou instanci �kolu vybran�ho a nastav� nov� datum deadline podle definice opakov�n�
- Toggle filtering
	- zapne/vypne pr�v� aktivn� filtr (viz sekci Filtrovac� panel)

==== Stromov� p�ehled �kol� ====

Zde se zobrazuje p�ehled �kol� uspo��dan� do stromu.

Zobrazeno je n�kolik sloupc�:
	- Id - ��seln� identifik�tor �kolu
	- Description - popis �kolu
	- Done - p��znak, zda-li je �kol hotov
	- % - kolik procent je hotovo
	- ! - priorita
	- Deadline - datum, dokdy m� b�t �kol hotov 

Kliknut�m na ��dek �kolu se vybran� �kol otev�e v panelu pro editaci �kolu.

�koly v stromu je mo�n� �adit podle zobrazen�ch sloupc�, a to kliknut�m na hlavi�ku sloupce. Je mo�no p�ep�nat mezi t�emi re�imy (vzestupn�, sestupn�, vypnuto).

Ozna�it lze pouze jeden �kol. CTRL-kliknut� na vybran� �kol jej odzna��.

Pokud je aktivn� n�jak� filtr, zobraz� se v p�ehledu jen vyfiltrovan� �koly.

==== Panel pro editaci �kol� ====

Zde se zobrazuje obsah �kolu pr�v� vybran�ho v p�ehledu �kol� a je ho mo�n� upravovat. �prava se obecn� ulo�� do datab�ze a� p�i opu�t�n� pr�v� upravovan�ho pol��ka.

- Title - nadpis �kolu
- Description - del�� popis
- Tags
	- seznam tag� odd�len� ��rkami, tagy mohou obsahovat mezery
	- tagy slou�� jako kategorie, jen je mo�no �kolu p�i�adit tag� v�ce najednou
- Done - hotovo
- % - kolik procent je hotovo (0-100)
- Priority - ��seln� priorita (0-10)
- Recurrence - opakov�n� �kolu
	- je mo�n� opakov�n� upravit pomoc� dialogu pro editaci opakov�n� nebo p��mo zapsat v textov�m tvaru
- Id - ��seln� identifikace
- Deadline - datum, kdy m� b�t �kol hotov - form�t RRRR-MM-DD
- Start - datum, kdy byla zapo�ata pr�ce na �kolu - form�t RRRR-MM-DD
- Completed - datum, kdy byl �kol spln�n - form�t RRRR-MM-DD
- Created - datum, kdy byl �kol vytvo�en - form�t RRRR-MM-DD HH:MM:SS
- Last Modified - datum, kdy byl �kol naposledy upraven - form�t RRRR-MM-DD HH:MM:SS

=== Dialog pro editaci opakov�n� �kol� ===

Opakov�n� �kolu (rekurence) je nastaven�, jak�m zp�sobem se m� opakovat v�skyt �kolu (konkr�tn� datum deadline). Je n�kolik mo�nost�, jak m� opakov�n� vypadat.

Dal�� v�skyt �kolu je mo�n� vytvo�it pomoc� tla��tka Generate next task by recurrence.

Opakov�n� m� i textov� z�pis. Po editaci v dialogu se zobraz� v p��slu�n�m pol��ku, ale je mo�n� jej editovat i p��mo. P�i chyb� se automaticky nastav� opakov�n� Once.

- Once - opakov�n� pouze jednou
	- ��dn� dal�� v�skyt nen�
	- textov� z�pis: <pr�zdn� �et�zec>
- Daily - opakov�n� ka�d� n-t� den
	- textov� z�pis: D <perioda v dnech>, p�. D 2
- Weekly - opakov�n� ka�d� n-t� t�den
	- existuj� dva re�imy:
	- ka�d� n-t� t�den od aktu�ln�ho data deadline
		- textov� z�pis: W <perioda v t�dnech>, p�. W 3
	- ka�d� n-t� t�den po nastaven�ch dnech v t�dnu
		- textov� z�pis: W <perioda v t�dnech> <vybran� dny>, p�. W 1 Mon Thu Fri
			- vybran� dny se zapisuj� pomoc� t��p�smenn�ch zkratek anglick�ch n�zv� dn� v t�dnu
- Monthly - opakov�n� ka�d� n-t� m�s�c
	- existuj� dva re�imy:
	- ka�d� n-t� m�s�c od aktu�ln�ho data deadline
		- textov� z�pis: M <perioda v m�s�c�ch>, p�. M 1
	- ka�d� n-t� m�s�c v nastaven� den v m�s�ci
		- textov� z�pis: M <perioda v m�s�c�ch> <den v m�s�ci>, p�. M 1 15
- Yearly - opakov�n� ka�d� rok
	- existuj� dva re�imy:
		- ka�d� rok od aktu�ln�ho data deadline
			- textov� z�pis: Y
		- ka�d� rok v nastaven� den a m�s�c
			- textov� z�pis: M <den v m�s�ci> <m�s�c>, p�. M 3 Feb
				- m�s�c se zapisuje pomoc� t��p�smenn� zkratky anglick�ho n�zv� m�s�ce
- Interval of days - ka�d� den v intervalu mezi dv�ma daty
	- textov� z�pis: I [<datum za��tku>/<datum konce>]
		- p�. I [2008-Jan-02/2009-Mar-14]
	- datum se zapisuje ve form�tu RRRR-MM-DD nebo not-a-date-time
	- pokud se jako hranice uvede not-a-date-time

==== Filtrovac� panel ====

�koly je mo�no filtrovat, a to hned dv�ma zp�soby - pomoc� tag� a/nebo pomoc� filtrovac�ch pravidel. Filtrovac� panel obsahuje proto dv� ��sti - seznam tag� a seznam pravidel. Tla��tka All a Any zna��, jak se maj� spojit vybran� polo�ky seznam� (pr�nikem resp. sjednocen�m). Polo�ek je mo�no vybrat/odzna�it pomoc� CTRL-kliknut�.

Tla��tkem Filter by tags se zap�n�/vyp�n� filtrov�n� pomoc� tag�. Tla��tkem Filter by rules se zap�n�/vyp�n� filtrov�n� pomoc� filtrovac�ch pravidel. Dohromady jsou tyto dv� ��sti spojeny pr�nikem.

Celkov� se filtrov�n� zap�n�/vyp�n� pomoc� tla��tka Toggle filtering.

Tla��tko New v ��sti pro filtrovac� pravidla otev�e dialog, kde lze vytvo�it nov� pravidlo. Pomoc� tla��tka Edit lze vybran� pravidlo upravit a pomoc� Delete smazat.

=== Dialog pro editaci filtrovac�ch pravidel ===

Zde lze upravit n�zev a definici filtrovac�ho pravidla. Definice pravidla nyn� obsahuje hol� SQL dotaz. P��klady:

	SELECT taskId FROM Task WHERE done = 0
	SELECT taskId FROM Tagged WHERE tagId = 11

Viz program�torskou dokumentaci, odd�l Datab�zov� sch�ma.

Pokud je dotaz neplatn� nebo obsahuje n�jakou chybu, filtrov�n� se vypne.
