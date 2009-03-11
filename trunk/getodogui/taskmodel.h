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
 * In addition to TreeStore TaskTreeStore contains a reference to the 
 * TaskManager, it respond to signals from it and change itself according
 * to what is in the TaskManager. Tasks can be localized in the model using the
 * getIterByTask() function.
 * 
 * TaskTreeStore is constructed using create() factory method.
 */
class TaskTreeStore : public Gtk::TreeStore {
public:
	/** Create a new TaskTreeStore.
	 * A factory method. Its purpose is to wrap it with Glib::RefPtr.
	 * \return RefPtr to the TaskTreeStore
	 */
	static Glib::RefPtr<TaskTreeStore> create(TaskManager& manager);

	/** %TaskTreeStore model columns.
	 * In addition there is a "visible" column which is used for filtering
	 * purposes.
	 */
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

		Gtk::TreeModelColumn<bool> visible; // for filtering

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
			add(visible); // for filtering
		}
	};

	ModelColumns columns;

	/** Find task in the model.
	 * Search for the task in by its id the tree and return an iterator
	 * to row where it is.
	 * \param task task to find
	 * \return iterator to the task or children().end() if not found
	 */
	Gtk::TreeModel::iterator getIterByTask(Task& task);
	//Gtk::TreeModel::Path& getPathByTaskId(id_t taskId);

protected:
	TaskTreeStore(TaskManager& manager);

	/** Task insertion signal handler.
	 * Called by signal_task_inserted of the underlying TaskManager.
	 * \param task task being inserted
	 */
	void on_task_inserted(Task& task);
	/** Task update signal handler.
	 * Called by signal_task_updated of the underlying TaskManager.
	 * \param task task being updated
	 */
	void on_task_updated(Task& task);
	/** Task deletion signal handler.
	 * Called by signal_task_removed of the underlying TaskManager.
	 * \param task task being removed
	 */
	void on_task_removed(Task& task);
	/** Task movement signal handler.
	 * Called by signal_task_moved of the underlying TaskManager.
	 * \param task task being moved, it has its new parent set inside
	 * \param oldParentId id of previous parent task from where it was moved
	 */
	void on_task_moved(Task& task, id_t oldParentId);

	/** Set the model row according to task contents.
	 * \param iter iterator pointing to the row
	 * \param task task using which to fill the row
	 */
	void setRowFromTask(Gtk::TreeModel::iterator& iter, Task& task);
	/** Copy contents of one model row to another.
	 * \param src iterator of source row
	 * \param dest iterator of destination row
	 */
	void copyRow(Gtk::TreeModel::iterator& src, Gtk::TreeModel::iterator& dest);
	/** Insert task into the model.
	 * Insert the task into the model tree hanging it under its parent task
	 * if any.
	 *
	 * Called by the signal handler.
	 * \param task task to be inserted
	 */
	void insertTask(Task& task);
	/** Delete task from the model.
	 * Find the task in the model tree and delete it.
	 *
	 * Called by the signal handler.
	 * \param task task to be deleted
	 */
	void eraseTask(Task& task);
	/** Refresh the model.
	 * Clean the model and insert all the task from the TaskManager in a batch.
	 *
	 * Implementation note: it traverses the tree of tasks in the TaskManager
	 * using a kind of DFS and appends tasks to the model tree.
	 */
	void refresh();
private:
	TaskManager& manager;
};

} // namespace getodo

#endif // TASK_MODEL_H
