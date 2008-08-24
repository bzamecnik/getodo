// getodogui.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "getodogui.h"

class ModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	ModelColumns()
	{
		add(m_col_id); 
		add(m_col_name);
	}
	Gtk::TreeModelColumn<int> m_col_id;
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
};

void tag_list_view(Gtk::TreeView& m_TreeView) {
	ModelColumns m_Columns;
	Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
	m_refTreeModel = Gtk::ListStore::create(m_Columns);
	m_TreeView.set_model(m_refTreeModel);

	using namespace getodo;
	TaskManager tm("test2.db");
	tm.addTag(Tag("meaning"));
	tm.addTag(Tag("one"));
	tm.addTag(Tag("life"));
	tm.addTag(Tag("photos"));
	tm.addTag(Tag("music"));

	std::list<Tag> tags = tm.getTagsList();
	Gtk::TreeModel::Row row;
	for (std::list<Tag>::iterator it = tags.begin(); it != tags.end(); ++it) {
		row = *(m_refTreeModel->append());
		row[m_Columns.m_col_id] = it->id;
		row[m_Columns.m_col_name] = it->name;
	}
	
	//Add the TreeView’s view columns:
	m_TreeView.append_column("Id", m_Columns.m_col_id);
	m_TreeView.append_column("Name", m_Columns.m_col_name);

}

int main(int argc, char* argv[])
{
	//Gtk::Main kit(argc, argv);
	//Gtk::Window window;
	//Gtk::Main::run(window);

	Gtk::Main kit(argc, argv);

	Glib::RefPtr<Gnome::Glade::Xml> refXml = Gnome::Glade::Xml::create("treeview.glade");
	Gtk::Window* pWindow = 0;
	Gtk::TreeView* pTreeView = 0;
	refXml->get_widget("window1", pWindow);
	refXml->get_widget("treeview1", pTreeView);
	if (pWindow && pTreeView) {
		tag_list_view(*pTreeView);

		kit.run(*pWindow);
//		delete pWindow;
//		pWindow = 0;
	}
	
	return 0;
}

