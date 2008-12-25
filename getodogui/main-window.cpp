// $Id$

#include "stdafx.h"
#include "main-window.h"

// ----- class MainWindow --------------------

MainWindow::MainWindow(BaseObjectType* cobject,
	const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
: Gtk::Window(cobject), refXml(refGlade),
  taskManager(0)
// TODO: initialize all widget pointers to 0
{
	Gtk::ToolButton* pTaskNewToolbutton;
	Gtk::ToolButton* pTaskDeleteToolbutton;
	Gtk::ToolButton* pTaskUpdateToolbutton;
	try {
		// child widgets
		refXml->get_widget("taskTreeview", pTaskTreeView);
		refXml->get_widget("tagTreeview", pTagTreeView);
		refXml->get_widget("filterTreeview", pFilterTreeView);
		
		refXml->get_widget("taskDescriptionEntry", pTaskDescriptionEntry);
		refXml->get_widget("taskLongDescriptionTextview", pTaskLongDescriptionTextView);
		refTaskLongDescriptionTextBuffer = Gtk::TextBuffer::create();

		refXml->get_widget("taskTagsEntry", pTaskTagsEntry);
		refXml->get_widget("taskDoneCheckbutton", pTaskDoneCheckbutton);
		refXml->get_widget("taskCompletedPercentageSpinbutton", pTaskCompletedPercentageSpinbutton);
		refXml->get_widget("taskPrioritySpinbutton", pTaskPrioritySpinbutton);
		refXml->get_widget("taskRecurrenceEntry", pTaskRecurrenceEntry);
		refXml->get_widget("taskIdLabel", pTaskIdLabel);
		refXml->get_widget("taskDateDeadlineEntry", pTaskDateDeadlineEntry);
		refXml->get_widget("taskDateStartedEntry", pTaskDateStartedEntry);
		refXml->get_widget("taskDateCompletedEntry", pTaskDateCompletedEntry);
		refXml->get_widget("taskDateCreatedLabel", pTaskDateCreatedLabel);
		refXml->get_widget("taskDateLastModifiedLabel", pTaskDateLastModifiedLabel);

		refXml->get_widget("taskNewToolbutton", pTaskNewToolbutton);
		refXml->get_widget("taskDeleteToolbutton", pTaskDeleteToolbutton);
		refXml->get_widget("taskUpdateToolbutton", pTaskUpdateToolbutton);

		// signals
		pTaskNewToolbutton->signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonTaskNew_clicked) );
		pTaskDeleteToolbutton->signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonTaskDelete_clicked) );
		pTaskUpdateToolbutton->signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonTaskUpdate_clicked) );

		refXml->connect_clicked("taskRecurrenceButton",
			sigc::mem_fun(*this, &MainWindow::on_buttonRecurrence_clicked) );

		pTaskDescriptionEntry->signal_focus_out_event().connect(
			sigc::bind(
				sigc::mem_fun(*this, &MainWindow::on_taskDescriptionEntry_focus_out_event),
				pTaskDescriptionEntry
			));
		//on_taskDescriptionEntry_focus_out_event
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
	//refTaskTreeModel = getodo::TaskTreeModel::create(*taskManager);
	refTaskTreeModel = getodo::TaskTreeStore::create(*taskManager);
	pTaskTreeView->set_model(refTaskTreeModel);
	
	// TODO: Sorting model doesn't work well with my custom models, fix this.
	// However it works well with TaskTreeStore.
	//refTaskTreeModelSort = Gtk::TreeModelSort::create(refTaskTreeModel);
	//refTaskTreeModelSort->set_sort_column(refTaskTreeModel->columns.id, Gtk::SORT_ASCENDING);
	//pTaskTreeView->set_model(refTaskTreeModelSort);
	
	// Add TreeView columns when setting a model for the first time.
	// The model columns don't change, so it's ok to add them only once.
	if (pTaskTreeView->get_columns().empty()) {
		pTaskTreeView->append_column("Id", refTaskTreeModel->columns.id);
		pTaskTreeView->append_column("Description", refTaskTreeModel->columns.description);
		pTaskTreeView->append_column("Done", refTaskTreeModel->columns.done);
		pTaskTreeView->append_column("%", refTaskTreeModel->columns.completedPercentage);
		pTaskTreeView->append_column("!", refTaskTreeModel->columns.priority);
		pTaskTreeView->append_column("Deadline", refTaskTreeModel->columns.dateDeadline);
	}

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

	// ---- filters treeview ----

	// filter TreeView is now using a dummy ListStore
	refFilterTreeModel = Gtk::ListStore::create(filterColumns);
	pFilterTreeView->set_model(refFilterTreeModel);
	if (pFilterTreeView->get_columns().empty()) {
		pFilterTreeView->append_column("Id", filterColumns.id);
		pFilterTreeView->append_column("Name", filterColumns.name);
		pFilterTreeView->append_column("Rule", filterColumns.rule);
	}

	// sample data:
	Gtk::TreeModel::Row row = *(refFilterTreeModel->append());
	row[filterColumns.id] = 42;
	row[filterColumns.name] = "dummy filter";
	row[filterColumns.rule] = "priority > 5";
}

void MainWindow::on_taskTreeview_selection_changed() {
	// display the selected task's contents in the task editing panel
	using namespace getodo;
	Gtk::TreeModel::iterator iter = pTaskTreeView->get_selection()->get_selected();
	Task* task = 0;
	if (iter) {
		task = taskManager->getTask((*iter)[refTaskTreeModel->columns.id]);
	}
	if (task){
		fillEditingPanel(*task);
	} else {
		clearEditingPanel();
	}
}

