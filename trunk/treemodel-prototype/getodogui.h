#include <iostream>
#include <list>
#include <set>
#include <string>
#include <map>
#include <sstream>
#include "TagModel.h"

class GeToDoApp {
protected:
	Gtk::Window* pWindow;
	Gtk::TreeView* pTreeView;
	Gtk::Entry* pEntryId;
	Gtk::Entry* pEntryName;
	Gtk::Button* pButtonAdd;
	Gtk::Button* pButtonEdit;
	Gtk::Button* pButtonDelete;
	Gtk::Button* pButtonDebug;
	Gtk::Main kit;
	Glib::RefPtr<TagTreeModel> m_refTreeModel;
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
private:
	TagManager tagManager;
};