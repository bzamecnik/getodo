// $Id$

#include "stdafx.h"
#include "getodogui.h"

namespace getodo {

// ----- class GeToDoApp --------------------

GeToDoApp* GeToDoApp::instance = 0;

GeToDoApp::GeToDoApp(int argc, char* argv[])
	: kit(argc, argv), pWindow(0)
{
	// default file name should be: ~/getodo.db
	// Glib::build_filename(Glib::get_home_dir(), "getodo.db")
	std::string dbName("getodo.db");
	
	if (argc > 1) {
		dbName = argv[1];
	}
	
	try {
		taskManager = boost::shared_ptr<TaskManager>(new TaskManager(dbName));
	} catch (GetodoError& ex) {
		std::cerr << ex.what() << std::endl;
		exit(1);
	}
	
	RecurrenceDialog* pRecurrenceDialog = 0;
	FilterDialog* pFilterDialog = 0;

	Glib::RefPtr<Gnome::Glade::Xml> refXml;
	try {
		refXml = Gnome::Glade::Xml::create("main-window.glade");
		refXml->get_widget_derived("mainWindow", pWindow);
		//widgets["mainWindow"] = pWindow;

		refXml = Gnome::Glade::Xml::create("recurrence-dialog.glade");
		refXml->get_widget_derived("recurrenceDialog", pRecurrenceDialog);
		widgets["recurrenceDialog"] = pRecurrenceDialog;

		refXml = Gnome::Glade::Xml::create("filter-dialog.glade");
		refXml->get_widget_derived("filterDialog", pFilterDialog);
		widgets["filterDialog"] = pFilterDialog;
	} catch (Gnome::Glade::XmlError& ex) {
		std::cerr << "Error loading Glade GUI files: " << ex.what() << std::endl;
		exit(1);
	}
	pWindow->setTaskManager(taskManager);
}

GeToDoApp::~GeToDoApp() {
	// TODO: the top-level widget should be deleted, but it give some errors... :(
	//
	//if (pWindow != 0) {
	//	delete pWindow;
	//}
	//std::pair<std::string, Gtk::Widget*> pair;
	//foreach(pair, widgets) {
	//	if (pair.second != 0) {
	//		delete pair.second;
	//	}
	//}
}

GeToDoApp* GeToDoApp::create(int argc, char* argv[]) {
	if (instance == 0) {
		instance = new GeToDoApp(argc, argv);
	}
	return instance;
}

GeToDoApp& GeToDoApp::getSingleton() {
	// assert(instance);
	return *instance;
}

void GeToDoApp::run() {
	if (pWindow) {	
		kit.run(*pWindow);
	}
}

} // namespace getodo

int main(int argc, char* argv[]) {
	using namespace getodo;

	//std::cout << Glib::locale_from_utf8(Glib::get_home_dir()) << std::endl;
	//Gtk::MessageDialog(Glib::get_home_dir()).run();
	
	// Glib::build_filename(Glib::get_home_dir(), "getodo.db")

	GeToDoApp *app;
	try {
		app = GeToDoApp::create(argc, argv);
		app->run();
	} catch(const Glib::Error& ex) {
		std::cerr << ex.what() << std::endl;
	} catch(const sqlite3x::database_error& ex) {
		//std::cerr << ex.what() << std::endl;
		Gtk::MessageDialog(ex.what(), false, Gtk::MESSAGE_ERROR).run();
	} catch(const std::exception& ex) {
		//std::cerr << ex.what() << std::endl;
		Gtk::MessageDialog(ex.what(), false, Gtk::MESSAGE_ERROR).run();
	}
	delete app;
	return 0;
}
