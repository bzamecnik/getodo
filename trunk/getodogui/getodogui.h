#include "getodo.h"
#include "tagmodel.h"
#include "taskmodel.h"

// ----- class TagWindow --------------------

// TagWindow for testing tag TreeView
class TagWindow : public Gtk::Window
{
public:
	TagWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~TagWindow();

	void setTaskManager(getodo::TaskManager* manager);
protected:
	Glib::RefPtr<Gnome::Glade::Xml> refXml;
	Glib::RefPtr<getodo::TagTreeModel> refTreeModel;

	Gtk::TreeView* pTreeView;
	Gtk::Entry* pEntryId;
	Gtk::Entry* pEntryName;

	void on_buttonAdd_clicked();
	void on_buttonEdit_clicked();
	void on_buttonDelete_clicked();
	void on_buttonDebug_clicked();
	void on_treeview_cursor_changed();
private:
	getodo::TaskManager* taskManager;
	Gtk::TreeModel::iterator getCursorIter(Gtk::TreeView* pTreeView);
};

// ----- class TaskWindow --------------------

// TaskWindow for testing task TreeView
class TaskWindow : public Gtk::Window
{
public:
	TaskWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~TaskWindow();

	void setTaskManager(getodo::TaskManager* manager);
protected:
	Glib::RefPtr<Gnome::Glade::Xml> refXml;
	Glib::RefPtr<getodo::TaskTreeModel> refTreeModel;

	Gtk::TreeView* pTreeView;
	Gtk::Entry* pEntryId;
	Gtk::Entry* pEntryName;

	void on_buttonAdd_clicked();
	void on_buttonEdit_clicked();
	void on_buttonDelete_clicked();
	void on_buttonDebug_clicked();
	void on_treeview_cursor_changed();
private:
	getodo::TaskManager* taskManager;
	Gtk::TreeModel::iterator getCursorIter(Gtk::TreeView* pTreeView);
};

// TODO: main window for real usage

// ----- class GeToDoApp --------------------

class GeToDoApp {
protected:
	TaskWindow* pWindow;
	Gtk::Main kit;
	//Glib::RefPtr<Gtk::TreeModelSort> refTreeModelSort;
public:
	GeToDoApp(int argc, char* argv[]);
	~GeToDoApp();
	void run();
private:
	getodo::TaskManager* taskManager;
};
