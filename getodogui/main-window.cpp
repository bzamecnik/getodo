// $Id$

#include "stdafx.h"
#include "main-window.h"

namespace getodo {

// ----- class MainWindow --------------------

MainWindow::MainWindow(BaseObjectType* cobject,
	const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
: Gtk::Window(cobject), refXml(refGlade),
  filteringActive(false), tagFilterActive(false), ruleFilterActive(false)
// TODO: initialize all widget pointers to 0
{
	Gtk::ToolButton* pTaskNewToplevelToolbutton = 0;
	Gtk::ToolButton* pTaskNewToolbutton = 0;
	Gtk::ToolButton* pTaskNewSubtaskToolbutton = 0;
	Gtk::ToolButton* pTaskDeleteToolbutton = 0;
	Gtk::ToolButton* pTaskUpdateToolbutton = 0;
	Gtk::ToolButton* pTaskNextByRecurrenceToolbutton = 0;

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
		refXml->get_widget("taskNextByRecurrenceToolbutton", pTaskNextByRecurrenceToolbutton);
		refXml->get_widget("taskFilterToggletoolbutton", pTaskFilterToggletoolbutton);

		refXml->get_widget("tagFilterTogglebutton", pTagFilterToggleButton);
		refXml->get_widget("ruleFilterAllRadiobutton", pRuleFilterAllRadiobutton);
		refXml->get_widget("ruleFilterTogglebutton", pRuleFilterToggleButton);
		refXml->get_widget("tagFilterAllRadiobutton", pTagFilterAllRadiobutton);

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
		pTaskNextByRecurrenceToolbutton->signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonTaskNextByRecurrence_clicked) );
		pTaskFilterToggletoolbutton->signal_toggled().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonTaskFilter_toggled) );
		
		pTagFilterToggleButton->signal_toggled().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonTagFilter_toggled) );
		pRuleFilterToggleButton->signal_toggled().connect(
			sigc::mem_fun(*this, &MainWindow::on_buttonRuleFilter_toggled) );

		pTagFilterAllRadiobutton->signal_toggled().connect(
			sigc::mem_fun(*this, &MainWindow::on_radioTagFilterAll_toggled) );
		pRuleFilterAllRadiobutton->signal_toggled().connect(
			sigc::mem_fun(*this, &MainWindow::on_radioRuleFilterAll_toggled) );

		refXml->connect_clicked("taskRecurrenceButton",
			sigc::mem_fun(*this, &MainWindow::on_buttonRecurrence_clicked) );

		refXml->connect_clicked("ruleFilterNewButton",
			sigc::mem_fun(*this, &MainWindow::on_buttonRuleFilterNew_clicked) );

		refXml->connect_clicked("ruleFilterEditButton",
			sigc::mem_fun(*this, &MainWindow::on_buttonRuleFilterEdit_clicked) );

		refXml->connect_clicked("ruleFilterDeleteButton",
			sigc::mem_fun(*this, &MainWindow::on_buttonRuleFilterDelete_clicked) );

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
		pTaskDoneCheckbutton->signal_toggled().connect(
			sigc::mem_fun(*this, &MainWindow::on_taskDoneCheckbutton_toggled)
			);
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
	pTagTreeView->get_selection()->signal_changed().connect(sigc::mem_fun(*this,
		&MainWindow::on_tagTreeview_selection_changed) );
}

MainWindow::~MainWindow() {}

