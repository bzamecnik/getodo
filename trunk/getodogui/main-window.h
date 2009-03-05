// $Id$

#ifndef GETODOGUI_MAIN_WINDOW_H
#define GETODOGUI_MAIN_WINDOW_H

#include "getodo.h"
#include "getodogui.h"
#include "tagmodel.h"
#include "taskmodel.h"

// ----- class MainWindow --------------------

/** GeToDo main window.
 */
class MainWindow : public Gtk::Window
{
public:
	MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~MainWindow();

	void setTaskManager(getodo::TaskManager* manager);
protected:
	Glib::RefPtr<Gnome::Glade::Xml> refXml;
	
	// ----- child widgets ----

	// task treeview
	Gtk::TreeView* pTaskTreeView;
	//Glib::RefPtr<getodo::TaskTreeModel> refTaskTreeModel;
	Glib::RefPtr<getodo::TaskTreeStore> refTaskTreeModel;
	Glib::RefPtr<Gtk::TreeModelSort> refTaskTreeModelSort;
	Glib::RefPtr<Gtk::TreeModelFilter> refTaskTreeModelFilter;

	// task editing panel
	// TODO:
	// * change some widget's type from the dummy Entry
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

	// filtering panel
	Gtk::TreeView* pTagTreeView;
	Gtk::TreeView* pFilterTreeView;
	Glib::RefPtr<getodo::TagTreeModel> refTagTreeModel;
	//Glib::RefPtr<Gtk::TreeModelSort> refTagTreeModelSort;


	Glib::RefPtr<Gtk::ListStore> refFilterTreeModel;
	struct FilterModelColumns : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<int> id;
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> rule;

		FilterModelColumns() {
			add(id), add(name), add(rule);
		}
	};
	const FilterModelColumns filterColumns;

	// menubar

	// toolbar

	Gtk::ToggleToolButton* pTaskFilterToggletoolbutton;

	// ----- signal handlers -----
	void on_buttonTaskNewToplevel_clicked();
	void on_buttonTaskNew_clicked();
	void on_buttonTaskNewSubtask_clicked();
	void on_buttonTaskDelete_clicked();
	void on_buttonTaskUpdate_clicked();
	void on_buttonTaskFilter_toggled();
	void on_buttonRecurrence_clicked();

	void on_taskTreeview_selection_changed();
	bool on_taskTreeview_filter_row_visible(const Gtk::TreeModel::const_iterator& iter);

	void on_filterTreeview_selection_changed();

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
private:
	getodo::TaskManager* taskManager;
	//Gtk::TreeModel::iterator getCursorIter(Gtk::TreeView* pTreeView,
	//	Glib::RefPtr<Gtk::TreeModel> refTreeModel);

	bool filteringActive;

	// true, if signal handler should respond to event from this panel
	// false, if there is some batch filling or cleaning of the panel
	bool editingPanelActive;

	void fillEditingPanel(getodo::Task& task);
	void clearEditingPanel();
	void saveEditingPanelToTask(getodo::Task& task);
	getodo::id_t getCurrentlyEditedTaskId();
	bool updateTaskPartial(boost::function<void(getodo::TaskPersistence&)> f);

	void setTaskFilterRule(getodo::FilterRule& filter); // set the rule and enable filtering
	void resetTaskFilterRule(); // unset the rule and disable filtering
	void setFilterRuleFromSelection(); // true if a rule set, false if reset

	//void toggleFiltering(bool on); // enable filtering on true
};

#endif // GETODOGUI_MAIN_WINDOW_H