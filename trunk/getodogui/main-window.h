// $Id$
//
// Class MainWindow
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008-2009
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef GETODOGUI_MAIN_WINDOW_H
#define GETODOGUI_MAIN_WINDOW_H

#include "getodo.h"
#include "getodogui.h"
#include "taskmodel.h"
#include "tagmodel.h"
#include "filtermodel.h"

namespace getodo {

// ----- class MainWindow --------------------

/** GeToDo main window.
 * This is the main Gtk::Window of the application. It is loaded from a Glade
 * file as a derived widget. The instance contains a reference to TaskManager.
 *
 * There are several main parts:
 * - task TreeView
 * - tag TreeView
 * - filter TreeView
 * - task editing panel
 * - button toolbar
 *
 * Tasks can be filtered in two ways. You can filter by tags and/or using
 * filter rules. In either case multiple items can be combined using
 * logic union or intersection. Eg. show tasks that match _all_ selected
 * tags and _any_ of selected rules.
 *
 * Note: child widgets should be destroyed automatically according to GTK docs,
 * but this one is a top-level widget and should be then destroyed by hand.
 * Due to some unknown reasons deleting MainWindows instance result in an
 * error. This is an open problem for now. (The same holds for the other
 * top-level widget like Gtk::Window or Gtk::Dialog and their derivates.
 */
class MainWindow : public Gtk::Window
{
public:
	/** MainWindow constructor.
	 * The instance is usually created by Glade using get_widget_derived().
	 * The child widgets are instantiated here using Glade.
	 */
	MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~MainWindow();

	/** Set task manager.
	 * Use a new task manager and initialize all the models according to it.
	 * Also initialize various TreeView columns but only once (at the first
	 * time calling).
	 *
	 * \param manager valid TaskManager instance
	 */
	void setTaskManager(boost::shared_ptr<TaskManager> manager);
protected:
	/** Glade XML object using which are child widgets instantiated. */
	Glib::RefPtr<Gnome::Glade::Xml> refXml;

	// ---- task tree view ----

	/** %Task TreeView.
	 * Uses sorted and filtered TaskTreeStore as its model.
	 */
	Gtk::TreeView* pTaskTreeView;

	/** Basic model for task tree view
	 * It receives signals from TaskManager and change itself accordingly.
	 * The model is wrapped in TreeModelSort and TreeModelFilter to support
	 * sorting and filtering.
	 */
	Glib::RefPtr<TaskTreeStore> refTaskTreeModel;
	Glib::RefPtr<Gtk::TreeModelSort> refTaskTreeModelSort;
	Glib::RefPtr<Gtk::TreeModelFilter> refTaskTreeModelFilter;

	/** Selection change handler for task treeview.
	 * Show selected task in the editing panel.
	 */
	void on_taskTreeview_selection_changed();
	/** Filtering callback for task treeview.
	 * %Task on row given by \p iter is visible if it passed the currently
	 * active filtering rule as set in task manager.
	 *
	 * Filtering happens only when it is enabled in #filteringActive.
	 *
	 * \return true if task is visible
	 */
	bool on_taskTreeview_filter_row_visible(const Gtk::TreeModel::const_iterator& iter);

	// ---- task editing panel ----

	// TODO: change some widgets' type from the dummy Entry to something reasonable
	Gtk::Entry* pTaskDescriptionEntry;
	Gtk::TextView* pTaskLongDescriptionTextView;
	Glib::RefPtr<Gtk::TextBuffer> refTaskLongDescriptionTextBuffer;
	Gtk::Entry* pTaskTagsEntry;
	Gtk::CheckButton* pTaskDoneCheckbutton;
	Gtk::SpinButton* pTaskCompletedPercentageSpinbutton;
	Gtk::SpinButton* pTaskPrioritySpinbutton; // SpinButton or ComboBox, colorful at best
	//Gtk::Label* pTaskRecurrenceLabel;
	Gtk::Entry* pTaskRecurrenceEntry;
	Gtk::Label* pTaskIdLabel;
	Gtk::Entry* pTaskDateDeadlineEntry; // future: DateEntry (from GDA)
	Gtk::Entry* pTaskDateStartedEntry; // future: DateEntry (from GDA)
	Gtk::Entry* pTaskDateCompletedEntry; // future: DateEntry (from GDA)
	Gtk::Label* pTaskDateCreatedLabel;
	Gtk::Label* pTaskDateLastModifiedLabel;

