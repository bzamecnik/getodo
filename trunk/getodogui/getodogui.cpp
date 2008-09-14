#include "stdafx.h"
#include "getodogui.h"

//// ----- class TagWindow --------------------
//
//TagWindow::TagWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
//: Gtk::Window(cobject), refXml(refGlade)
//{
//	try {
//		// child widgets
//		refXml->get_widget("tagTreeview", pTagTreeView);
//		refXml->get_widget("entryId", pEntryId);
//		refXml->get_widget("entryName", pEntryName);
//		// signals
//		refXml->connect_clicked("buttonAdd", sigc::mem_fun(*this, &TagWindow::on_buttonAdd_clicked) );
//		refXml->connect_clicked("buttonEdit", sigc::mem_fun(*this, &TagWindow::on_buttonEdit_clicked) );
//		refXml->connect_clicked("buttonDelete", sigc::mem_fun(*this, &TagWindow::on_buttonDelete_clicked) );
//		refXml->connect_clicked("buttonDebug", sigc::mem_fun(*this, &TagWindow::on_buttonDebug_clicked) );
//	} catch (Gnome::Glade::XmlError& e) {
//		std::cerr << e.what() << std::endl;
//		exit(-1);
//	}
//	pTagTreeView->signal_cursor_changed().connect(sigc::mem_fun(*this,
//		&TagWindow::on_treeview_cursor_changed) );
//}
//
//TagWindow::~TagWindow() {}
//
//void TagWindow::setTaskManager(getodo::TaskManager* manager) {
//	if (!manager) { return; } 
//	taskManager = manager;
//}
//
//void TagWindow::on_treeview_cursor_changed()
//{
//	Gtk::TreeModel::iterator iter = getCursorIter(pTagTreeView);
//	if (iter) {
//		Gtk::TreeModel::Row row = *iter;
//		// TODO: boost::lexical_cast
//		std::ostringstream ss;
//		ss << row[refTreeModel->columns.id];
//		pEntryId->set_text(ss.str());
//		pEntryName->set_text(row[refTreeModel->columns.name]);
//	}
//}
//
//void TagWindow::on_buttonAdd_clicked() {
//	using namespace getodo;
//	// TODO: boost::lexical_cast
//	std::istringstream ss(pEntryId->get_text().c_str());
//	int col_id;
//	ss >> col_id;
//	//Tag* newTag = new Tag(col_id,pEntryName->get_text().c_str());
//	//tagManager.insertTag(newTag);
//	taskManager->addTag(Tag(pEntryName->get_text().c_str()));
//	
//	// TODO: focus (selection) on newly added thing
//}
//
//void TagWindow::on_buttonEdit_clicked() {
//	using namespace getodo;
//	std::istringstream ss(pEntryId->get_text().c_str());
//	int col_id;
//	ss >> col_id;
//	Tag* tag = new Tag(col_id,pEntryName->get_text().c_str());
//	//tagManager.updateTag(tag);
//	taskManager->editTag(tag->id, *tag);
//}
//
//void TagWindow::on_buttonDelete_clicked() {
//	using namespace getodo;
//	Gtk::TreeModel::iterator iter = getCursorIter(pTagTreeView);
//	if (iter) { // TODO: check iter validity
//		int col_id = iter->get_value(refTreeModel->columns.id);
//		//tagManager.removeTag(col_id);
//		taskManager->deleteTag(col_id);
//	}
//	// set cursor to next item (or previous, if no next exists)
//}
//
//void TagWindow::on_buttonDebug_clicked() {
//	using namespace getodo;
//	std::cout << "TagManager:" << std::endl;
//	std::list<Tag*> tags = taskManager->getTagsList();
//	for (std::list<Tag*>::iterator it = tags.begin();
//		it != tags.end(); ++it) {
//		std::cout << (*it)->toString() << std::endl;
//	}
//}
//
//Gtk::TreeModel::iterator TagWindow::getCursorIter(Gtk::TreeView* pTreeView) {
//	Gtk::TreeModel::iterator iter;
//	if (pTreeView) {
//		Gtk::TreeModel::Path path;
//		Gtk::TreeViewColumn* column;
//		pTreeView->get_cursor(path, column);
//		if (path.gobj()){
//			iter = refTreeModel->get_iter(path);
//		}
//	}
//	return iter;
//}

// ----- class MainWindow --------------------

MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
: Gtk::Window(cobject), refXml(refGlade)
// TODO: initialize all widget pointers to 0
{
	Gtk::ToolButton* pTaskNewToolbutton;
	Gtk::ToolButton* pTaskDeleteToolbutton;
	try {
		// child widgets
		refXml->get_widget("taskTreeview", pTaskTreeView);
		refXml->get_widget("tagTreeview", pTagTreeView);
		
		refXml->get_widget("taskDescriptionEntry", pTaskDescriptionEntry);
		refXml->get_widget("taskLongDescriptionTextview", pTaskLongDescriptionTextView);
		refTaskLongDescriptionTextBuffer = Gtk::TextBuffer::create();

		refXml->get_widget("taskTagsEntry", pTaskTagsEntry);
		refXml->get_widget("taskDoneCheckbutton", pTaskDoneCheckbutton);
		refXml->get_widget("taskCompletedPercentageSpinbutton", pTaskCompletedPercentageSpinbutton);
		refXml->get_widget("taskPrioritySpinbutton", pTaskPrioritySpinbutton);
		refXml->get_widget("taskIdLabel", pTaskIdLabel);
		refXml->get_widget("taskDateDeadlineEntry", pTaskDateDeadlineEntry);
		refXml->get_widget("taskDateStartedEntry", pTaskDateStartedEntry);
		refXml->get_widget("taskDateCompletedEntry", pTaskDateCompletedEntry);
		refXml->get_widget("taskDateCreatedLabel", pTaskDateCreatedLabel);
		refXml->get_widget("taskDateLastModifiedLabel", pTaskDateLastModifiedLabel);

		refXml->get_widget("taskNewToolbutton", pTaskNewToolbutton);
		refXml->get_widget("taskDeleteToolbutton", pTaskDeleteToolbutton);

		// signals
		pTaskNewToolbutton->signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonTaskNew_clicked) );
		pTaskDeleteToolbutton->signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonTaskDelete_clicked) );
	} catch (Gnome::Glade::XmlError& e) {
		std::cerr << e.what() << std::endl;
		exit(-1);
	}
	pTaskTreeView->get_selection()->signal_changed().connect(sigc::mem_fun(*this,
		&MainWindow::on_taskTreeview_selection_changed) );
}

MainWindow::~MainWindow() {}

void MainWindow::setTaskManager(getodo::TaskManager* manager) {
	if (!manager || !pTaskTreeView) { return; } 
	// set a new model for task TreeView
	taskManager = manager;

	// ---- task treeview ----
	refTaskTreeModel = getodo::TaskTreeModel::create(*taskManager);
	pTaskTreeView->set_model(refTaskTreeModel);
	
	// Add TreeView columns when setting a model for the first time.
	// The model columns don't change, so it's ok to add them only once.
	if (pTaskTreeView->get_columns().empty()) {
		//pTaskTreeView->append_column("Id", refTaskTreeModel->columns.id);
		pTaskTreeView->append_column("Done", refTaskTreeModel->columns.done);
		pTaskTreeView->append_column("%", refTaskTreeModel->columns.completedPercentage);
		pTaskTreeView->append_column("!", refTaskTreeModel->columns.priority);
		pTaskTreeView->append_column("Description", refTaskTreeModel->columns.description);
		pTaskTreeView->append_column("Deadline", refTaskTreeModel->columns.dateDeadline);
	}
	
	// TODO: Sorting model doesn't work well with my custom models, fix this.
	//refTaskTreeModelSort = Gtk::TreeModelSort::create(refTaskTreeModel);
	//refTaskTreeModelSort->set_sort_column(refTaskTreeModel->columns.id, Gtk::SORT_ASCENDING);
	//pTaskTreeView->set_model(refTaskTreeModelSort);

	// ---- tag treeview ----
	if (!pTagTreeView) { return; }
	refTagTreeModel = getodo::TagTreeModel::create(*taskManager);

	pTagTreeView->set_model(refTagTreeModel);
	if (pTagTreeView->get_columns().empty()) {
		pTagTreeView->append_column("Id", refTagTreeModel->columns.id); // future: hidden
		//pTagTreeView->append_column("Active", refTagTreeModel->columns.active);
		pTagTreeView->append_column("Name", refTagTreeModel->columns.name);
	}
	
	//refTagTreeModelSort = Gtk::TreeModelSort::create(refTagTreeModel);
	//refTagTreeModelSort->set_sort_column(refTagTreeModel->columns.id, Gtk::SORT_ASCENDING);
	//pTagTreeView->set_model(refTagTreeModelSort);

	// task long description textview
	pTaskLongDescriptionTextView->set_buffer(refTaskLongDescriptionTextBuffer);	
}

