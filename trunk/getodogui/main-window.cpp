// $Id$

#include "stdafx.h"
#include "main-window.h"

// ----- class MainWindow --------------------

MainWindow::MainWindow(BaseObjectType* cobject,
	const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
: Gtk::Window(cobject), refXml(refGlade),
  taskManager(0), filteringActive(false)
// TODO: initialize all widget pointers to 0
{
	Gtk::ToolButton* pTaskNewToplevelToolbutton = 0;
	Gtk::ToolButton* pTaskNewToolbutton = 0;
	Gtk::ToolButton* pTaskNewSubtaskToolbutton = 0;
	Gtk::ToolButton* pTaskDeleteToolbutton = 0;
	Gtk::ToolButton* pTaskUpdateToolbutton = 0;
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

		refXml->get_widget("taskNewToplevelToolbutton", pTaskNewToplevelToolbutton);
		refXml->get_widget("taskNewToolbutton", pTaskNewToolbutton);
		refXml->get_widget("taskNewSubtaskToolbutton", pTaskNewSubtaskToolbutton);
		refXml->get_widget("taskDeleteToolbutton", pTaskDeleteToolbutton);
		refXml->get_widget("taskUpdateToolbutton", pTaskUpdateToolbutton);
		refXml->get_widget("taskUpdateToolbutton", pTaskUpdateToolbutton);
		refXml->get_widget("taskFilterToggletoolbutton", pTaskFilterToggletoolbutton);

		// signals
		pTaskNewToplevelToolbutton->signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonTaskNewToplevel_clicked) );
		pTaskNewToolbutton->signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonTaskNew_clicked) );
		pTaskNewSubtaskToolbutton->signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonTaskNewSubtask_clicked) );
		pTaskDeleteToolbutton->signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonTaskDelete_clicked) );
		pTaskUpdateToolbutton->signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonTaskUpdate_clicked) );
		pTaskFilterToggletoolbutton->signal_toggled().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonTaskFilter_toggled) );

		refXml->connect_clicked("taskRecurrenceButton",
			sigc::mem_fun(*this, &MainWindow::on_buttonRecurrence_clicked) );

		// - saving individual parts of task
		// TODO: bind the signal handler in a cycle over such a vector:
		//std::vector<std::pair<Gtk::Widget*, boost::function>> editingPanelWidgets =
		
		pTaskDescriptionEntry->signal_focus_out_event().connect(
			sigc::bind(
				sigc::mem_fun(*this, &MainWindow::on_taskDescriptionEntry_focus_out_event),
				pTaskDescriptionEntry
			));
		pTaskLongDescriptionTextView->signal_focus_out_event().connect(
			sigc::bind(
				sigc::mem_fun(*this, &MainWindow::on_taskLongDescriptionTextview_focus_out_event),
				pTaskLongDescriptionTextView
			));
		pTaskDoneCheckbutton->signal_focus_out_event().connect(
			sigc::bind(
				sigc::mem_fun(*this, &MainWindow::on_taskDoneCheckbutton_focus_out_event),
				pTaskDoneCheckbutton
			));
		pTaskCompletedPercentageSpinbutton->signal_focus_out_event().connect(
			sigc::bind(
				sigc::mem_fun(*this, &MainWindow::on_taskCompletedPercentageSpinbutton_focus_out_event),
				pTaskCompletedPercentageSpinbutton
			));
		pTaskPrioritySpinbutton->signal_focus_out_event().connect(
			sigc::bind(
				sigc::mem_fun(*this, &MainWindow::on_taskPrioritySpinbutton_focus_out_event),
				pTaskPrioritySpinbutton
			));
		pTaskRecurrenceEntry->signal_focus_out_event().connect(
			sigc::bind(
				sigc::mem_fun(*this, &MainWindow::on_taskRecurrenceEntry_focus_out_event),
				pTaskRecurrenceEntry
			));
		pTaskDateDeadlineEntry->signal_focus_out_event().connect(
			sigc::bind(
				sigc::mem_fun(*this, &MainWindow::on_taskDateDeadlineEntry_focus_out_event),
				pTaskDateDeadlineEntry
			));
		pTaskDateStartedEntry->signal_focus_out_event().connect(
			sigc::bind(
				sigc::mem_fun(*this, &MainWindow::on_taskDateStartedEntry_focus_out_event),
				pTaskDateStartedEntry
			));
		pTaskDateCompletedEntry->signal_focus_out_event().connect(
			sigc::bind(
				sigc::mem_fun(*this, &MainWindow::on_taskDateCompletedEntry_focus_out_event),
				pTaskDateCompletedEntry
			));
	} catch (Gnome::Glade::XmlError& e) {
		std::cerr << e.what() << std::endl;
		exit(-1);
	}
	pTaskTreeView->get_selection()->signal_changed().connect(sigc::mem_fun(*this,
		&MainWindow::on_taskTreeview_selection_changed) );
	pFilterTreeView->get_selection()->signal_changed().connect(sigc::mem_fun(*this,
		&MainWindow::on_filterTreeview_selection_changed) );
}