	/** %Task editing panel widgets are automatically saved when they lose focus. */
	bool on_taskDescriptionEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry);
	bool on_taskLongDescriptionTextview_focus_out_event(GdkEventFocus* event, Gtk::TextView* textview);
	//bool on_taskTagsEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry);
	void on_taskDoneCheckbutton_toggled();
	bool on_taskCompletedPercentageSpinbutton_focus_out_event(GdkEventFocus* event, Gtk::SpinButton* spinbutton);
	bool on_taskPrioritySpinbutton_focus_out_event(GdkEventFocus* event, Gtk::SpinButton* spinbutton);
	bool on_taskRecurrenceEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry);
	bool on_taskDateDeadlineEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry);
	bool on_taskDateStartedEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry);
	bool on_taskDateCompletedEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry);

	/** Show recurrence editing dialog. */
	void on_buttonRecurrence_clicked();

	// ---- filtering panel - tags ----
	
	/** %Tag TreeView.
	 * Uses sorted TagListStore as its model.
	 */
	Gtk::TreeView* pTagTreeView;

	/** Basic model for tag tree view.
	 * It receives signals from TaskManager and change itself accordingly.
	 * The model is wrapped in TreeModelSort to support sorting.
	 */
	Glib::RefPtr<TagListStore> refTagListStore;

	Glib::RefPtr<Gtk::TreeModelSort> refTagListModelSort;

	/** %Tag name cell rendered. Used to support in place editing. */
	Gtk::CellRendererText tagNameCellRenderer;

	Gtk::TreeViewColumn tagNameTreeViewColumn;

	/** Show tag name in the column.
	 * This is needed as a custom tag name CellRenderer is used for editing.
	 */
	void on_tagNameTreeViewColumn_cell_data(Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter);

	/** Save edited tag name. */
	void on_tagNameCellRenderer_edited(const Glib::ustring& pathString, const Glib::ustring& newText);

	/** Enable or disable filtering by tags. */
	Gtk::ToggleButton* pTagFilterToggleButton;
	
	/** Select whether to join tags using union or intersection. */
	Gtk::RadioButton* pTagFilterAllRadiobutton;

	void on_buttonTagFilter_toggled();
	void on_radioTagFilterAll_toggled();

	/** Update activeTagFilter according to what is selected. */
	void on_tagTreeview_selection_changed();
	
	// ---- filtering panel - filter rules ----
	
	/** Filter rule TreeView.
	 * Uses sorted FilterListStore as its model.
	 */
	Gtk::TreeView* pFilterTreeView;

	/** Basic model for filter rule tree view.
	 * It receives signals from TaskManager and change itself accordingly.
	 * The model is wrapped in TreeModelSort to support sorting.
	 */
	Glib::RefPtr<FilterListStore> refFilterListStore;
	Glib::RefPtr<Gtk::TreeModelSort> refFilterListModelSort;

	/** Enable or disable filtering by filter rule. */
	Gtk::ToggleButton* pRuleFilterToggleButton;

	/** Select whether to join filter rules using union or intersection. */
	Gtk::RadioButton* pRuleFilterAllRadiobutton;

	void on_buttonRuleFilter_toggled();
	void on_radioRuleFilterAll_toggled();

	/** Create new filter rule.
	 * Use FilterDialog.
	 */
	void on_buttonRuleFilterNew_clicked();

	/** Edit existing filter rule.
	 * Take first of the selected filter rules and edit them using
	 * FilterDialog.
	 */
	void on_buttonRuleFilterEdit_clicked();

	/** Delete selected filter rules. */
	void on_buttonRuleFilterDelete_clicked();

	/** Update activeRuleFilter according to what is selected. */
	void on_filterTreeview_selection_changed();

	// ---- toolbar ----

	/* Enable or disable filtering altogether. */
	Gtk::ToggleToolButton* pTaskFilterToggletoolbutton;

	/** Create new top-level task. */
	void on_buttonTaskNewToplevel_clicked();

	/** Create new task.
	 * Create new task on the same level as is the selected one
	 * or top-level task if none selected.
	 */
	void on_buttonTaskNew_clicked();

	/** Create new subtask of selected task.
	 * Create new top-level task if nothing selected.
	 */
	void on_buttonTaskNewSubtask_clicked();

	/** Delete selected task. */
	void on_buttonTaskDelete_clicked();

	/** Update task being edited.
	 * Save contents of task editing panel to database.
	 */
	void on_buttonTaskUpdate_clicked();

	/* Generate next task by recurrence.
	 * Copy selected task to new one and set deadline date according to
	 * next date from recurrence.
	 * Not applicable if no task selected or there is no recurrence or
	 * deadline date.
	 */
	void on_buttonTaskNextByRecurrence_clicked();

	/* Enable or disable filtering altogether. */
	void on_buttonTaskFilter_toggled();