void MainWindow::on_taskTreeview_selection_changed()
{
	// display the selected task's contents in the task editing panel

	using namespace getodo;
	Gtk::TreeModel::iterator iter = pTaskTreeView->get_selection()->get_selected();
	if (iter) {
		TaskNode* node = static_cast<TaskNode*>(iter.gobj()->user_data);
		if (!node) { return; }
		Task& task = node->get_item();
		fillEditingPanel(task);
	} else {
		clearEditingPanel();
	}
}

void MainWindow::on_buttonTaskNew_clicked() {
	using namespace getodo;
	id_t newTaskId = taskManager->addTask(*(new Task()));
	fillEditingPanel(*taskManager->getTask(newTaskId));
	// TODO: select newly created task's row in treeview (?)
	pTaskDescriptionEntry->grab_focus();
}

void MainWindow::on_buttonTaskDelete_clicked() {
	Gtk::TreeModel::iterator iter = pTaskTreeView->get_selection()->get_selected();
	if (iter) {
		taskManager->deleteTask((*iter)[refTaskTreeModel->columns.id]);
	}
}

void MainWindow::fillEditingPanel(getodo::Task& task) {
	pTaskDescriptionEntry->set_text(task.getDescription());
	refTaskLongDescriptionTextBuffer->set_text(task.getLongDescription());
	//pTaskTagsEntry->set_text(task.getTagsAsString(*taskManager));
	pTaskDoneCheckbutton->set_active(task.isDone());
	pTaskCompletedPercentageSpinbutton->set_value(task.getCompletedPercentage());
	pTaskPrioritySpinbutton->set_value(task.getPriority());
	pTaskIdLabel->set_text(boost::lexical_cast<std::string,int>(task.getTaskId()));
	pTaskDateDeadlineEntry->set_text(task.getDateDeadline().toString());
	pTaskDateStartedEntry->set_text(task.getDateStarted().toString());
	pTaskDateCompletedEntry->set_text(task.getDateCompleted().toString());
	pTaskDateCreatedLabel->set_text(task.getDateCreated().toString());
	pTaskDateLastModifiedLabel->set_text(task.getDateLastModified().toString());
}

void MainWindow::clearEditingPanel() {
	pTaskDescriptionEntry->set_text("");
	refTaskLongDescriptionTextBuffer->set_text("");
	//pTaskTagsEntry->set_text("");
	pTaskDoneCheckbutton->set_active(false);
	pTaskCompletedPercentageSpinbutton->set_value(0);
	pTaskPrioritySpinbutton->set_value(0);
	pTaskIdLabel->set_text("");
	pTaskDateDeadlineEntry->set_text("");
	pTaskDateStartedEntry->set_text("");
	pTaskDateCompletedEntry->set_text("");
	pTaskDateCreatedLabel->set_text("");
	pTaskDateLastModifiedLabel->set_text("");
}

// ----- class GeToDoApp --------------------

GeToDoApp::GeToDoApp(int argc, char* argv[])
	: kit(argc, argv), pWindow(0)
{
	using namespace getodo;
	if (argc <= 1) {
		std::cerr << "Usage: " << argv[0] << " DATABASE_FILE" << std::endl;
		exit(1);
	}
	taskManager = new TaskManager(argv[1]);

	Glib::RefPtr<Gnome::Glade::Xml> refXml;
	try {
		refXml = Gnome::Glade::Xml::create("getodo-mainwindow.glade");
		refXml->get_widget_derived("mainWindow", pWindow);
	} catch (Gnome::Glade::XmlError& e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
	pWindow->setTaskManager(taskManager);
}

GeToDoApp::~GeToDoApp() {
}

void GeToDoApp::run() {
	if (pWindow) {	
		kit.run(*pWindow);
		// TODO: Find out, whether to explicitly delete widget created by Glade.
		
		// delete pWindow;
		// pWindow = 0;
	}
}

int main(int argc, char* argv[]) {
	GeToDoApp app(argc, argv);
	app.run();
	return 0;
}
