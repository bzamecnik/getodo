// $Id$

#include "stdafx.h"
#include "taskmodel.h"

namespace getodo {

// ----- class TaskTreeStore ---------------

TaskTreeStore::TaskTreeStore(TaskManager& _manager) : manager(_manager) {
	set_column_types(columns);
}

Glib::RefPtr<TaskTreeStore> TaskTreeStore::create(TaskManager& _manager) {
	TaskTreeStore* store = new TaskTreeStore(_manager);
	_manager.signal_task_inserted.connect(sigc::mem_fun(*store, &TaskTreeStore::on_task_inserted));
	_manager.signal_task_updated.connect(sigc::mem_fun(*store, &TaskTreeStore::on_task_updated));
	_manager.signal_task_removed.connect(sigc::mem_fun(*store, &TaskTreeStore::on_task_removed));
	_manager.signal_task_moved.connect(sigc::mem_fun(*store, &TaskTreeStore::on_task_moved));
	store->refresh();
	return Glib::RefPtr<TaskTreeStore>(store);
}

void TaskTreeStore::on_task_inserted(Task& task) {
	insertTask(task);
}

void TaskTreeStore::on_task_updated(Task& task) {
	Gtk::TreeModel::iterator iter = getIterByTask(task);
	if (!iter) { return; }
	setRowFromTask(iter, task);
	row_changed(get_path(iter), iter);
}

void TaskTreeStore::on_task_removed(Task& task) {
	eraseTask(task);
}

void TaskTreeStore::on_task_moved(Task& task, id_t oldParentId) {
	Gtk::TreeModel::iterator taskIter;
	Task* oldParentTask = manager.getTask(oldParentId);
	// Find out in which task's children the moved task was
	Gtk::TreeNodeChildren childrenIter = children();
	if (oldParentTask) {
		Gtk::TreeModel::iterator oldParentIter = getIterByTask(*oldParentTask);
		if (oldParentIter) {
			childrenIter = oldParentIter->children();
		}
	}
	// Find the task in the TreeStore (which is still under
	// the old parent or root children()
	id_t taskId = task.getTaskId();
	for (Gtk::TreeModel::iterator iter = childrenIter.begin();
		iter != childrenIter.end(); ++iter)
	{
		if ((*iter)[columns.id] == taskId) {
			taskIter = iter;
			break;
		}
	}
	
	if (!taskIter) { return; } // no such a task in the TreeStore
	Gtk::TreeModel::iterator newTaskIter;
	if (task.hasParent()) {
		Task* parentTask = manager.getTask(task.getParentId());
		if (!parentTask) { return; }
		Gtk::TreeModel::iterator newParentIter = getIterByTask(*parentTask);
		newTaskIter = append(newParentIter->children());
	} else {
		newTaskIter = append();
	}
	// copy task and the whole subtree as a child of parentTask
	copyRow(taskIter, newTaskIter);
	
	// TODO: copy the the subtree

	// erase task
	erase(taskIter);
}

void TaskTreeStore::insertTask(Task& task) {
	Task* parent = manager.getTask(task.getParentId());
	Gtk::TreeModel::iterator iter;
	if (task.hasParent() && parent) {
		Gtk::TreeModel::iterator parentIter = getIterByTask(*parent);
		iter = append(parentIter->children());
	} else {
		iter = append();
	}
	setRowFromTask(iter, task);
}

void TaskTreeStore::eraseTask(Task& task) {
	erase(getIterByTask(task));
}

Gtk::TreeModel::iterator TaskTreeStore::getIterByTask(Task& task) {
	// traverse from this task by parents up to its root
	// -> make a path containing task ids

	// NOTE: getPathByTask() uses information about real tree structure
	// in database. So it traverses down by the path from root to the task
	// to be found. If the task is in the TreeStore, but somewhere else than
	// on this path, this function will not find it.

	std::deque<int> taskIdPath;
	Task* currentTask = &task;
	id_t currentTaskId = task.getTaskId();
	do {
		taskIdPath.push_front(currentTaskId);
		currentTaskId = currentTask->getParentId();
		currentTask = manager.getTask(currentTaskId);
	} while (currentTask != 0);

	// traverse down the treestore searching for proper ids
	Gtk::TreeNodeChildren childrenIter = children();
	Gtk::TreeModel::iterator iter;
	for (std::deque<int>::size_type depth = 0; depth < taskIdPath.size(); depth++) {
		// problem: this could be slow
		for (iter = childrenIter.begin(); iter != childrenIter.end(); ++iter) {
			if ((*iter)[columns.id] == taskIdPath[depth]) {
				childrenIter = iter->children();
				break;
			}
		}
	}
	if ((*iter)[columns.id] != task.getTaskId()) {
		iter = children().end();
	}
	return iter;
}

void TaskTreeStore::setRowFromTask(Gtk::TreeModel::iterator& iter, Task& task) {
	// TODO: check if iter is ok
	Gtk::TreeModel::Row row = *iter;
	row[columns.id] = task.getTaskId();
	row[columns.description] = task.getDescription();
	row[columns.dateDeadline] = task.getDateDeadline().toString();
	row[columns.priority] = task.getPriority();
	row[columns.completedPercentage] = task.getCompletedPercentage();
	row[columns.done] = task.isDone();
}

void TaskTreeStore::copyRow(Gtk::TreeModel::iterator& src, Gtk::TreeModel::iterator& dest) {
	Gtk::TreeModel::Row srcRow = *src;
	Gtk::TreeModel::Row destRow = *dest;
	destRow[columns.id] = int(srcRow[columns.id]);
	destRow[columns.description] = Glib::ustring(srcRow[columns.description]);
	destRow[columns.dateDeadline] = Glib::ustring(srcRow[columns.dateDeadline]);
	destRow[columns.priority] = int(srcRow[columns.priority]);
	destRow[columns.completedPercentage] = int(srcRow[columns.completedPercentage]);
	destRow[columns.done] = bool(srcRow[columns.done]);
}

// insert all tasks from task manager to tree store
void TaskTreeStore::refresh() {
	clear();

	// Traverse the task tree using a kind of DFS and append
	// tasks to the tree store.

	// TODO: This operation could be more efficient.
	
	// A stack of node levels of task tree waiting for
	// appending to the TaskTreeStore
	// - vector<id_t> - substasks
	// - vector<id_t>::iterator - next task to process
	std::stack<
		std::pair<
			boost::shared_ptr<std::vector<id_t>>,
			std::vector<id_t>::iterator
		>> stack;

	std::vector<Task*>& topLevelTasks = manager.getTopLevelTasks();
	// figure out top level task id's only
	boost::shared_ptr<std::vector<id_t>> topLevelIds(
		new std::vector<id_t>(topLevelTasks.size()));
	std::transform(topLevelTasks.begin(), topLevelTasks.end(), topLevelIds->begin(),
		boost::bind(&Task::getTaskId, _1));
	stack.push(std::make_pair(topLevelIds, topLevelIds->begin()));
	while (!stack.empty()) {
		std::pair<
			boost::shared_ptr<std::vector<id_t>>,
			std::vector<id_t>::iterator
		>& pair = stack.top();
		while (pair.second != pair.first->end()) {
			Task* task = manager.getTask(*pair.second);
			if (!task) {
				continue; // odd situation
			}
			Gtk::TreeModel::iterator iter;
			if (task->hasParent()) {
				Task* parentTask = manager.getTask(task->getParentId());
				if (!parentTask) {
					iter = append(); // odd situation
				}
				iter = append(getIterByTask(*parentTask)->children());
			} else {
				iter = append();
			}
			setRowFromTask(iter, *task);
			pair.second++;
			boost::shared_ptr<std::vector<id_t>> subtasks(&task->getSubtaskIds());
			if (!subtasks->empty()) {
				stack.push(std::make_pair(subtasks, subtasks->begin()));
				goto continueloop; // break this loop and continue to the outer loop
			}
		}
		stack.pop();
		continueloop:
		;
	}

	// dummy implementation - insert top level task only
	//std::vector<Task*>& tasks = manager.getTasks();
	//std::vector<Task*>::iterator it;
	//for (it = tasks.begin(); it != tasks.end(); ++it) {
	//	if (!(*it)->hasParent()) {
	//		Gtk::TreeModel::iterator iter = append();
	//		setRowFromTask(iter, **it);
	//	}
	//}
}

} // namespace getodo
