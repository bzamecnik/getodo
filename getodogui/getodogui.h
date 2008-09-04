#include "getodo.h"
#include "tagmodel.h"

class GeToDoApp {
// TODO: make a separate class for window
protected:
	Gtk::Window* pWindow;
	Gtk::TreeView* pTreeView;
	Gtk::Entry* pEntryId;
	Gtk::Entry* pEntryName;
	Gtk::Main kit;
	Glib::RefPtr<getodo::TagTreeModel> refTreeModel;
	//Glib::RefPtr<Gtk::TreeModelSort> refTreeModelSort;
public:
	GeToDoApp(int argc, char* argv[]);
	~GeToDoApp();
	void run();
//	void on_treeview_cursor_changed();
	void on_buttonAdd_clicked();
	void on_buttonEdit_clicked();
	void on_buttonDelete_clicked();
	void on_buttonDebug_clicked();
	void on_treeview_cursor_changed();
private:
	Gtk::TreeModel::iterator getCursorIter(Gtk::TreeView* pTreeView);
	getodo::TaskManager* taskManager;
};