private:
	/** %Task manager.
	 * This is a vital component used to hold all the data and support
	 * database persistence.
	 */
	boost::shared_ptr<TaskManager> taskManager;

	// ---- filtering ----

	/** Filtering active.
	 * If enabled tasks are filered using active filter rule set in
	 * the TaskManager.
	 */
	bool filteringActive;

	/**  Filtering using tags active. */
	bool tagFilterActive;

	/**  Filtering using filter rules active. */
	bool ruleFilterActive;

	/**  %Tag filter joining mode.
	 * - true = ALL (intersection)
	 * - false = ANY (union)
	 */
	bool tagFilterAll;

	/**  Rule filter joining mode.
	 * - true = ALL (intersection)
	 * - false = ANY (union)
	 */
	bool ruleFilterAll;

	/** Active tag filter.
	 * Filter rule made by joining selected tags.
	 */
	FilterRule activeTagFilter;

	/** Active rule filter.
	 * Filter rule made by joining selected filter rules.
	 */
	FilterRule activeRuleFilter;

	/** Set active filter.
	 * From #activeTagFilter, #activeRuleFilter take those which are enabled,
	 * intersect them and set to task manager as active filter rule.
	 */
	void setActiveFilter();

	/** Make a filter rule from selected tags and set to #activeTagFilter. */
	void setFilterFromTagSelection();

	/** Make a filter rule from selected filter rules and set to #activeRuleFilter. */
	void setFilterFromRuleSelection();
	
	//void toggleFiltering(bool on); // enable filtering on true

	// ---- task editing panel ----

	/** Lock for editing panel.
	 * - true, if signal handler should respond to event from this panel
	 * - false, if there is some batch filling or cleaning of the panel
	 */
	bool editingPanelActive;

	/** Fill editing panel with contents of \p task.
	 * \param task task used to fill the panel
	 */
	void fillEditingPanel(Task& task);

	/** Clear editing panel. */
	void clearEditingPanel();

	/** Save contents of editing panel to \p task.
	 * \param task task where to save the panel contents
	 */
	void saveEditingPanelToTask(Task& task);

	/** Get id of task being currently edited in the panel.
	 * \return edited task id
	 */
	id_t getCurrentlyEditedTaskId();
	
	/** Update one field of currenctly edited task.
	 * The purpose is to update only what is needed, not the whole task.
	 * Call an arbitrary function from TaskPersistence with its arguments.
	 *
	 * Example:
	 * \code
	 * updateTaskPartial(boost::bind( &TaskPersistence::setDescription, _1,
	 *   boost::ref(entry->get_text()) ));
	 * \endcode
	 *
	 * \param f functor of a TaskPersistence function
	 * \return true if the update ran alright
	 */
	bool updateTaskPartial(boost::function<void(TaskPersistence&)> f);

	/** Select task in the task TreeView.
	 * \param iter TaskTreeModel iterator
	 */
	void selectTask(Gtk::TreeModel::iterator iter);

	/* Create a new task.
	 * \param newTask new task
	 */
	void createNewTask(const Task& newTask, id_t parentId = Task::INVALID_ID);
};

} // namespace getodo

#endif // GETODOGUI_MAIN_WINDOW_H
