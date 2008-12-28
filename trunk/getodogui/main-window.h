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
	//Glib::RefPtr<Gtk::TreeModelSort> refTaskTreeModelSort;

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

	// ----- signal handlers -----
	void on_buttonTaskNew_clicked();
	void on_buttonTaskNewSubtask_clicked();
	void on_buttonTaskDelete_clicked();
	void on_buttonTaskUpdate_clicked();
	void on_buttonRecurrence_clicked();

	void on_taskTreeview_selection_changed();

	bool on_taskDescriptionEntry_focus_out_event(GdkEventFocus* event, Gtk::Entry* entry);
private:
	getodo::TaskManager* taskManager;
	//Gtk::TreeModel::iterator getCursorIter(Gtk::TreeView* pTreeView,
	//	Glib::RefPtr<Gtk::TreeModel> refTreeModel);

	void fillEditingPanel(getodo::Task& task);
	void clearEditingPanel();
	void saveEditingPanelToTask(getodo::Task& task);
	getodo::id_t getCurrentlyEditedTaskId();
	bool updateTaskPartial(boost::function<void(getodo::TaskPersistence&)> f);
};

#endif // GETODOGUI_MAIN_WINDOW_H
