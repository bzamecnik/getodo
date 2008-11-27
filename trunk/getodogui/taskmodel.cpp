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

void TaskNode::remove_child(TaskNode& child) {
	TaskNodeVector::iterator it = std::find(children.begin(), children.end(), &child);
	if (it != children.end()) {
		children.erase(it);
	}
}

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

std::ostream& operator<<(std::ostream& os, TaskModel::Path& path) {
	for (TaskModel::Path::iterator it = path.begin(); it != path.end(); ++it) {
		os << (*it);
		if ((it+1) != path.end()) {
			os << ':';
		}
	}
	return os;
}

TaskNodeVector& TaskModel::get_tasks(void) const {
	return const_cast<TaskNodeVector&>(topLevelNodes);
}

int TaskModel::get_n_tasks() const {
	return (int)topLevelNodes.size();
}

/** Get next node.
 * Next node on the same level as \a node.
 */
TaskNode& TaskModel::get_next_node(TaskNode& node) const {
	// Used by: iter_next_vfunc()
	TaskNode* parent = node.get_parent();
	const TaskNodeVector& children(parent != 0 ? parent->get_children() : topLevelNodes);
	TaskNodeVector::const_iterator it = std::find(children.begin(), children.end(), &node);
	if (it == children.end() || (it+1) == children.end())
        throw InvalidNode();
    return **(++it);
}

/** Get node by path.
 * Traverse the tree of nodes. At each level move to a position
 * for that level which is specified in the path.
 */
TaskNode& TaskModel::get_node(Path& path) const {
	const TaskNodeVector* nodes = &topLevelNodes;
	TaskNode* node = 0;
	
	for (Path::const_iterator it = path.begin(); it != path.end(); ++it) {
		if ((nodes == 0) || ((size_t)(*it + 1) > nodes->size())) {
			throw InvalidPath();
		}
		TaskNodeVector::const_iterator nodesIt = nodes->begin();
		// skip as many nodes as current path element says
		std::advance(nodesIt, *it);
		if ((nodesIt == nodes->end()) || (*nodesIt == 0)) {
			throw InvalidPath();
		}
		node = *nodesIt;
		nodes = &(node->get_children());
	}
	if (!node) {
		throw InvalidPath();
	}
	return *node;
}

/** Get path by node.
 */
TaskModel::Path TaskModel::get_path(TaskNode& node) const {
	Path path;
	TaskNode* currentNode = &node;
	TaskNode* parent = node.get_parent();

	// traverse upwards in the tree
	while (parent != 0) {
		// find the ordinal number of current node on current level
		TaskNodeVector& nodes(parent->get_children());
		TaskNodeVector::iterator it = std::find(nodes.begin(),
			nodes.end(), currentNode);
		if (it == nodes.end()) {
			throw InvalidNode();
		}
		path.push_front((int)std::distance(nodes.begin(), it));
		currentNode = parent;
		parent = currentNode->get_parent();
	}
	// find the ordinal number of current node on top level
	TaskNodeVector::const_iterator it = std::find(topLevelNodes.begin(),
		topLevelNodes.end(), currentNode);
	if (it == topLevelNodes.end()) {
		throw InvalidNode();
	}
	path.push_front((int)std::distance(topLevelNodes.begin(), it));
	return path;
}

void TaskModel::refresh(void) {
	clear();
	std::list<Task*> taskList = manager.getTasksList();
	// TODO: when sorting will work correctly, use normal iterator here
	// This is only a hack to sort the list in ascending order (by Id).
	for (std::list<Task*>::reverse_iterator it = taskList.rbegin(); it != taskList.rend(); ++it) {
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
		node = 0;
	}
	taskNodeMap.clear();
	topLevelNodes.clear();
}

void TaskModel::remove(Task& task) {
	//std::cerr << "TaskModel::remove(" << task.getTaskId() << ")" << std::endl; //DEBUG
	//std::cerr << "before remove:" << std::endl;
	//printNodeTree(); //DEBUG

	TaskNodeMap::iterator mapIt = taskNodeMap.find(task.getTaskId());
	if (mapIt == taskNodeMap.end()) { return; }
	
	TaskNode* node = mapIt->second;
	if (node == 0) { return; }

	Path pathToDeletedNode = get_path(*node);	
	TaskNode* parent = node->get_parent();
	TaskNodeVector& children = node->get_children();
	
	if (parent != 0) {
		// removing an inner node
		parent->remove_child(*node);
		if (!parent->has_children()) {
			// last child removed
			signal_node_has_child_toggled(*parent, get_path(*parent));
		}
	} else {
		// removing one of top level nodes
		TaskNodeVector::iterator taskNodeIt = std::find(
			topLevelNodes.begin(), topLevelNodes.end(), node);
		if (taskNodeIt != topLevelNodes.end()) {	
			topLevelNodes.erase(taskNodeIt);
		}
	}

	signal_node_removed(*node, pathToDeletedNode);

	// remove the node from id->node map
	taskNodeMap.erase(mapIt);

	//std::cerr << "after remove:" << std::endl;
	//printNodeTree(); //DEBUG

	delete node;
	node = 0;
}