void MainWindow::setTaskManager(boost::shared_ptr<TaskManager> manager) {
	if (!manager || !pTaskTreeView) { return; } 
	// set a new model for task TreeView
	taskManager = manager;

	// ---- task treeview ----
	// a custom tree store
	refTaskTreeModel = TaskTreeStore::create(*taskManager);
	
	// treestore packed into a filter model
	refTaskTreeModelFilter = Gtk::TreeModelFilter::create(refTaskTreeModel);

	// TODO: default should be unfiltered
	refTaskTreeModelFilter->set_visible_func( sigc::mem_fun(*this,
		&MainWindow::on_taskTreeview_filter_row_visible) );
	// need to pack the filter model inside a sort model

	refTaskTreeModelSort = Gtk::TreeModelSort::create(refTaskTreeModelFilter);
	// TODO: think of what will be the default sorting column
	refTaskTreeModelSort->set_sort_column(refTaskTreeModel->columns.dateDeadline, Gtk::SORT_DESCENDING);

	pTaskTreeView->set_model(refTaskTreeModelSort);
	
	// Add TreeView columns when setting a model for the first time.
	// The model columns don't change, so it's ok to add them only once.
	if (pTaskTreeView->get_columns().empty()) {
		TaskTreeStore::ModelColumns& columns = refTaskTreeModel->columns;

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
	// tag model
	refTagListStore = TagListStore::create(*taskManager);
	//pTagTreeView->set_model(refTagListStore);

	pTagTreeView->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	
	// with sorting
	refTagListModelSort = Gtk::TreeModelSort::create(refTagListStore);
	refTagListModelSort->set_sort_column(refTagListStore->columns.name, Gtk::SORT_ASCENDING);
	pTagTreeView->set_model(refTagListModelSort);

	if (pTagTreeView->get_columns().empty()) {
		//pTagTreeView->append_column("Active", refTagListStore->columns.active);
		
		// name column is editable
		tagNameTreeViewColumn.set_title("Name");
		tagNameTreeViewColumn.pack_start(tagNameCellRenderer);
		pTagTreeView->append_column(tagNameTreeViewColumn);

		tagNameCellRenderer.set_property("editable", true);

		tagNameTreeViewColumn.set_cell_data_func(tagNameCellRenderer,
			sigc::mem_fun(*this, &MainWindow::on_tagNameTreeViewColumn_cell_data) );

		tagNameCellRenderer.signal_edited().connect( sigc::mem_fun(*this,
			&MainWindow::on_tagNameCellRenderer_edited) );
	}

	// ---- filter treeview ----
	if (!pFilterTreeView) { return; }
	// filter model
	refFilterListStore = FilterListStore::create(*taskManager);
	// sorting
	refFilterListModelSort = Gtk::TreeModelSort::create(refFilterListStore);
	refFilterListModelSort->set_sort_column(refFilterListStore->columns.name, Gtk::SORT_ASCENDING);
	
	pFilterTreeView->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);

	pFilterTreeView->set_model(refFilterListModelSort);

	if (pFilterTreeView->get_columns().empty()) {
		FilterListStore::ModelColumns& columns = refFilterListStore->columns;
		pFilterTreeView->append_column("Filter name", columns.name);
	}

	// ---- task long description textview ----
	if (!pTaskLongDescriptionTextView) { return; }
	pTaskLongDescriptionTextView->set_buffer(refTaskLongDescriptionTextBuffer);
}

void MainWindow::on_taskTreeview_selection_changed() {
	// display the selected task's contents in the task editing panel
	Gtk::TreeModel::iterator iter = pTaskTreeView->get_selection()->get_selected();
	boost::shared_ptr<Task> task;
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
	setFilterFromRuleSelection();
}

void MainWindow::on_tagTreeview_selection_changed() {
	setFilterFromTagSelection();
}

void MainWindow::on_buttonTaskNewToplevel_clicked() {
	// Create a new top level task and start editing it.
	createNewTask(Task());
}

void MainWindow::on_buttonTaskNew_clicked() {
	// create a task on the same level as selected task
	if (!taskManager) { return; }

	// find out currently selected task
	id_t selectedTaskId = getCurrentlyEditedTaskId();
	boost::shared_ptr<Task> selectedTask = taskManager->getTask(selectedTaskId);
	Task newTask;
	if (selectedTask) {
		createNewTask(newTask, selectedTask->getParentId());
	} else {
		// make a new top level task instead
		createNewTask(newTask);
	}
}

void MainWindow::on_buttonTaskNewSubtask_clicked() {
	// Create a new subtask or currently selected task and start editing it.
	// Create new top level task if nothing selected.
	if (!taskManager) { return; }

	id_t selectedTaskId = getCurrentlyEditedTaskId();
	createNewTask(Task(), selectedTaskId);
}

