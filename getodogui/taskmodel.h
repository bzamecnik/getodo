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

protected:
	TaskTreeStore(TaskManager& manager);

	// called by underlying TaskManager
	void on_task_inserted(Task& task);
	void on_task_updated(Task& task);
	void on_task_removed(Task& task);

	Gtk::TreeModel::Path& getPathByTask(Task& task);
	//Gtk::TreeModel::Path& getPathByTaskId(id_t taskId);
	void setRowFromTask(Gtk::TreeModel::iterator& iter, Task& task);
	void insertTask(Task& task);
	void refresh();
private:
	TaskManager& manager;
};


// ----- The following will be obsoleted by TaskTreeStore: -----


class TaskNode;
typedef std::vector<TaskNode*> TaskNodeVector; // TODO: use shared_ptr!

/** %Task node.
 * A wrapper for Task objects in TaskModel.
 */
class TaskNode
{
public:
    TaskNode(Task& _task) : children(0), parent(0), task(_task)
    { }
    virtual ~TaskNode();

    bool has_children(void) {
        return !children.empty();
    }
	TaskNodeVector& get_children(void) { return children; }
	void add_child(TaskNode& child) { children.push_back(&child); }
	void remove_child(TaskNode& child);
	TaskNode* get_parent(void) { return parent; }
	void set_parent(TaskNode* parent) { this->parent = parent; } // TODO: is it safe?
    Task& get_item(void) { return task; }
    const Task& get_item(void) const { return task; }

	//DEBUG
	std::ostream& print(std::ostream& os) {
		os << "TaskNode @ " << this << ": task (" << task.getTaskId() << ")";
		os << ", parent node @ " << parent << ", parent task: " << task.getParentId();
		os << ", children: [" << std::endl;
		for (TaskNodeVector::iterator it = children.begin();
			it != children.end(); ++it)
		{
			(*it)->print(os);
			os << ", " << std::endl;
		}
		os << "]" << std::endl;
		return os;
	}
protected:
    TaskNodeVector children;
    TaskNode* parent; // TODO: use shared_ptr!
private:
	Task& task;
};

/** %Task model.
 * TaskModel acts as a wrapper over TaskManager that holds its tree structure.
 * It proxies signals from TaskManager to TaskTreeModel.
 * Maybe merge TaskModel and TaskTreeModel.
 */
class TaskModel : public sigc::trackable {
public:
    typedef std::deque<int> Path;
	typedef std::map<id_t, TaskNode*> TaskNodeMap; // TODO: use shared_ptr!

    struct InvalidNode { }; // an exception
    struct InvalidPath { }; // an exception

	TaskModel(TaskManager& manager);
    virtual ~TaskModel();

	TaskNodeVector& get_tasks() const;
	int get_n_tasks() const;
    TaskNode& get_next_node(TaskNode& node) const;
    TaskNode& get_node(Path& path) const;
    Path get_path(TaskNode& node) const;

	void refresh(void);
	void clear(void);

	// signals for TaskTreeModel
    sigc::signal2<void, TaskNode&, Path&> signal_node_inserted;
    sigc::signal2<void, TaskNode&, Path&> signal_node_updated;
    sigc::signal2<void, TaskNode&, Path&> signal_node_removed;
	sigc::signal2<void, TaskNode&, Path&> signal_node_has_child_toggled;

	// DEBUG
	void printNodeTree() {
		std::cerr << "topLevelNodes:" << std::endl;
		TaskNodeVector::iterator it;
		for (it = topLevelNodes.begin(); it != topLevelNodes.end(); ++it) {
			(*it)->print(std::cerr);
			std::cerr << std::endl;
		}
	}
	void printNodeMap() {
		std::cerr << "taskNodeMap:" << std::endl;
		TaskNodeMap::iterator it;
		for (it = taskNodeMap.begin(); it != taskNodeMap.end(); ++it) {
			std::cerr << "  " << (it->first) << ": " << (it->second) << std::endl;
		}
	}
private:
	TaskManager& manager;
	TaskNodeVector topLevelNodes; // children of root
	TaskNodeMap taskNodeMap;

	void remove(Task& task);
	void insert(Task& task);

