#include "getodo.h"
#include "tagmodel.h"
#include "taskmodel.h"

//// ----- class TagWindow --------------------
//
//// TagWindow for testing tag TreeView
//class TagWindow : public Gtk::Window
//{
//public:
//	TagWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
//	virtual ~TagWindow();
//
//	void setTaskManager(getodo::TaskManager* manager);
//protected:
//	Glib::RefPtr<Gnome::Glade::Xml> refXml;
//	Glib::RefPtr<getodo::TagTreeModel> refTreeModel;
//
//	
//	Gtk::Entry* pEntryId;
//	Gtk::Entry* pEntryName;
//
//	void on_buttonAdd_clicked();
//	void on_buttonEdit_clicked();
//	void on_buttonDelete_clicked();
//	void on_buttonDebug_clicked();
//	void on_treeview_cursor_changed();
//private:
//	getodo::TaskManager* taskManager;
//	Gtk::TreeModel::iterator getCursorIter(Gtk::TreeView* pTreeView);
//};

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
	// * add labels to describe the entry widgets
	// * change widget type from the dummy Entry
	Gtk::Entry* pTaskDescriptionEntry;
	Gtk::TextView* pTaskLongDescriptionTextView; // future: TextView
	Glib::RefPtr<Gtk::TextBuffer> refTaskLongDescriptionTextBuffer;
	Gtk::Entry* pTaskTagsEntry;
	//Gtk::CheckButton* pTaskDoneCheckbutton;
	Gtk::Entry* pTaskCompletedPercentageEntry; // future: SpinButton
	Gtk::Entry* pTaskPriorityEntry; // future: SpinButton or ComboBox
	Gtk::Label* pTaskIdLabel; // future: hidden
	Gtk::Entry* pTaskDateDeadlineEntry; // future: DateEntry (from GDA)
	Gtk::Entry* pTaskDateStartedEntry; // future: DateEntry (from GDA)
	Gtk::Entry* pTaskDateCompletedEntry; // future: DateEntry (from GDA)
	Gtk::Label* pTaskDateCreatedLabel;
	Gtk::Label* pTaskDateLastModifiedLabel;

	// filtering panel
	Gtk::TreeView* pTagTreeView;
	Glib::RefPtr<getodo::TagTreeModel> refTagTreeModel;
	//Glib::RefPtr<Gtk::TreeModelSort> refTagTreeModelSort;

	// ----- signal handlers -----
	//void on_buttonAdd_clicked();
	//void on_buttonEdit_clicked();
	//void on_buttonDelete_clicked();
	//void on_buttonDebug_clicked();
	void on_tasktreeview_cursor_changed();
private:
	getodo::TaskManager* taskManager;
	Gtk::TreeModel::iterator getCursorIter(Gtk::TreeView* pTreeView,
		Glib::RefPtr<Gtk::TreeModel> refTreeModel);
};

// TODO: main window for real usage

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
