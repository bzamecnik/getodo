// $Id$

#include "stdafx.h"
#include "getodogui.h"

// ----- class GeToDoApp --------------------

GeToDoApp* GeToDoApp::instance = 0;

GeToDoApp::GeToDoApp(int argc, char* argv[])
	: kit(argc, argv), pWindow(0), pRecurrenceDialog(0)
{
	using namespace getodo;
	if (argc <= 1) {
		std::cerr << "Usage: " << argv[0] << " DATABASE_FILE" << std::endl;
		exit(1);
	}
	taskManager = new TaskManager(argv[1]);

	Glib::RefPtr<Gnome::Glade::Xml> refXml;
	try {
		refXml = Gnome::Glade::Xml::create("main-window.glade");
		refXml->get_widget_derived("mainWindow", pWindow);
		refXml = Gnome::Glade::Xml::create("recurrence-dialog.glade");
		refXml->get_widget_derived("recurrenceDialog", pRecurrenceDialog);
	} catch (Gnome::Glade::XmlError& e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
	pWindow->setTaskManager(taskManager);
}

GeToDoApp::~GeToDoApp() {
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
		// TODO: Find out, whether to explicitly delete widget created by Glade.
		
		// delete pWindow;
		// pWindow = 0;
	}
}

RecurrenceDialog& GeToDoApp::getRecurrenceDialog() {
	return *pRecurrenceDialog;
}

int main(int argc, char* argv[]) {
	GeToDoApp *app;
	try {
		app = GeToDoApp::create(argc, argv);
		app->run();
	} catch(const Glib::Error& ex) {
		std::cerr << ex.what() << std::endl;
	} catch(const sqlite3x::database_error& ex) {
		std::cerr << ex.what() << std::endl;
	} catch(const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
	delete app;
	return 0;
}