MainWindow::~MainWindow() {}

void MainWindow::setTaskManager(getodo::TaskManager* manager) {
	if (!manager || !pTaskTreeView) { return; } 
	// set a new model for task TreeView
	taskManager = manager;

	// ---- task treeview ----
	// a custom tree store
	refTaskTreeModel = getodo::TaskTreeStore::create(*taskManager);
	
	// treestore packed into a filter model
	refTaskTreeModelFilter = Gtk::TreeModelFilter::create(refTaskTreeModel);

	// TODO: default should be unfiltered
	//refTaskTreeModelFilter->set_visible_func( sigc::mem_fun(*this,
	//	&MainWindow::on_taskTreeview_filter_row_visible) );
	// need to pack the filter model inside a sort model

	refTaskTreeModelSort = Gtk::TreeModelSort::create(refTaskTreeModelFilter);
	// TODO: think of what will be the default sorting column
	refTaskTreeModelSort->set_sort_column(refTaskTreeModel->columns.dateDeadline, Gtk::SORT_DESCENDING);

	pTaskTreeView->set_model(refTaskTreeModelSort);
	
	// Add TreeView columns when setting a model for the first time.
	// The model columns don't change, so it's ok to add them only once.
	if (pTaskTreeView->get_columns().empty()) {
		getodo::TaskTreeStore::ModelColumns& columns = refTaskTreeModel->columns;

		pTaskTreeView->append_column("Id", columns.id);
		pTaskTreeView->get_column(0)->set_sort_column(columns.id);

		pTaskTreeView->append_column("Description", columns.description);
		pTaskTreeView->get_column(1)->set_sort_column(columns.description);
		
		pTaskTreeView->append_column("Done", columns.done);
		pTaskTreeView->get_column(2)->set_sort_column(columns.done);
		
		pTaskTreeView->append_column("%", columns.completedPercentage);
		pTaskTreeView->get_column(3)->set_sort_column(columns.completedPercentage);
		
		pTaskTreeView->append_column("!", columns.priority);
		pTaskTreeView->get_column(4)->set_sort_column(columns.priority);
		
		pTaskTreeView->append_column("Deadline", columns.dateDeadline);
		pTaskTreeView->get_column(5)->set_sort_column(columns.dateDeadline);
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
	//Gtk::TreeModel::Row row = *(refFilterTreeModel->append());
	//row[filterColumns.id] = 42;
	//row[filterColumns.name] = "dummy filter";
	//row[filterColumns.rule] = "priority > 5";

	std::vector<getodo::FilterRule*>& filters = taskManager->getFilterRules();
	for(std::vector<getodo::FilterRule*>::iterator it = filters.begin();
		it != filters.end(); ++it)
	{
		Gtk::TreeModel::Row row = *(refFilterTreeModel->append());
		row[filterColumns.id] = (*it)->id;
		row[filterColumns.name] = Glib::ustring((*it)->name);
		row[filterColumns.rule] = Glib::ustring((*it)->rule);
	}
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

void MainWindow::on_filterTreeview_selection_changed() {
	setFilterRuleFromSelection();
}

void MainWindow::on_buttonTaskNewToplevel_clicked() {
	// Create a new top level task and start editing it.
	if (!taskManager) { return; }

	using namespace getodo;
	id_t newTaskId = taskManager->addTask(*(new Task()));
	Task& newTask = *taskManager->getTask(newTaskId);
	fillEditingPanel(newTask);
	
	// select newly created task's row in treeview
	Gtk::TreeModel::iterator iter = refTaskTreeModel->getIterByTask(newTask);
	pTaskTreeView->get_selection()->select(refTaskTreeModel->get_path(iter));

	pTaskDescriptionEntry->grab_focus();
}

void MainWindow::on_buttonTaskNew_clicked() {
	// TODO: add a task on the same level as selected task
}

void MainWindow::on_buttonTaskNewSubtask_clicked() {
	// Create a new subtask or currently selected task and start editing it.
	// Create new top level task if nothing selected.
	if (!taskManager) { return; }

	using namespace getodo;
	id_t selectedTaskId = getCurrentlyEditedTaskId();
	Task* parentTask = 0;
	id_t newTaskId = taskManager->addTask(*(new Task()));
	Task& newTask = *taskManager->getTask(newTaskId);

	if (Task::isValidId(selectedTaskId)	&&
		(parentTask = taskManager->getTask(selectedTaskId)))
	{
		newTask.setParent(parentTask->getTaskId(), *taskManager);
	}
	
	fillEditingPanel(newTask);
	
	// select newly created task's row in treeview
	Gtk::TreeModel::iterator iter = refTaskTreeModel->getIterByTask(newTask);
	Gtk::TreeModel::Path path = refTaskTreeModel->get_path(iter);
	pTaskTreeView->expand_to_path(path);
	pTaskTreeView->get_selection()->select(path);

	pTaskDescriptionEntry->grab_focus();
}

void MainWindow::on_buttonTaskDelete_clicked() {
	// delete currently selected task
	if (!taskManager) { return; }

	// TODO: or delete multiple selected rows (use get_selected_rows())
	Gtk::TreeModel::iterator iter = pTaskTreeView->get_selection()->get_selected();
	if (iter) {
		// TODO: select next row (will that work, when using a sorted model?)
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

void MainWindow::on_buttonTaskFilter_toggled() {
	std::cout << "on_buttonTaskFilter_toggled" << std::endl;
	filteringActive = pTaskFilterToggletoolbutton->get_active();
	//toggleFiltering(active);
	if (filteringActive) {
		setFilterRuleFromSelection();
	} else {
		resetTaskFilterRule();
		//pFilterTreeView->get_selection()->unselect_all();
	}
}

// event handling to save individual items of editing panel

bool MainWindow::on_taskDescriptionEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry) {
	using namespace getodo;
	if (!entry) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setDescription, _1,
		boost::ref(entry->get_text()) ));
}

bool MainWindow::on_taskLongDescriptionTextview_focus_out_event(GdkEventFocus* event, Gtk::TextView* textview) {
	using namespace getodo;
	if (!textview) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setLongDescription, _1,
		boost::ref(textview->get_buffer()->get_text()) ));
}