void MainWindow::on_buttonTaskDelete_clicked() {
	// delete currently selected task
	if (!taskManager) { return; }

	Gtk::MessageDialog question("Really delete this task and all its subtasks?",
		false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
	if (question.run() == Gtk::RESPONSE_YES) {
		// TODO: or delete multiple selected rows (use get_selected_rows())
		Gtk::TreeModel::iterator iter = pTaskTreeView->get_selection()->get_selected();
		if (iter) {
			taskManager->deleteTask((*iter)[refTaskTreeModel->columns.id]);
			
			// TODO: select next row (will that work, when using a sorted model?)
		}
	}
}

void MainWindow::on_buttonTaskUpdate_clicked() {
	// save editing panel contents
	if (!taskManager) { return; }
	id_t taskId = getCurrentlyEditedTaskId();
	boost::shared_ptr<Task> updatedTask = taskManager->getTask(taskId);
	if (updatedTask) {
		saveEditingPanelToTask(*updatedTask);
		taskManager->editTask(taskId, *updatedTask);
	}
}

void MainWindow::on_buttonTaskNextByRecurrence_clicked() {
	if (!taskManager) { return; }
	id_t taskId = getCurrentlyEditedTaskId();
	boost::shared_ptr<Task> selectedTask = taskManager->getTask(taskId);
	if (selectedTask) {
		Date deadlineDate = selectedTask->getDateDeadline();
		Date nextDate = selectedTask->getRecurrence().next(deadlineDate);
		
		if (!nextDate.date.is_not_a_date()) {
			boost::shared_ptr<Task> newTask = selectedTask->copyAsNew();
			newTask->setDateDeadline(nextDate);
			createNewTask(*newTask);
		} else {
			Gtk::MessageDialog("Can't create a copy of task using the recurrence. Such a next date is not valid.").run();
		}
	}
}

void MainWindow::on_buttonTaskFilter_toggled() {
	if (!taskManager) { return; }

	//std::cout << "on_buttonTaskFilter_toggled" << std::endl; // DEBUG
	filteringActive = pTaskFilterToggletoolbutton->get_active();
	if (filteringActive && !taskManager->hasActiveFilterRule()) {
		pTaskFilterToggletoolbutton->set_active(false);
	} else {
		refTaskTreeModelFilter->refilter();
	}
}

void MainWindow::on_buttonTagFilter_toggled() {
	tagFilterActive = pTagFilterToggleButton->get_active();
	setActiveFilter();
}

void MainWindow::on_buttonRuleFilter_toggled() {
	ruleFilterActive = pRuleFilterToggleButton->get_active();
	setActiveFilter();
}

void MainWindow::on_radioTagFilterAll_toggled() {
	tagFilterAll = pTagFilterAllRadiobutton->get_active();
	setFilterFromTagSelection();
}

void MainWindow::on_radioRuleFilterAll_toggled() {
	ruleFilterAll = pRuleFilterAllRadiobutton->get_active();
	setFilterFromRuleSelection();
}

void MainWindow::on_tagNameTreeViewColumn_cell_data(
	Gtk::CellRenderer* renderer,
	const Gtk::TreeModel::iterator& iter)
{
	if(iter) {
		Glib::ustring tagName = (*iter)[refTagListStore->columns.name];
		tagNameCellRenderer.set_property("text", tagName);
	}
}

void MainWindow::on_tagNameCellRenderer_edited(
	const Glib::ustring& pathString,
	const Glib::ustring& newText)
{
	if (!taskManager) { return; }

	Gtk::TreePath path(pathString);
	if (!newText.empty()) {
		Gtk::TreeModel::iterator iter = refTagListModelSort->get_iter(path);
		if (iter) {
			// save the new tag name
			Gtk::TreeRow row = *iter;			
			Tag editedTag = Tag(row[refTagListStore->columns.id],
				std::string(newText.c_str()));
			taskManager->editTag(editedTag.id, editedTag);
			// no need to update this, as is will be updated automatically
			//row[refTagListStore->columns.name] = newText;
		}
	} else {
		// error message & edit again
		Gtk::MessageDialog dialog(*this, "Tag name must not be empty.",
			false, Gtk::MESSAGE_ERROR);
		dialog.run();
		pTagTreeView->set_cursor(path, tagNameTreeViewColumn,
			tagNameCellRenderer, true /* start_editing */);
	}
}

// event handling to save individual items of editing panel

bool MainWindow::on_taskDescriptionEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry) {
	if (!entry || !editingPanelActive) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setDescription, _1,
		boost::ref(entry->get_text()) ));
}