	// called by underlying TaskManager
	void on_task_inserted(Task& task);
	void on_task_updated(Task& task);
	void on_task_removed(Task& task);
};

/** %Task TreeModel.
 * A custom Gtk::TreeModel implementation for using TaskManager from TreeView.
 */
class TaskTreeModel : public Glib::Object, public Gtk::TreeModel {
public:
	struct ModelColumns : public Gtk::TreeModelColumnRecord {
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
		
		std::vector<GType> types;

		ModelColumns() {
			add(id); types.push_back(GTK_TYPE_INT);
			add(description); types.push_back(GTK_TYPE_STRING);
			//add(dateCreated); types.push_back(GTK_TYPE_STRING);
			//add(dateLastModified); types.push_back(GTK_TYPE_STRING);
			//add(dateStarted); types.push_back(GTK_TYPE_STRING);
			add(dateDeadline); types.push_back(GTK_TYPE_STRING);
			//add(dateCompleted); types.push_back(GTK_TYPE_STRING);
			//add(recurrence); types.push_back(GTK_TYPE_STRING);
			add(priority); types.push_back(GTK_TYPE_INT);
			add(completedPercentage); types.push_back(GTK_TYPE_INT);
			add(done); types.push_back(GTK_TYPE_BOOL);
			//add(done);
		}
	};
	const ModelColumns columns;

	~TaskTreeModel();

	static Glib::RefPtr<TaskTreeModel> create(TaskManager& manager);

	void clear();
protected:
	TaskTreeModel(TaskManager& manager);

    virtual void set_value_impl(const iterator& row, int column, const Glib::ValueBase& value);
    //virtual void get_value_impl(const iterator& row, int column, Glib::ValueBase& value) const;

	//  Gtk::TreeModel stuff
    virtual GType get_column_type_vfunc(int index) const;
	virtual Gtk::TreeModelFlags get_flags_vfunc() const;
    virtual bool iter_next_vfunc(const iterator& iter, iterator& iter_next) const;
    virtual bool iter_children_vfunc(const iterator& parent, iterator& itert) const;
    virtual bool iter_has_child_vfunc(const iterator& iter) const;
    virtual int iter_n_children_vfunc(const iterator& iter) const;
	virtual int get_n_columns_vfunc() const;
    virtual bool iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const;
    virtual bool iter_parent_vfunc(const iterator& child, iterator& iter) const;
    virtual void ref_node_vfunc(const iterator& iter) const;
    virtual void unref_node_vfunc(const iterator& iter) const;
    virtual TreeModel::Path get_path_vfunc(const iterator& iter) const;
    virtual bool get_iter_vfunc(const Path& path, iterator& iter) const;
    virtual void get_value_vfunc(const iterator& iter, int column, Glib::ValueBase& value) const;
	virtual bool iter_is_valid(const iterator& iter) const; // fix stamp validating
private:	
	TaskModel model;
	int stamp;
	
	// GTK+ class stuff
    friend class TaskTreeModel_Class;
    static TaskTreeModel_Class tasktreemodel_class_;

	void refresh();
    void clearIter(GtkTreeIter* iter) const;

	void convert_path_taskmodel_treemodel(const TaskModel::Path& mpath, TreeModel::Path& tpath) const;
	void convert_path_treemodel_taskmodel(const TreeModel::Path& tpath, TaskModel::Path& mpath) const;

	// called from underlying TaskModel
	void on_treemodel_inserted(TaskNode& node, TaskModel::Path& path);
    void on_treemodel_updated(TaskNode& node, TaskModel::Path& path);
    void on_treemodel_removed(TaskNode& node, TaskModel::Path& path);
	void on_treemodel_has_child_toggled(TaskNode& node, TaskModel::Path& path);
}; // class TaskTreeModel

/** TaskTreeModel class for GTK+.
 */
class TaskTreeModel_Class : public Glib::Class
{
public:
    struct TaskTreeModelClass
    {
        GObjectClass parent_class;
    };

    friend class TaskTreeModel;

    const Glib::Class& init();
    static void class_init_function(void* g_class, void* class_data);
}; // class TaskTreeModel_Class

} // namespace getodo

#endif // TASK_MODEL_H
