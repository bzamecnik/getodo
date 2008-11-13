// $Id$

#ifndef GETODOGUI_GETODOGUI_H
#define GETODOGUI_GETODOGUI_H

#include "getodo.h"

class GeToDoApp;
class MainWindow;
class RecurrenceDialog;

#include "main-window.h"
#include "recurrence-dialog.h"
//#include "tagmodel.h"
//#include "taskmodel.h"

// ----- class BaseWindow --------------------

// ----- class GeToDoApp --------------------

/** GeToDo application.
 */
class GeToDoApp
{
// a singleton
protected:
	Gtk::Main kit;
	MainWindow* pWindow;
	RecurrenceDialog* pRecurrenceDialog;
public:
	~GeToDoApp();
	void run();
	static GeToDoApp& getSingleton();
	static GeToDoApp* create(int argc, char* argv[]);

	RecurrenceDialog& getRecurrenceDialog(); // TODO: a bit ugly
private:
	getodo::TaskManager* taskManager;
	static GeToDoApp* instance;

	GeToDoApp(int argc, char* argv[]);
};

#endif // GETODOGUI_GETODOGUI_H