bool MainWindow::on_taskLongDescriptionTextview_focus_out_event(GdkEventFocus* event, Gtk::TextView* textview) {
	if (!textview || !editingPanelActive) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setLongDescription, _1,
		boost::ref(textview->get_buffer()->get_text()) ));
}

//bool MainWindow: on_taskTagsEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry) {
// //TODO: we need TaskPersistence.setTagsFromString()
//}

void MainWindow::on_taskDoneCheckbutton_toggled() {
	if (!taskManager || !editingPanelActive) { return; }

	updateTaskPartial(boost::bind( &TaskPersistence::setDone, _1,
		pTaskDoneCheckbutton->get_active() ));
	// TODO: this is not nice
	id_t taskId = getCurrentlyEditedTaskId();
	boost::shared_ptr<Task> task = taskManager->getTask(taskId);
	if (task) {
		pTaskCompletedPercentageSpinbutton->set_value(task->getCompletedPercentage());
	}
}

bool MainWindow::on_taskCompletedPercentageSpinbutton_focus_out_event(GdkEventFocus* event, Gtk::SpinButton* spinbutton) {
	if (!spinbutton || !editingPanelActive) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setCompletedPercentage, _1,
		spinbutton->get_value_as_int() ));
}

bool MainWindow::on_taskPrioritySpinbutton_focus_out_event(GdkEventFocus* event, Gtk::SpinButton* spinbutton) {
	if (!spinbutton || !editingPanelActive) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setPriority, _1,
		spinbutton->get_value_as_int() ));
}

bool MainWindow::on_taskRecurrenceEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry) {
	if (!entry || !editingPanelActive) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setRecurrence, _1,
		boost::ref(*Recurrence::fromString(entry->get_text())) ));
}

bool MainWindow::on_taskDateDeadlineEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry) {
	if (!entry || !editingPanelActive) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setDateDeadline, _1,
		boost::ref(Date(entry->get_text())) ));
}

bool MainWindow::on_taskDateStartedEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry) {
	if (!entry || !editingPanelActive) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setDateStarted, _1,
		boost::ref(Date(entry->get_text())) ));
}

bool MainWindow::on_taskDateCompletedEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry) {
	if (!entry || !editingPanelActive) { return false; }
	return updateTaskPartial(boost::bind( &TaskPersistence::setDateCompleted, _1,
		boost::ref(Date(entry->get_text())) ));
}

void MainWindow::on_buttonRecurrence_clicked() {
	RecurrenceDialog* dialog = 0;
	dialog = GeToDoApp::getSingleton().getWidget("recurrenceDialog", dialog);
	if (dialog == 0) {
		return;
	}
	dialog->setRecurrence(*Recurrence::fromString(
		pTaskRecurrenceEntry->get_text()));
	int response = dialog->run();
	if (response == Gtk::RESPONSE_OK) {
		pTaskRecurrenceEntry->set_text(Recurrence::toString(dialog->getRecurrence()));
	}
	updateTaskPartial(boost::bind( &TaskPersistence::setRecurrence, _1,
		boost::ref(dialog->getRecurrence()) ));
}

