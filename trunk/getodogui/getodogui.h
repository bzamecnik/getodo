// $Id$

#ifndef GETODOGUI_GETODOGUI_H
#define GETODOGUI_GETODOGUI_H

#include "getodo.h"

class GeToDoApp;
class MainWindow;
class RecurrenceDialog;
class FilterDialog;

#include "main-window.h"
#include "recurrence-dialog.h"
#include "filter-dialog.h"

// ----- class GeToDoApp --------------------

/** GeToDo application.
 * Purpose of this class it to initialize the database and GUI stuff and run
 * the application. Then it holds all the window and dialog widgets from each
 * Glade file.
 * 
 * It is a singleton class, its constructed using create() function and its
 * instance is provided by by getSingleton() function.
 */
class GeToDoApp
{
public:
	~GeToDoApp();
	/** Run the application. */
	void run();
	/** Get singleton instance.
	 * Call it only after calling GeToDoApp::create().
	 *
	 * \return reference to GeToDoApp instance
	 */
	static GeToDoApp& getSingleton();
	/** Create an instance.
	 * The purpose to separated create() factory method is to provide
	 * command-line arguments (but only once) from main() function.
	 *
	 * \param argc number of arguments
	 * \param argv array of command-line arguments
	 * \return pointer to the new instance
	 */
	static GeToDoApp* create(int argc, char* argv[]);

	/** Get a widget by its name.
	 * Get a pointer to a widget loaded previously from a Glade file to a local cache.
	 *
	 * This is similar to Gnome::Glade::Xml::get_widget(const Glib::ustring& name,
	 * T_Widget *& widget) but uses a local cache of instantiated widgets.
	 *
	 * Call it only after calling GeToDoApp::create().
	 *
	 * \return pointer to the widget or 0 if something went wrong
	 */
	template <class TWidget>
	TWidget* getWidget(const std::string& name, TWidget*& widget) {
		std::map<std::string, Gtk::Widget*>::iterator iter = widgets.find(name);
		Gtk::Widget* pWidget = (iter != widgets.end()) ? iter->second : 0;
		return (pWidget != 0) ? dynamic_cast<TWidget*>(pWidget) : 0;
	}
protected:
	Gtk::Main kit;
	MainWindow* pWindow;
private:
	std::map<std::string, Gtk::Widget*> widgets;
	getodo::TaskManager* taskManager;
	static GeToDoApp* instance;

	GeToDoApp(int argc, char* argv[]);
};

#endif // GETODOGUI_GETODOGUI_H