//bool MainWindow: on_taskTagsEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry) {
// //TODO: we need TaskPersistence.setTagsFromString()
//}

bool MainWindow::on_taskDoneCheckbutton_focus_out_event(GdkEventFocus* event, Gtk::CheckButton* checkbutton) {
	using namespace getodo;
	if (!checkbutton) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setDone, _1,
		checkbutton->get_active() ));
}

bool MainWindow::on_taskCompletedPercentageSpinbutton_focus_out_event(GdkEventFocus* event, Gtk::SpinButton* spinbutton) {
	using namespace getodo;
	if (!spinbutton) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setCompletedPercentage, _1,
		spinbutton->get_value_as_int() ));
}

bool MainWindow::on_taskPrioritySpinbutton_focus_out_event(GdkEventFocus* event, Gtk::SpinButton* spinbutton) {
	using namespace getodo;
	if (!spinbutton) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setPriority, _1,
		spinbutton->get_value_as_int() ));
}

bool MainWindow::on_taskRecurrenceEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry) {
	using namespace getodo;
	if (!entry) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setRecurrence, _1,
		boost::ref(*Recurrence::fromString(entry->get_text())) ));
}

bool MainWindow::on_taskDateDeadlineEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry) {
	using namespace getodo;
	if (!entry) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setDateDeadline, _1,
		boost::ref(Date(entry->get_text())) ));
}