void MainWindow::on_buttonRuleFilterNew_clicked() {
	if (!taskManager) { return; }
	FilterDialog* dialog = 0;
	dialog = GeToDoApp::getSingleton().getWidget("filterDialog", dialog);
	if (dialog == 0) {
		return;
	}
	int response = dialog->run();
	if (response == Gtk::RESPONSE_OK) {
		// add the new filter rule to the taskmanager
		FilterRule newFilter = dialog->getFilterRule();
		if (!newFilter.isEmpty()) {
			taskManager->addFilterRule(newFilter);
		}
	}
}

void MainWindow::on_buttonRuleFilterEdit_clicked() {
	if (!taskManager) { return; }
	FilterDialog* dialog = 0;
	dialog = GeToDoApp::getSingleton().getWidget("filterDialog", dialog);
	if (dialog == 0) {
		return;
	}
	Gtk::TreeSelection::ListHandle_Path selectedRows = pFilterTreeView->get_selection()->get_selected_rows();
	// try to get the first selected row
	Gtk::TreeSelection::ListHandle_Path::iterator pathIter = selectedRows.begin();
	if (pathIter == selectedRows.end()) {
		return; // no rows selected
	}
	Gtk::TreeIter sortIter = refFilterListModelSort->get_iter(*pathIter);
	Gtk::TreeIter iter = refFilterListModelSort->convert_iter_to_child_iter(sortIter);
	id_t filterId = FilterRule::INVALID_ID;
	if (iter) {
		filterId = (*iter)[refFilterListStore->columns.id];
	}
	FilterRule selectedFilter;
	if (!taskManager->hasFilterRule(filterId)) {
		return;
	}
	selectedFilter = taskManager->getFilterRule(filterId);

	dialog->setFilterRule(selectedFilter);
	int response = dialog->run();
	if (response == Gtk::RESPONSE_OK) {
		// update the filter rule in taskmanager
		FilterRule updatedFilter = dialog->getFilterRule();
		if (!updatedFilter.isEmpty()) {
			taskManager->editFilterRule(filterId, updatedFilter);
		}
		setFilterFromRuleSelection();
	}
}

