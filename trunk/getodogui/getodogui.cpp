#include "stdafx.h"
#include "getodogui.h"

GeToDoApp::GeToDoApp(int argc, char* argv[])
	: kit(argc, argv), pWindow(0), pTreeView(0), pEntryId(0), pEntryName(0)
{
	using namespace getodo;
	if (argc > 1) {
		taskManager = new TaskManager(argv[1]);
	}

	Glib::RefPtr<Gnome::Glade::Xml> refXml;
	try {
		refXml = Gnome::Glade::Xml::create("treeview.glade");
		refXml->get_widget("window1", pWindow);
		refXml->get_widget("treeview1", pTreeView);
		refXml->get_widget("entryId", pEntryId);
		refXml->get_widget("entryName", pEntryName);
	} catch (Gnome::Glade::XmlError& e) {
		std::cerr << e.what() << std::endl;
		exit(-1);
	}

	if (!pWindow || !pTreeView) { return; }

	//taskManager->addTag(Tag("meaning"));
	//taskManager->addTag(Tag("one"));
	//taskManager->addTag(Tag("life"));
	//taskManager->addTag(Tag("photos"));
	//taskManager->addTag(Tag("music"));

	refTreeModel = TagTreeModel::create(*taskManager);
	pTreeView->set_model(refTreeModel);
	//refTreeModelSort = Gtk::TreeModelSort::create(refTreeModel);
	//refTreeModelSort->set_sort_column(refTreeModel->columns.id, Gtk::SORT_ASCENDING);
	//pTreeView->set_model(refTreeModelSort);

	pTreeView->append_column("Id", refTreeModel->columns.id);
	pTreeView->append_column("Name", refTreeModel->columns.name);

	pTreeView->signal_cursor_changed().connect(sigc::mem_fun(*this,
		&GeToDoApp::on_treeview_cursor_changed) );
	
	try {
		refXml->connect_clicked("buttonAdd", sigc::mem_fun(*this, &GeToDoApp::on_buttonAdd_clicked) );
		refXml->connect_clicked("buttonEdit", sigc::mem_fun(*this, &GeToDoApp::on_buttonEdit_clicked) );
		refXml->connect_clicked("buttonDelete", sigc::mem_fun(*this, &GeToDoApp::on_buttonDelete_clicked) );
		refXml->connect_clicked("buttonDebug", sigc::mem_fun(*this, &GeToDoApp::on_buttonDebug_clicked) );
	} catch (Gnome::Glade::XmlError& e) {
		std::cerr << e.what() << std::endl;
		exit(-1);
	}

	// previous:
	//pButtonAdd->signal_clicked().connect(sigc::mem_fun(*this, &GeToDoApp::on_buttonAdd_clicked) );
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
		ss << row[refTreeModel->columns.id];
		pEntryId->set_text(ss.str());
		pEntryName->set_text(row[refTreeModel->columns.name]);
	}
}

void GeToDoApp::on_buttonAdd_clicked() {
	using namespace getodo;
	// TODO: boost::lexical_cast
	std::istringstream ss(pEntryId->get_text().c_str());
	int col_id;
	ss >> col_id;
	//Tag* newTag = new Tag(col_id,pEntryName->get_text().c_str());
	//tagManager.insertTag(newTag);
	taskManager->addTag(Tag(pEntryName->get_text().c_str()));
	
	// TODO: focus (selection) on newly added thing
}

void GeToDoApp::on_buttonEdit_clicked() {
	using namespace getodo;
	std::istringstream ss(pEntryId->get_text().c_str());
	int col_id;
	ss >> col_id;
	Tag* tag = new Tag(col_id,pEntryName->get_text().c_str());
	//tagManager.updateTag(tag);
	taskManager->editTag(tag->id, *tag);
}

void GeToDoApp::on_buttonDelete_clicked() {
	using namespace getodo;
	Gtk::TreeModel::iterator iter = getCursorIter(pTreeView);
	if (iter) { // TODO: check iter validity
		int col_id = iter->get_value(refTreeModel->columns.id);
		//tagManager.removeTag(col_id);
		taskManager->deleteTag(col_id);
	}
	// set cursor to next item (or previous, if no next exists)
}

void GeToDoApp::on_buttonDebug_clicked() {
	using namespace getodo;
	std::cout << "TagManager:" << std::endl;
	std::list<Tag*> tags = taskManager->getTagsList();
	for (std::list<Tag*>::iterator it = tags.begin();
		it != tags.end(); ++it) {
		std::cout << (*it)->toString() << std::endl;
	}
}

Gtk::TreeModel::iterator GeToDoApp::getCursorIter(Gtk::TreeView* pTreeView) {
	Gtk::TreeModel::iterator iter;
	if (pTreeView) {
		Gtk::TreeModel::Path path;
		Gtk::TreeViewColumn* column;
		pTreeView->get_cursor(path, column);
		if (path.gobj()){
			iter = refTreeModel->get_iter(path);
		}
	}
	return iter;
}

int main(int argc, char* argv[]) {
	GeToDoApp app(argc, argv);
	app.run();
	return 0;
}