bool MainWindow::on_taskDateStartedEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry) {
	using namespace getodo;
	if (!entry) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setDateStarted, _1,
		boost::ref(Date(entry->get_text())) ));
}

bool MainWindow::on_taskDateCompletedEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry) {
	using namespace getodo;
	if (!entry) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setDateCompleted, _1,
		boost::ref(Date(entry->get_text())) ));
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
	updateTaskPartial(boost::bind( &TaskPersistence::setRecurrence, _1,
		boost::ref(dialog.getRecurrence()) ));
}

bool MainWindow::on_taskTreeview_filter_row_visible(const Gtk::TreeModel::const_iterator& iter) {
	if (!filteringActive) {
		return true;
	}
	if(iter)
	{
		//iter seems to be an iter to the child model:
		//Gtk::TreeModel::iterator iter_child =
			//refTaskTreeModelFilter->convert_iter_to_child_iter(iter);
		//if(iter_child)
		//{
		Gtk::TreeModel::Row row = *iter;
		return taskManager->isTaskVisible(row[refTaskTreeModel->columns.id]);
		//return row[refTaskTreeModel->columns.visible];
		//}
	}
	return true;
}

void MainWindow::fillEditingPanel(getodo::Task& task) {
	if (!taskManager) { return; }

	pTaskDescriptionEntry->set_text(task.getDescription());
	//refTaskLongDescriptionTextBuffer->set_text(task.getLongDescription());
	pTaskLongDescriptionTextView->get_buffer()->set_text(task.getLongDescription());
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
	pTaskLongDescriptionTextView->get_buffer()->set_text("");
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
	task.setLongDescription(pTaskLongDescriptionTextView->get_buffer()->get_text());
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
	if (!taskManager || !taskManager->hasTask(taskId)) {
		return false;
	}
	TaskPersistence& tp = taskManager->getPersistentTask(taskId);
	f(tp);
	Task* task = tp.getTask();
	if (!task) { return false; }
	pTaskDateLastModifiedLabel->set_text(task->getDateLastModified().toString());
	taskManager->signal_task_updated(*task);
	return true;
}


void MainWindow::setTaskFilterRule(getodo::FilterRule& filter) {
	try {
		taskManager->setActiveFilterRule(filter);
	} catch (getodo::GetodoError&) {
		//toggleFiltering(false);
		filteringActive = false;
	}
	refTaskTreeModelFilter->refilter();
}

void MainWindow::resetTaskFilterRule() {
	taskManager->resetActiveFilterRule();
	refTaskTreeModelFilter->refilter();
}

void MainWindow::setFilterRuleFromSelection() {
	// set the selected filter rule as active
	using namespace getodo;
	Gtk::TreeModel::iterator iter = pFilterTreeView->get_selection()->get_selected();
	Gtk::TreeModel::Row row = *iter;
	id_t filterId = getodo::FilterRule::INVALID_ID;
	if (iter) {
		filterId = row[filterColumns.id];
	}
	std::cout << "filter id: " << filterId << std::endl;
	getodo::FilterRule* activeRule = taskManager->getActiveFilterRule();
	if (activeRule && (activeRule->id == filterId)) {
		return; // nothing changes, the selected rule is already active
	}
//	try {
	// TODO: possible exception from getFilterRule() should be handled
	// The problem is that throwing exception in signal handler kills the app.
	if (taskManager->hasFilterRule(filterId)) {
		FilterRule& filterRule = taskManager->getFilterRule(filterId);
		setTaskFilterRule(filterRule);
		pTaskFilterToggletoolbutton->set_active();
	}
//	} catch (GetodoError&) {
	else {
		resetTaskFilterRule();
		pTaskFilterToggletoolbutton->set_active(false);
	}
	//	//setTaskFilterRule(FilterRule(std::string(Glib::ustring(row[filterColumns.name])),
	//	//	std::string(Glib::ustring(row[filterColumns.rule]))));
//	}
}

//void MainWindow::toggleFiltering(bool on) {
//	if (on) {
//		refTaskTreeModelFilter->set_visible_func( sigc::mem_fun(*this,
//				&MainWindow::on_taskTreeview_filter_row_visible) );
//	} else {
//		// TODO: doesn't work...
//		refTaskTreeModelFilter->set_visible_func( sigc::hide(sigc::_1(true)) );
//	}
//}
