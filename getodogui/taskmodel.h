// $Id$

#ifndef TASK_MODEL_H
#define TASK_MODEL_H

#include "getodo.h"
#include <sigc++/sigc++.h>
#include <algorithm>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <stack>

namespace getodo {

/** Custom %Task TreeStore.
 * 
 */
class TaskTreeStore : public Gtk::TreeStore {
public:
	static Glib::RefPtr<TaskTreeStore> create(TaskManager& manager);

	class ModelColumns : public Gtk::TreeModel::ColumnRecord {
	public:
		Gtk::TreeModelColumn<int> id;
		Gtk::TreeModelColumn<Glib::ustring> description;
		//Gtk::TreeModelColumn<Glib::ustring> dateCreated;
		//Gtk::TreeModelColumn<Glib::ustring> dateLastModified;
		//Gtk::TreeModelColumn<Glib::ustring> dateStarted;
		Gtk::TreeModelColumn<Glib::ustring> dateDeadline;
		//Gtk::TreeModelColumn<Glib::ustring> dateCompleted;
		//Gtk::TreeModelColumn<Glib::ustring> recurrence;
		Gtk::TreeModelColumn<int> priority;
		Gtk::TreeModelColumn<int> completedPercentage;
		Gtk::TreeModelColumn<bool> done;

		ModelColumns() {
			add(id);
			add(description);
			//add(dateCreated);
			//add(dateLastModified);
			//add(dateStarted);
			add(dateDeadline);
			//add(dateCompleted);
			//add(recurrence);
			add(priority);
			add(completedPercentage);
			add(done);
		}
	};

	ModelColumns columns;

	Gtk::TreeModel::iterator getIterByTask(Task& task);
	//Gtk::TreeModel::Path& getPathByTaskId(id_t taskId);

protected:
	TaskTreeStore(TaskManager& manager);

	// called by underlying TaskManager
	void on_task_inserted(Task& task);
	void on_task_updated(Task& task);
	void on_task_removed(Task& task);
	void on_task_moved(Task& task, id_t oldParentId);

	void setRowFromTask(Gtk::TreeModel::iterator& iter, Task& task);
	void TaskTreeStore::copyRow(Gtk::TreeModel::iterator& src, Gtk::TreeModel::iterator& dest);
	void insertTask(Task& task);
	void eraseTask(Task& task);
	void refresh();
private:
	TaskManager& manager;
};

} // namespace getodo

#endif // TASK_MODEL_H
