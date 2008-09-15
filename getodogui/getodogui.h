#include "getodo.h"
#include "tagmodel.h"
#include "taskmodel.h"

// ----- class MainWindow --------------------

// MainWindow for testing task TreeView
class MainWindow : public Gtk::Window
{
public:
	MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~MainWindow();

	void setTaskManager(getodo::TaskManager* manager);
protected:
	Glib::RefPtr<Gnome::Glade::Xml> refXml;
	
	// ----- child widgets ----

	// task treeview
	Gtk::TreeView* pTaskTreeView;
	Glib::RefPtr<getodo::TaskTreeModel> refTaskTreeModel;
	//Glib::RefPtr<Gtk::TreeModelSort> refTaskTreeModelSort;

	// task editing panel
	// TODO:
	// * change some widget's type from the dummy Entry
	Gtk::Entry* pTaskDescriptionEntry;
	Gtk::TextView* pTaskLongDescriptionTextView;
	Glib::RefPtr<Gtk::TextBuffer> refTaskLongDescriptionTextBuffer;
	Gtk::Entry* pTaskTagsEntry;
	Gtk::CheckButton* pTaskDoneCheckbutton;
	Gtk::SpinButton* pTaskCompletedPercentageSpinbutton;
	Gtk::SpinButton* pTaskPrioritySpinbutton; // SpinButton or ComboBox, colorful at best
	//Gtk::Label* pTaskRecurrenceLabel;
	Gtk::Entry* pTaskRecurrenceEntry;
	Gtk::Label* pTaskIdLabel;
	Gtk::Entry* pTaskDateDeadlineEntry; // future: DateEntry (from GDA)
	Gtk::Entry* pTaskDateStartedEntry; // future: DateEntry (from GDA)
	Gtk::Entry* pTaskDateCompletedEntry; // future: DateEntry (from GDA)
	Gtk::Label* pTaskDateCreatedLabel;
	Gtk::Label* pTaskDateLastModifiedLabel;

	// filtering panel
	Gtk::TreeView* pTagTreeView;
	Glib::RefPtr<getodo::TagTreeModel> refTagTreeModel;
	//Glib::RefPtr<Gtk::TreeModelSort> refTagTreeModelSort;

	// menubar

	// toolbar

	// ----- signal handlers -----
	void on_buttonTaskNew_clicked();
	void on_buttonTaskDelete_clicked();
	void on_buttonTaskUpdate_clicked();
	//void on_buttonEdit_clicked();
	//void on_buttonDelete_clicked();
	//void on_buttonDebug_clicked();
	void on_taskTreeview_selection_changed();
private:
	getodo::TaskManager* taskManager;
	//Gtk::TreeModel::iterator getCursorIter(Gtk::TreeView* pTreeView,
	//	Glib::RefPtr<Gtk::TreeModel> refTreeModel);

	void MainWindow::fillEditingPanel(getodo::Task& task);
	void MainWindow::clearEditingPanel();
	void MainWindow::saveEditingPanelToTask(getodo::Task& task);
};

// ----- class GeToDoApp --------------------

class GeToDoApp {
protected:
	MainWindow* pWindow;
	Gtk::Main kit;
public:
	GeToDoApp(int argc, char* argv[]);
	~GeToDoApp();
	void run();
private:
	getodo::TaskManager* taskManager;
};
