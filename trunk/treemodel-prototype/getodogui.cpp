#include "stdafx.h"
#include "getodogui.h"

GeToDoApp::GeToDoApp(int argc, char* argv[])
	: kit(argc, argv), pWindow(0), pTreeView(0)
{
	Glib::RefPtr<Gnome::Glade::Xml> refXml = Gnome::Glade::Xml::create("treeview.glade");
	refXml->get_widget("window1", pWindow);
	refXml->get_widget("treeview1", pTreeView);
	refXml->get_widget("entryId", pEntryId);
	refXml->get_widget("entryName", pEntryName);
	refXml->get_widget("buttonAdd", pButtonAdd);
	refXml->get_widget("buttonEdit", pButtonEdit);
	refXml->get_widget("buttonDelete", pButtonDelete);
	refXml->get_widget("buttonDebug", pButtonDebug);

	if (!pWindow || !pTreeView) { return; }
	
	m_refTreeModel = TagTreeModel::create(tagManager);
	pTreeView->set_model(m_refTreeModel);

	pTreeView->append_column("Id", m_refTreeModel->columns.id);
	pTreeView->append_column("Name", m_refTreeModel->columns.name);

	pTreeView->signal_cursor_changed().connect(sigc::mem_fun(*this,
		&GeToDoApp::on_treeview_cursor_changed) );
	
	pButtonAdd->signal_clicked().connect(sigc::mem_fun(*this, &GeToDoApp::on_buttonAdd_clicked) );
	pButtonEdit->signal_clicked().connect(sigc::mem_fun(*this, &GeToDoApp::on_buttonEdit_clicked) );
	pButtonDelete->signal_clicked().connect(sigc::mem_fun(*this, &GeToDoApp::on_buttonDelete_clicked) );
	pButtonDebug->signal_clicked().connect(sigc::mem_fun(*this, &GeToDoApp::on_buttonDebug_clicked) );
}

GeToDoApp::~GeToDoApp() {
}

void GeToDoApp::run() {
	if (pWindow) {	
		kit.run(*pWindow);
		// delete pWindow;
		// pWindow = 0;
	}
}

void GeToDoApp::on_treeview_cursor_changed()
{
	Gtk::TreeModel::iterator iter = getCursorIter(pTreeView);
	if (iter) {
		Gtk::TreeModel::Row row = *iter;
		// TODO: boost::lexical_cast
		std::ostringstream ss;
		ss << row[m_refTreeModel->columns.id];
		pEntryId->set_text(ss.str());
		pEntryName->set_text(row[m_refTreeModel->columns.name]);
	}
}

void GeToDoApp::on_buttonAdd_clicked() {
	// TODO: boost::lexical_cast
	std::istringstream ss(pEntryId->get_text().c_str());
	int col_id;
	ss >> col_id;
	Tag* newTag = new Tag(col_id,pEntryName->get_text().c_str());
	tagManager.insertTag(newTag);
}

void GeToDoApp::on_buttonEdit_clicked() {
	std::istringstream ss(pEntryId->get_text().c_str());
	int col_id;
	ss >> col_id;
	Tag* tag = new Tag(col_id,pEntryName->get_text().c_str());
	tagManager.updateTag(tag);
}

void GeToDoApp::on_buttonDelete_clicked() {
	Gtk::TreeModel::iterator iter = getCursorIter(pTreeView);
	if (iter) { // TODO: check iter validity
		int col_id = iter->get_value(m_refTreeModel->columns.id);
		tagManager.removeTag(col_id);
	}
	// set cursor to next item (or previous, if no next exists)
}

void GeToDoApp::on_buttonDebug_clicked() {
	std::cerr << "TagManager:" << std::endl;
	std::list<Tag*> tags = tagManager.getTagsList();
	for (std::list<Tag*>::iterator it = tags.begin();
		it != tags.end(); ++it) {
		std::cerr << (*it)->toString() << std::endl;
	}
}
//
//// NOTE: changes in tree store can be back-propagated to the underlying data structre
//// using these signal handlers:
//// * row_changed
//// * row_deleted
//// * row_inserted
//// * rows_reordered
////
//// Changes in opposite direction (data structure -> tree store) will be made in a proxy
//// Eg.: TaskManager->TaskManagerProxy
//// data changed in TaskManagerProxy, which changes it in TaskManager
//// and also in the tree store (which in turn signals to the tree view)
//void GeToDoApp::on_treemodel_row_changed(const Gtk::TreeModel::Path& path,
//										 const Gtk::TreeModel::iterator& iter) {
//	if (iter) {
//		Gtk::TreeModel::Row row = *iter;
//		std::cout << "Changed: [" << path.to_string() << "] " << row[m_Columns.m_col_name] << std::endl;
//	}
//}
//
//void GeToDoApp::on_treemodel_row_deleted(const Gtk::TreeModel::Path& path) {
//	std::cout << "Deleted: [" << path.to_string() << "]" << std::endl;
//}
//
//void GeToDoApp::on_treemodel_row_inserted(const Gtk::TreeModel::Path& path,
//										 const Gtk::TreeModel::iterator& iter) {
//	if (iter) {
//		Gtk::TreeModel::Row row = *iter;
//		std::cout << "Inserted: [" << path.to_string() << "] " << row[m_Columns.m_col_name] << std::endl;
//	}
//}
//
//void GeToDoApp::insert_row(DataStorage::value_type item) {
//	if (m_refTreeModel) {
//		Gtk::TreeModel::Row row = *(m_refTreeModel->append());
//		row[m_Columns.m_col_name] = item;
//	}
//}

Gtk::TreeModel::iterator GeToDoApp::getCursorIter(Gtk::TreeView* pTreeView) {
	Gtk::TreeModel::iterator iter;
	if (pTreeView) {
		Gtk::TreeModel::Path path;
		Gtk::TreeViewColumn* column;
		pTreeView->get_cursor(path, column);
		if (path.gobj()){
			iter = m_refTreeModel->get_iter(path);
		}
	}
	return iter;
}

int main(int argc, char* argv[])
{
	GeToDoApp app(argc, argv);
	app.run();
	return 0;
}