void MainWindow::on_buttonTaskNew_clicked() {
	// create a new task and start editing it
	if (!taskManager) { return; }

	using namespace getodo;
	id_t newTaskId = taskManager->addTask(*(new Task()));
	Task& newTask = *taskManager->getTask(newTaskId);
	fillEditingPanel(newTask);
	
	// select newly created task's row in treeview
	Gtk::TreeModel::Path& path = refTaskTreeModel->getPathByTask(newTask);
	pTaskTreeView->get_selection()->select(path);

	pTaskDescriptionEntry->grab_focus();
}

void MainWindow::on_buttonTaskDelete_clicked() {
	// delete currently selected task
	if (!taskManager) { return; }

	// TODO: or delete multiple selected rows (use get_selected_rows())
	Gtk::TreeModel::iterator iter = pTaskTreeView->get_selection()->get_selected();
	if (iter) {
		taskManager->deleteTask((*iter)[refTaskTreeModel->columns.id]);
	}
}

void MainWindow::on_buttonTaskUpdate_clicked() {
	// save editing panel contents
	if (!taskManager) { return; }
	using namespace getodo;
	id_t taskId = getCurrentlyEditedTaskId();
	Task* updatedTask = taskManager->getTask(taskId);
	if (updatedTask) {
		saveEditingPanelToTask(*updatedTask);
		taskManager->editTask(taskId, *updatedTask);
	}
}

bool MainWindow::on_taskDescriptionEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry) {
	if (!taskManager || !entry) { return false; }
	using namespace getodo;
	id_t taskId = getCurrentlyEditedTaskId();
	if (!taskManager->hasTask(taskId)) {
		return false;
	}
	TaskPersistence& tp = taskManager->getPersistentTask(taskId);
	return updateTaskPartial(boost::bind( &TaskPersistence::setDescription, _1,
		boost::ref(entry->get_text()) ));
}

void MainWindow::on_buttonRecurrence_clicked() {
	using namespace getodo;
	RecurrenceDialog& dialog = GeToDoApp::getSingleton().getRecurrenceDialog();
	dialog.setRecurrence(*getodo::Recurrence::fromString(
		pTaskRecurrenceEntry->get_text()));
	int response = dialog.run();
	if (response == Gtk::RESPONSE_OK) {
		pTaskRecurrenceEntry->set_text(Recurrence::toString(dialog.getRecurrence()));
	}
	updateTaskPartial(boost::bind(
		&TaskPersistence::setRecurrence, _1, boost::ref(dialog.getRecurrence())));
}

void MainWindow::fillEditingPanel(getodo::Task& task) {
	if (!taskManager) { return; }

	pTaskDescriptionEntry->set_text(task.getDescription());
	refTaskLongDescriptionTextBuffer->set_text(task.getLongDescription());
	pTaskTagsEntry->set_text(task.getTagsAsString(*taskManager));
	pTaskDoneCheckbutton->set_active(task.isDone());
	pTaskCompletedPercentageSpinbutton->set_value(task.getCompletedPercentage());
	pTaskPrioritySpinbutton->set_value(task.getPriority());
	//pTaskRecurrenceEntry->set_text(
	//	task.getRecurrence().toString()); // without recurrence type id
	pTaskRecurrenceEntry->set_text(
		getodo::Recurrence::toString(task.getRecurrence())); // with recurrence type id
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
	pTaskTagsEntry->set_text("");
	pTaskDoneCheckbutton->set_active(false);
	pTaskCompletedPercentageSpinbutton->set_value(0);
	pTaskPrioritySpinbutton->set_value(0);
	pTaskRecurrenceEntry->set_text("");
	pTaskIdLabel->set_text("");
	pTaskDateDeadlineEntry->set_text("");
	pTaskDateStartedEntry->set_text("");
	pTaskDateCompletedEntry->set_text("");
	pTaskDateCreatedLabel->set_text("");
	pTaskDateLastModifiedLabel->set_text("");
}

void MainWindow::saveEditingPanelToTask(getodo::Task& task) {
	if (!taskManager) { return; }

	using namespace getodo;
	task.setDescription(pTaskDescriptionEntry->get_text());
	task.setLongDescription(refTaskLongDescriptionTextBuffer->get_text());
	task.setTagsFromString(*taskManager, pTaskTagsEntry->get_text());
	task.setDone(pTaskDoneCheckbutton->get_active());
	task.setCompletedPercentage(pTaskCompletedPercentageSpinbutton->get_value_as_int());
	task.setPriority(pTaskPrioritySpinbutton->get_value_as_int());
	task.setRecurrence(Recurrence::fromString(
		pTaskRecurrenceEntry->get_text())); // with recurrence type id
	task.setDateDeadline(Date(pTaskDateDeadlineEntry->get_text()));
	task.setDateStarted(Date(pTaskDateStartedEntry->get_text()));
	task.setDateCompleted(Date(pTaskDateCompletedEntry->get_text()));
}

getodo::id_t MainWindow::getCurrentlyEditedTaskId() {
	using namespace getodo;
	try {
		return boost::lexical_cast<id_t, std::string>(pTaskIdLabel->get_text());
	} catch (boost::bad_lexical_cast) {
		return -1;
	}
}

bool MainWindow::updateTaskPartial(boost::function<void(getodo::TaskPersistence&)> f) {
	using namespace getodo;
	id_t taskId = getCurrentlyEditedTaskId();
	if (!taskManager->hasTask(taskId)) {
		return false;
	}
	TaskPersistence& tp = taskManager->getPersistentTask(taskId);
	f(tp);
	Task& task = *tp.getTask();
	pTaskDateLastModifiedLabel->set_text(task.getDateLastModified().toString());
	taskManager->signal_task_updated(task);
	return true;
}