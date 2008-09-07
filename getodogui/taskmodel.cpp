// $Id$

#include "stdafx.h"
#include "taskmodel.h"

namespace getodo {

// ----- class TaskNode --------------------

TaskNode::~TaskNode() {
	for (TaskNodeVector::iterator it = children.begin();
		it != children.end(); ++it)
	{
		delete *it;
	}
}

//// ----- class TaskManager --------------------
//
//TaskManager::TaskManager() {}
//TaskManager::~TaskManager() {
//	for(tasks_t::iterator it = tasks.begin(); it != tasks.end(); ++it) {
//		if(it->second) {
//			delete it->second;
//			it->second = 0;
//		}
//	}
//}
//void TaskManager::insertTask(Task* task) {
//	using namespace std;
//	if (task) {
//		pair<tasks_t::iterator, bool> result = tasks.insert(make_pair(task->id, task));
//		if (result.second) {
//			signal_task_inserted(*task);
//		}
//	}
//}
//bool TaskManager::hasTask(id_t taskId) {
//	return tasks.find(taskId) != tasks.end();
//}
//Task* TaskManager::getTask(id_t taskId) {
//	tasks_t::iterator foundTask = tasks.find(taskId);
//	if (foundTask != tasks.end()) {
//		return foundTask->second;
//	} else {
//		return 0;
//	}
//}
//void TaskManager::updateTask(Task* task) {
//	if (!task) { return; }
//	tasks_t::iterator updatedTask = tasks.find(task->id);
//	if (updatedTask != tasks.end()) {
//		tasks[task->id]->name = task->name;
//		signal_task_updated(*task);
//	} else {
//		insertTask(task);
//	}
//	
//}
//void TaskManager::removeTask(id_t taskId) {
//	tasks_t::iterator taskToRemove = tasks.find(taskId);
//	if(taskToRemove != tasks.end()) {
//		signal_task_removed(*(taskToRemove->second));
//		delete taskToRemove->second;
//		tasks.erase(taskToRemove);
//	}
//}
//
//std::list<Task*> TaskManager::getTasksList() {
//	std::list<Task*> taskList;
//	for (tasks_t::iterator it = tasks.begin(); it != tasks.end(); ++it) {
//		taskList.push_back(it->second);
//	}
//	return taskList;
//}

// ----- class TaskModel --------------------

TaskModel::TaskModel(TaskManager& _manager)
: manager(_manager)
{
	manager.signal_task_inserted.connect(sigc::mem_fun(*this, &TaskModel::on_task_inserted));
	manager.signal_task_updated.connect(sigc::mem_fun(*this, &TaskModel::on_task_updated));
	manager.signal_task_removed.connect(sigc::mem_fun(*this, &TaskModel::on_task_removed));

	refresh();
}

TaskModel::~TaskModel() {
	for (TaskNodeMap::iterator it = taskNodeMap.begin(); it != taskNodeMap.end(); ++it) {
		delete it->second;
		it->second = 0;
	}
}

TaskNodeVector& TaskModel::get_tasks(void) const {
	return const_cast<TaskNodeVector&>(taskNodes);
}

TaskNode& TaskModel::get_next_node(TaskNode& node) const {
	TaskNodeVector::const_iterator it = std::find(taskNodes.begin(), taskNodes.end(), &node);
	if (it == taskNodes.end() || (it+1) == taskNodes.end())
        throw InvalidNode();
    return **(++it);
}

TaskNode& TaskModel::get_node(Path& path) const {
	// this is for lists only
	// TODO: remake for trees
	if (path.size() < 1) {
		throw InvalidPath();
	}
	
	TaskNodeVector::const_iterator it = taskNodes.begin();
	std::advance(it, path[0]);
	if (it == taskNodes.end()) {
		throw InvalidPath();
	}
	TaskNode* node = *it;
	if (!node) {
		throw InvalidPath();
	}
	return *node;
}

TaskModel::Path TaskModel::get_path(TaskNode& node) const {
	// TODO: remake for trees
	Path path(1);
	path[0] = (int)std::distance(taskNodes.begin(),
		std::find(taskNodes.begin(), taskNodes.end(), &node));
	//if (path[0] > 0) {
	//	--path[0];
	//}
	return path;
}

void TaskModel::refresh(void) {
	clear();
	std::list<Task*> taskList = manager.getTasksList();
	for (std::list<Task*>::iterator it = taskList.begin(); it != taskList.end(); ++it) {
		insert(**it);
	}
}

void TaskModel::clear(void) {
	for (TaskNodeMap::iterator it = taskNodeMap.begin();
		it != taskNodeMap.end(); ++it)
	{
		TaskNode* node = it->second;
		signal_node_removed(*node, get_path(*node));
		delete node;
	}
	taskNodeMap.clear();
	taskNodes.clear();
}

void TaskModel::remove(Task& task) {
	TaskNodeMap::iterator mapIt = taskNodeMap.find(task.getTaskId());
	if (mapIt == taskNodeMap.end()) { return; }
	
	TaskNode* node = mapIt->second;
	TaskNodeVector::iterator taskNodeIt = std::find(taskNodes.begin(), taskNodes.end(), node);
	
	signal_node_removed(*node, get_path(*node));
	taskNodes.erase(taskNodeIt);
	taskNodeMap.erase(mapIt);
}

void TaskModel::insert(Task& task) {
	TaskNode* newNode = new TaskNode(task);
	taskNodes.push_back(newNode);
	std::pair<TaskNodeMap::iterator,bool> result = taskNodeMap.insert(std::make_pair(task.getTaskId(), newNode));
	if (result.second) {
		Path newPath(1);
		newPath[0] = (int)std::distance(taskNodes.begin(),
			std::find(taskNodes.begin(), taskNodes.end(), newNode));
		signal_node_inserted(*newNode, newPath);
	}
}

void TaskModel::on_task_inserted(Task& task) {
	// called by underlying TaskManager
	insert(task);
}

void TaskModel::on_task_updated(Task& task) {
	// called by underlying TaskManager
	TaskNodeMap::iterator mapIt = taskNodeMap.find(task.getTaskId());
	if (mapIt != taskNodeMap.end()) {
		TaskNode* node = mapIt->second;
		Path path = get_path(*node);
		// Don't delete the node!
		// * Just try to replace its content while having the
		// children or parent pointer untouched.

		// BUGFIX: Commenting the following code seem
		// to fix a bug, when the program hung with an old
		// iterator pointing to deleted node.
		// Is this ok?

		//if (node) {
		//	delete node;
		//	node = 0;
		//}
		//mapIt->second = node = new TaskNode(task);
		signal_node_updated(*node, path);
	} else {
		insert(task);
	}
}

void TaskModel::on_task_removed(Task& task) {
	// called by underlying TaskManager
	remove(task);
}

// ----- class TaskTreeModel --------------------

TaskTreeModel_Class TaskTreeModel::tasktreemodel_class_;

TaskTreeModel::TaskTreeModel(TaskManager& manager)
: Glib::ObjectBase("GeToDo_TaskTreeModel"), 
  Glib::Object(Glib::ConstructParams(tasktreemodel_class_.init(), (char*) 0)),
  TreeModel(),
  model(manager), stamp(1)
{
	model.signal_node_inserted.connect(sigc::mem_fun(*this, &TaskTreeModel::on_treemodel_inserted));
	model.signal_node_updated.connect(sigc::mem_fun(*this, &TaskTreeModel::on_treemodel_updated));
	model.signal_node_removed.connect(sigc::mem_fun(*this, &TaskTreeModel::on_treemodel_removed));
}

TaskTreeModel::~TaskTreeModel() {
}

Glib::RefPtr<TaskTreeModel> TaskTreeModel::create(TaskManager& manager)
{
	return Glib::RefPtr<TaskTreeModel>(new TaskTreeModel(manager));
}

void TaskTreeModel::set_value_impl(const iterator& row, int column, const Glib::ValueBase& value) {
	// TODO (for editing from TreeView)
}

Gtk::TreeModelFlags TaskTreeModel::get_flags_vfunc() const {
	using namespace Gtk;
	return TreeModelFlags() & TREE_MODEL_LIST_ONLY;
}

int TaskTreeModel::get_n_columns_vfunc() const {
	return (int)columns.types.size();
}

GType TaskTreeModel::get_column_type_vfunc(int index) const {
	using namespace Gtk;
	try {
		return columns.types.at(index);
	} catch(std::range_error) {
		return GTK_TYPE_OBJECT;
	}
}

bool TaskTreeModel::iter_next_vfunc(const iterator& iter, iterator& iter_next) const {
	if (!iter_is_valid(iter)) {
		iter_next = iterator();
		return false;
	}
	const GtkTreeIter* gtk_iter = iter.gobj();
	if (!gtk_iter || !gtk_iter->user_data) { return false; }

    TaskNode* node = static_cast<TaskNode*>(gtk_iter->user_data);
	GtkTreeIter* gtk_iter_next = iter_next.gobj();
	clearIter(gtk_iter_next);

    try {
        gtk_iter_next->user_data = &(model.get_next_node(*node));
    } catch(TaskModel::InvalidNode&) {
		iter_next = iterator();
        return false;
    }
	iter_next.set_stamp(stamp);
	return true;
}

bool TaskTreeModel::iter_children_vfunc(const iterator& parent, iterator& iter) const {
	// the first child of root
	GtkTreeIter* gtk_iter = iter.gobj();
	if (!parent->gobj()) {
		iter.set_stamp(stamp);
		clearIter(gtk_iter);
        gtk_iter->user_data = model.get_tasks().front();
        return true;
    }

	const GtkTreeIter* gtk_parent_iter = parent.gobj();
	// invalid parent
    if (!gtk_parent_iter->user_data) {
        clearIter(gtk_iter);
        return false;
    }

    // children of ordinary node (non-root)
    TaskNode* node = static_cast<TaskNode*>(gtk_parent_iter->user_data);
    TaskNodeVector& children(node->get_children());

	// no children
    if (children.empty()) {
        clearIter(gtk_iter);
        return false;
    }

	// first child
    iter.set_stamp(stamp);
	clearIter(gtk_iter);
    gtk_iter->user_data = children.front();
    return true;
}

bool TaskTreeModel::iter_has_child_vfunc(const iterator& iter) const {
	TaskNode* node = static_cast<TaskNode*>(iter.gobj()->user_data);
    return node && !node->get_children().empty();
}

int TaskTreeModel::iter_n_children_vfunc(const iterator& iter) const {
	if (!iter_is_valid(iter)) {
		return 0;
	}
	TaskNode* node = static_cast<TaskNode*>(iter.gobj()->user_data);
	return (node ? (int)node->get_children().size() : 0);
}

bool TaskTreeModel::iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const {
	if (!iter_is_valid(parent)) {
		iter = iterator();
		return false;
	}
	
	// TODO: fix the case when "static_cast<TaskNode*>(parent->user_data)" is 0
	TaskNodeVector& nodes( (!parent.gobj()) ? model.get_tasks() :
        static_cast<TaskNode*>(parent.gobj()->user_data)->get_children());

	GtkTreeIter* gtk_iter = iter.gobj();
	clearIter(gtk_iter);
    if ((int)nodes.size() < n) { return false; }

    TaskNodeVector::iterator it = nodes.begin();
    std::advance(it, n);

    iter.set_stamp(stamp);
    gtk_iter->user_data = *it;
    return true;
}

bool TaskTreeModel::iter_parent_vfunc(const iterator& child, iterator& iter) const {
	if (!iter_is_valid(iter)) {
		iter = iterator();
		return false;
	}

	GtkTreeIter* gtk_iter = iter.gobj();
	const GtkTreeIter* gtk_child_iter = child.gobj();
	clearIter(gtk_iter);
	if (gtk_child_iter && gtk_child_iter->user_data) {
        TaskNode* node = static_cast<TaskNode*>(gtk_child_iter->user_data);
        TaskNode* parent = node->get_parent();
        
        if (parent) {
            iter.set_stamp(stamp);
            gtk_iter->user_data = parent;
            return true;
        }
    }
    return false;
}

void TaskTreeModel::ref_node_vfunc(const iterator& iter) const {}

void TaskTreeModel::unref_node_vfunc(const iterator& iter) const {}

Gtk::TreeModel::Path TaskTreeModel::get_path_vfunc(const iterator& iter) const {
	const GtkTreeIter* it = iter.get_gobject_if_not_end();

    if (!it || !it->user_data) {
        return TreeModel::Path();
    }

	// convert TaskModel iterator -> TaskModel path -> TreeModel path
    TaskNode* node = static_cast<TaskNode*>(it->user_data);
    TaskModel::Path path(model.get_path(*node));

    TreeModel::Path treePath;
    convert_path_taskmodel_treemodel(path, treePath);
    return treePath;
}

bool TaskTreeModel::get_iter_vfunc(const TreeModel::Path& path, iterator& iter) const {
	if (path.size() < 1) {
		iter = iterator();
		return false;
	}

	TaskModel::Path modelPath;
    // convert TreeModel path -> TaskModel path
	modelPath.assign(path.begin(), path.end());
    
	GtkTreeIter* gtk_iter = iter.gobj();
	clearIter(gtk_iter);
	
    try {
        gtk_iter->user_data = &(model.get_node(modelPath));
    } catch(TaskModel::InvalidPath&) {
		gtk_iter->user_data = 0;
        return false;
    }
	iter.set_stamp(stamp);
	return true;
}

void TaskTreeModel::get_value_vfunc(const iterator& iter, int column, Glib::ValueBase& value) const {
	if (!iter_is_valid(iter)) { return; }
	const GtkTreeIter* it = iter.get_gobject_if_not_end();
	if (!it || !it->user_data) { return; }

	//check iter valitidy
    TaskNode* node = static_cast<TaskNode*>(it->user_data);

    switch (column) {
    case 0: // id
        value.init(G_TYPE_INT);
		g_value_set_int(value.gobj(), node->get_item().getTaskId());
        break;
    case 1: // description
        value.init(G_TYPE_STRING);
        g_value_set_string(value.gobj(), node->get_item().getDescription().c_str());
        break;
	case 2: // dateDeadline
		value.init(G_TYPE_STRING);
		g_value_set_string(value.gobj(), node->get_item().getDateDeadline().toString().c_str());
		break;
	case 3: // priority
		value.init(G_TYPE_INT);
		g_value_set_int(value.gobj(), node->get_item().getPriority());
		break;
	case 4: // completedPercentage
		value.init(G_TYPE_INT);
		g_value_set_int(value.gobj(), node->get_item().getCompletedPercentage());
		break;
	}
}

bool TaskTreeModel::iter_is_valid(const iterator& iter) const {
	return ((iter.get_stamp() == stamp) ? Gtk::TreeModel::iter_is_valid(iter) : false);
}

void TaskTreeModel::refresh() {
	model.refresh();
}

void TaskTreeModel::clearIter(GtkTreeIter* iter) const {
	if (iter) {
		iter->stamp = 0;
		iter->user_data = iter->user_data2 = iter->user_data3 = 0;
	}
}

void TaskTreeModel::convert_path_taskmodel_treemodel(const TaskModel::Path& mpath, TreeModel::Path& tpath) const {
	TaskModel::Path::const_iterator it;
	for (it = mpath.begin(); it != mpath.end(); ++it) {
        tpath.push_back(*it);
    }
}

void TaskTreeModel::convert_path_treemodel_taskmodel(const TreeModel::Path& tpath, TaskModel::Path& mpath) const {
	TreeModel::Path::const_iterator it;
	for (it = tpath.begin(); it != tpath.end(); ++it) {
        mpath.push_back(*it);
    }
}

void TaskTreeModel::on_treemodel_inserted(TaskNode& node, TaskModel::Path& path) {
	// TODO: avoid duplicating code
	TreeModel::Path treePath;
	convert_path_taskmodel_treemodel(path, treePath);

    GtkTreeIter iter;
	clearIter(&iter);
	++stamp;
    iter.stamp = stamp;
    iter.user_data = &node;
	TreeModel::iterator treeIter(gobj(), &iter);
	row_inserted(treePath, treeIter);
}
void TaskTreeModel::on_treemodel_updated(TaskNode& node, TaskModel::Path& path) {
	TreeModel::Path treePath;
	convert_path_taskmodel_treemodel(path, treePath);
	//iterator treeIter = get_iter(treePath);
	//row_changed(treePath, treeIter);

	GtkTreeIter iter;
	clearIter(&iter);
    ++stamp;
	iter.stamp = stamp;
    iter.user_data = &node;
	row_changed(treePath, TreeModel::iterator(gobj(), &iter));
}
void TaskTreeModel::on_treemodel_removed(TaskNode& node, TaskModel::Path& path) {
	TreeModel::Path treePath;
	convert_path_taskmodel_treemodel(path, treePath);
	row_deleted(treePath);
}

// ----- class TaskTreeModel_Class --------------------

const Glib::Class& TaskTreeModel_Class::init()
{
    if (!gtype_)
    {
        class_init_func_ = &TaskTreeModel_Class::class_init_function;

        const GTypeInfo derived_info = 
        {
            sizeof(GObjectClass),
            0,
            0,
            class_init_func_,
            0,
            0,
            sizeof(GObject),
            0,
            0,
            0,
        };
        
        gtype_ = g_type_register_static(G_TYPE_OBJECT, "GeToDo_TaskTreeModel",
            &derived_info, GTypeFlags(0));

		Gtk::TreeModel::add_interface(get_type());
    }

    return *this;
}

void TaskTreeModel_Class::class_init_function(void* g_class, void* class_data) {}

} // namespace getodo