void TaskModel::insert(Task& task) {
	// If the task has a parent, find it and insert the new task
	// as parent's child. Otherwise add it to topLevelNodes.
	TaskNode* newNode = new TaskNode(task);
	TaskNodeMap::iterator parentIt = taskNodeMap.find(task.getParentId());
	TaskNodeVector* parentChildren = 0;
	Path newPath;
	std::pair<TaskNodeMap::iterator,bool> result = taskNodeMap.insert(std::make_pair(task.getTaskId(), newNode));
	if (!result.second) {
		return;
	}
	if ((parentIt != taskNodeMap.end()) && (parentIt->second != 0)) {
		TaskNode* parentNode = parentIt->second;
		newNode->set_parent(parentNode);
		TaskNodeVector& parentChildren = parentNode->get_children();
		parentChildren.push_back(newNode);
		if (parentChildren.size() == 1) {
			// first child added
			signal_node_has_child_toggled(*parentNode, get_path(*parentNode));
		}
		newPath = get_path(*parentChildren.back());
	} else {
		topLevelNodes.push_back(newNode);
		newPath.push_back((int)std::distance(topLevelNodes.begin(),
		std::find(topLevelNodes.begin(), topLevelNodes.end(), newNode)));
	}
	//std::cerr << "TaskModel::insert: " << newNode->get_item().getTaskId() << " at " << newPath << std::endl;
	signal_node_inserted(*newNode, newPath);
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
	model.signal_node_has_child_toggled.connect(sigc::mem_fun(*this, &TaskTreeModel::on_treemodel_has_child_toggled));
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
	return TreeModelFlags();// & TREE_MODEL_LIST_ONLY;
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
	// DEBUG
	//std::cerr << node;
	//std::cerr << "(" << (node != 0 ? node->get_item().getTaskId() : -1) << "): " << std::endl;

	GtkTreeIter* gtk_iter_next = iter_next.gobj();
	clearIter(gtk_iter_next);

    try {
        gtk_iter_next->user_data = &(model.get_next_node(*node));
    } catch(TaskModel::InvalidNode&) {
		iter_next = iterator();
		//std::cerr << std::endl;
        return false;
    }
	// DEBUG
	//std::cerr << gtk_iter_next->user_data << std::endl;
	iter_next.set_stamp(stamp);
	return true;
}

bool TaskTreeModel::iter_children_vfunc(const iterator& parent, iterator& iter) const {
	// the first child of root
	GtkTreeIter* gtk_iter = iter.gobj();
	clearIter(gtk_iter);
	if (!parent->gobj()) {
		iter.set_stamp(stamp);
        gtk_iter->user_data = model.get_tasks().front();
        return true;
    }
	const GtkTreeIter* gtk_parent_iter = parent.gobj();

	// invalid parent
    if (!gtk_parent_iter->user_data) {
        return false;
    }

    // children of ordinary node (non-root)
    TaskNode* node = static_cast<TaskNode*>(gtk_parent_iter->user_data);
    TaskNodeVector& children(node->get_children());

	// no children
    if (children.empty()) {
        return false;
    }

	// first child
	iter.set_stamp(stamp);
    gtk_iter->user_data = children.front();
    return true;
}

bool TaskTreeModel::iter_has_child_vfunc(const iterator& iter) const {
	TaskNode* node = static_cast<TaskNode*>(iter.gobj()->user_data);
    return node && !node->get_children().empty();
}

int TaskTreeModel::iter_n_children_vfunc(const iterator& iter) const {
	if (!iter_is_valid(iter)) {
		return model.get_n_tasks();
	}
	TaskNode* node = static_cast<TaskNode*>(iter.gobj()->user_data);
	return (node ? (int)node->get_children().size() : model.get_n_tasks());
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
    if ((int)nodes.size() < n) {
		return false;
	}

    TaskNodeVector::iterator it = nodes.begin();
    std::advance(it, n);

    iter.set_stamp(stamp);
    gtk_iter->user_data = *it;
    return true;
}

/** 
 * Sets @a iter to be the parent of @a child. If @a child is at the toplevel, and
 * doesn't have a parent, then @a iter is set to an invalid iterator and false
 * is returned.
 *
 * @param child An iterator.
 * @param iter An iterator that will be set to refer to the parent node, or will be set as invalid.
 * @result true if the operation was possible.
 */
bool TaskTreeModel::iter_parent_vfunc(const iterator& child, iterator& iter) const {
	if (!iter_is_valid(child)) {
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
    TaskModel::Path modelPath(model.get_path(*node));
    TreeModel::Path treePath;
	//treePath.assign(modelPath.begin(), modelPath.end());
	convert_path_taskmodel_treemodel(modelPath, treePath);
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
	case 5: // done
		value.init(G_TYPE_BOOLEAN);
		g_value_set_boolean(value.gobj(), node->get_item().isDone());
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
	iter->stamp = 0;
	iter->user_data = iter->user_data2 = iter->user_data3 = 0;
}

void TaskTreeModel::convert_path_taskmodel_treemodel(const TaskModel::Path& mpath, TreeModel::Path& tpath) const {
	TaskModel::Path::const_iterator it;
	for (it = mpath.begin(); it != mpath.end(); ++it) {
        tpath.push_back(*it);
    }
	//mpath.assign(tpath.begin(), tpath.end());
}

void TaskTreeModel::convert_path_treemodel_taskmodel(const TreeModel::Path& tpath, TaskModel::Path& mpath) const {
	TreeModel::Path::const_iterator it;
	for (it = tpath.begin(); it != tpath.end(); ++it) {
        mpath.push_back(*it);
    }
	// can't use assign()
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
	row_inserted(treePath, TreeModel::iterator(gobj(), &iter));
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
	++stamp;
	TreeModel::Path treePath;
	convert_path_taskmodel_treemodel(path, treePath);
	row_deleted(treePath);
}

void TaskTreeModel::on_treemodel_has_child_toggled(TaskNode& node, TaskModel::Path& path) {
	TreeModel::Path treePath;
	convert_path_taskmodel_treemodel(path, treePath);
	GtkTreeIter iter;
	clearIter(&iter);
    //++stamp;
	iter.stamp = stamp;
    iter.user_data = &node;
	row_has_child_toggled(treePath, TreeModel::iterator(gobj(), &iter));
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

//// for reference (TODO: delete if not needed anymore)
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

} // namespace getodo