void MainWindow::on_buttonRuleFilterDelete_clicked() {
	if (!taskManager) { return; }
	Gtk::MessageDialog dialog(*this, "Really delete these filter rules?",
		false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
	int response = dialog.run();
	if (response == Gtk::RESPONSE_OK) {
		Gtk::TreeSelection::ListHandle_Path selectedRows =
			pFilterTreeView->get_selection()->get_selected_rows();
		foreach(Gtk::TreePath path, selectedRows) {
			Gtk::TreeIter sortIter = refFilterListModelSort->get_iter(path);
			Gtk::TreeIter iter = refFilterListModelSort->convert_iter_to_child_iter(sortIter);
			if (iter) {
				taskManager->deleteFilterRule((*iter)[refFilterListStore->columns.id]);
			}
		}
	}
}

bool MainWindow::on_taskTreeview_filter_row_visible(const Gtk::TreeModel::const_iterator& iter) {
	if (!taskManager || !filteringActive) { return true; }
	if (iter) {
		//iter seems to be an iter to the child model:
		//Gtk::TreeModel::iterator iter_child =
			//refTaskTreeModelFilter->convert_iter_to_child_iter(iter);
		//if(iter_child)
		//{
		Gtk::TreeModel::Row row = *iter;
		return taskManager->isTaskVisible(row[refTaskTreeModel->columns.id]);
		// DEBUG:
		//id_t taskId = row[refTaskTreeModel->columns.id];
		//bool visible = taskManager->isTaskVisible(taskId);
		// std::cout << "visible: " << taskId << " - " << visible << std::endl;
		//return visible;
		
		//}
	}
	return true;
}

void MainWindow::setActiveFilter() {
	if (!taskManager) { return; }

	std::vector<FilterRule> activeFilters;
	
	if (!tagFilterActive && !ruleFilterActive) {
		pTaskFilterToggletoolbutton->set_active(false);
		taskManager->resetActiveFilterRule();
	} else {
		if (tagFilterActive) {
			activeFilters.push_back(activeTagFilter);
		}
		if (ruleFilterActive) {
			activeFilters.push_back(activeRuleFilter);
		}
		//filteringActive = true;
		FilterRule activefilter = FilterBuilder::intersectFilters(activeFilters);
		//std::cout << "active filter: " << activefilter.rule << " "; // DEBUG
		try {
			taskManager->setActiveFilterRule(activefilter);
			//std::cout << "(OK)" << std::endl;
		} catch (GetodoError&) {
			pTaskFilterToggletoolbutton->set_active(false);
			//std::cout << "(bad)" << std::endl;
		}
	}
	refTaskTreeModelFilter->refilter();
}

void MainWindow::setFilterFromTagSelection() {
	//Gtk::TreeModel::iterator iter = pTagTreeView->get_selection()->get_selected();
	//
	//Gtk::TreeModel::Row row = *iter;
	//id_t tagId = FilterRule::INVALID_ID;
	//if (iter) {
	//	tagId = row[refTagListStore->columns.id];
	//}
	////std::cout << "tag id: " << tagId << std::endl; // DEBUG
	//if (taskManager->hasTag(tagId)) {
	//	FilterRule filter = FilterBuilder::createTagFilter(tagId);
	//	taskManager->setActiveFilterRule(filter);
	//	filteringActive = true;
	//	//std::cout << "tag filter: " << filter << std::endl; // DEBUG
	//	refTaskTreeModelFilter->refilter();
	//}

	// for multiple selection
	std::vector<FilterRule> tagFilters;

	Gtk::TreeSelection::ListHandle_Path selectedRows = pTagTreeView->get_selection()->get_selected_rows();
	foreach(Gtk::TreePath path, selectedRows) {
		Gtk::TreeIter sortIter = refTagListModelSort->get_iter(path);
		Gtk::TreeIter iter = refTagListModelSort->convert_iter_to_child_iter(sortIter);
		id_t tagId = FilterRule::INVALID_ID;
		if (iter) {
			tagId = (*iter)[refTagListStore->columns.id];
		}
		if (taskManager->hasTag(tagId)) {
			FilterRule filter = FilterBuilder::createTagFilter(tagId);
			tagFilters.push_back(filter);
		}
	}
	if (!tagFilters.empty()) {
		// join them using UNION or INTERSECT according to 'All' and 'Any' buttons
		if (tagFilterAll) {
			activeTagFilter = FilterBuilder::intersectFilters(tagFilters);
		} else {
			activeTagFilter = FilterBuilder::unionFilters(tagFilters);
		}
	} else {
		activeTagFilter = FilterRule();
	}
	setActiveFilter();
}

void MainWindow::setFilterFromRuleSelection() {
	//Gtk::TreeModel::iterator iter = pFilterTreeView->get_selection()->get_selected();
	//// enable the toggle button if anything selected, disable if nothing
	//// this will activate the selected filter rule
	//if (iter != 0) {
	//	setFilterRuleFromSelection();
	//}
	//pTaskFilterToggletoolbutton->set_active(iter != 0);
	
	// for multiple selection
	std::vector<FilterRule> ruleFilters;

	Gtk::TreeSelection::ListHandle_Path selectedRows = pFilterTreeView->get_selection()->get_selected_rows();
	foreach(Gtk::TreePath path, selectedRows) {
		Gtk::TreeIter sortIter = refFilterListModelSort->get_iter(path);
		Gtk::TreeIter iter = refFilterListModelSort->convert_iter_to_child_iter(sortIter);
		id_t filterId = FilterRule::INVALID_ID;
		if (iter) {
			filterId = (*iter)[refFilterListStore->columns.id];
		}
		if (taskManager->hasFilterRule(filterId)) {
			ruleFilters.push_back(taskManager->getFilterRule(filterId));
		}
	}
	if (!ruleFilters.empty()) {
		// join them using UNION or INTERSECT according to 'All' and 'Any' buttons
		if (ruleFilterAll) {
			activeRuleFilter = FilterBuilder::intersectFilters(ruleFilters);
		} else {
			activeRuleFilter = FilterBuilder::unionFilters(ruleFilters);
		}
	} else {
		activeRuleFilter = FilterRule();
	}
	setActiveFilter();
}

void MainWindow::fillEditingPanel(Task& task) {
	if (!taskManager) { return; }
	
	editingPanelActive = false;

	pTaskDescriptionEntry->set_text(task.getDescription());
	//refTaskLongDescriptionTextBuffer->set_text(task.getLongDescription());
	pTaskLongDescriptionTextView->get_buffer()->set_text(task.getLongDescription());
	pTaskTagsEntry->set_text(task.getTagsAsString(*taskManager));
	pTaskDoneCheckbutton->set_active(task.isDone());
	pTaskCompletedPercentageSpinbutton->set_value(task.getCompletedPercentage());
	pTaskPrioritySpinbutton->set_value(task.getPriority());
	//// without recurrence type id
	//pTaskRecurrenceEntry->set_text(task.getRecurrence().toString());
	// with recurrence type id
	pTaskRecurrenceEntry->set_text(Recurrence::toString(task.getRecurrence()));
	pTaskIdLabel->set_text(boost::lexical_cast<std::string,int>(task.getTaskId()));
	pTaskDateDeadlineEntry->set_text(task.getDateDeadline().toString());
	pTaskDateStartedEntry->set_text(task.getDateStarted().toString());
	pTaskDateCompletedEntry->set_text(task.getDateCompleted().toString());
	pTaskDateCreatedLabel->set_text(task.getDateCreated().toString());
	pTaskDateLastModifiedLabel->set_text(task.getDateLastModified().toString());

	editingPanelActive = true;
}

void MainWindow::clearEditingPanel() {
	editingPanelActive = false;

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

	editingPanelActive = true;
}

void MainWindow::saveEditingPanelToTask(Task& task) {
	if (!taskManager) { return; }

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
	
	refTaskTreeModelFilter->refilter();
}

id_t MainWindow::getCurrentlyEditedTaskId() {
	try {
		return boost::lexical_cast<id_t, std::string>(pTaskIdLabel->get_text());
	} catch (boost::bad_lexical_cast) {
		return Task::INVALID_ID;
	}
}

bool MainWindow::updateTaskPartial(boost::function<void(TaskPersistence&)> f) {
	id_t taskId = getCurrentlyEditedTaskId();
	if (!taskManager || !taskManager->hasTask(taskId)) {
		return false;
	}
	TaskPersistence& tp = taskManager->getPersistentTask(taskId);
	f(tp);
	boost::shared_ptr<Task> task = tp.getTask();
	if (!task) { return false; }
	pTaskDateLastModifiedLabel->set_text(task->getDateLastModified().toString());
	taskManager->signal_task_updated(*task);
	return true;
}

void MainWindow::selectTask(Gtk::TreeModel::iterator iter) {
	Gtk::TreeModel::iterator filterIter = refTaskTreeModelFilter->convert_child_iter_to_iter(iter);
	Gtk::TreeModel::iterator sortIter = refTaskTreeModelSort->convert_child_iter_to_iter(filterIter);
	Gtk::TreeModel::Path path = refTaskTreeModelSort->get_path(sortIter);
	pTaskTreeView->expand_to_path(path);
	pTaskTreeView->get_selection()->select(path);
	pTaskTreeView->scroll_to_row(path);

	pTaskDescriptionEntry->grab_focus();
}

void MainWindow::createNewTask(const Task& newTask, id_t parentId) {
	if (!taskManager) { return; }

	// disable filtering, otherwise errors occur (TODO: handle that)
	pTaskFilterToggletoolbutton->set_active(false);

	// register the new task
	id_t newTaskId = taskManager->addTask(newTask);
	boost::shared_ptr<Task> newTaskCopy = taskManager->getTask(newTaskId);
	if (!newTaskCopy) { return; }
	fillEditingPanel(*newTaskCopy);
	
	if (parentId != Task::INVALID_ID) {
		// make the new task a child of given parent
		newTaskCopy->setParent(parentId, *taskManager);
	}

	// select it
	selectTask(refTaskTreeModel->getIterByTask(*newTaskCopy));
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

} // namespace getodo
