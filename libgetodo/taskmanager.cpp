// $Id$
//
// C++ Implementation: taskmanager
//
// Description:
//
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "taskmanager.h"

namespace getodo {

using namespace sqlite3x;

// ----- class TaskManager --------------------

// ----- Constructors & destructor -----

TaskManager::TaskManager(const std::string& dbname)
: conn(0) {
	try {
		conn = new sqlite3_connection(dbname);
	} catch(database_error&) {
		conn = 0;
		return;
	}
	tryLoadingFromDatabase();
}

TaskManager::TaskManager(sqlite3_connection* c)
: conn(c) {
	tryLoadingFromDatabase();
}

TaskManager::~TaskManager() {
	if(conn) {
		conn->close();
	}
	for (std::map<id_t,Task*>::iterator it = tasks.begin();
		it != tasks.end(); ++it) {
		delete it->second;
	}
	for (std::map<id_t,Tag*>::iterator it = tags.begin();
		it != tags.end(); ++it) {
		delete it->second;
	}
	for (std::map<id_t,FilterRule*>::iterator it = filters.begin();
		it != filters.end(); ++it) {
		delete it->second;
	}
	tasks.clear();
	tags.clear();
	filters.clear();
}


// ----- SQLite connection -----

sqlite3_connection* TaskManager::getConnection() {
	return conn;
}

// ----- Task operations -----

id_t TaskManager::addTask(const Task& task) {
	Task* taskCopy = new Task(task);
	taskCopy->setTaskId(Task::INVALID_ID); // delete id
	// save it to database and get the new taskId
	TaskPersistence tp(conn, taskCopy);
	tp.insert();
	// insert the task into TaskManager
	tasks[taskCopy->getTaskId()] = taskCopy;
	signal_task_inserted(*taskCopy);

	// TODO: add all tags from task

	return taskCopy->getTaskId();
}

bool TaskManager::hasTask(id_t taskId) {
	std::map<id_t,Task*>::iterator it = tasks.find(taskId);
	return (it != tasks.end());
}

Task* TaskManager::getTask(id_t taskId) {
	std::map<id_t,Task*>::iterator it = tasks.find(taskId);
	if (it != tasks.end()) {
		return it->second;
	} else {
		return 0; // better throw GetodoError
	}
}

TaskPersistence& TaskManager::getPersistentTask(id_t taskId) {
	return *(new TaskPersistence(conn, getTask(taskId)));
}

Task& TaskManager::editTask(id_t taskId, const Task& task) {
	if (!hasTask(taskId)) {
		throw new GetodoError("No such a task to edit: "
			+ boost::lexical_cast<std::string, id_t>(taskId));
	}

	// TODO: Write operator= for Task!!!

	// Note: Tags are synchronized in TaskPersistence::update().

	Task* taskCopy = new Task(task); // copy
	// Delete original task from tasks
	delete tasks[taskId];
	tasks[taskId] = 0;
	// Correct new task's taskId to be the same as the former's one
	taskCopy->setTaskId(taskId);
	// Copy new task there
	tasks[taskId] = taskCopy;
	// Save it to database
	TaskPersistence tp(conn, taskCopy);
	tp.update();
	signal_task_updated(*taskCopy);
	return *taskCopy;
}

void TaskManager::deleteTask(id_t taskId) {
	Task* task = getTask(taskId);
	if (task == 0) {
		throw new GetodoError("No such a task to delete: "
			+ boost::lexical_cast<std::string, id_t>(taskId));
	}

	Task* parent = 0;
	if (task->hasParent()) {
		id_t parentId = task->getParentId();
		parent = getTask(parentId);
		parent->removeSubtask(taskId);
		TaskPersistence& tp = getPersistentTask(parentId);
		tp.update();
	}

	//// connect deleted tasks's children to parent node if any
	//std::vector<id_t>& subtaskIds = task->getSubtaskIds();
	//for (std::vector<id_t>::iterator subtaskIdIt = subtaskIds.begin();
	//	subtaskIdIt != subtaskIds.end(); ++subtaskIdIt)
	//{
	//	Task* subtask = getTask(*subtaskIdIt);
	//	if (subtask != 0) {
	//		if (parent != 0) {
	//			subtask->setParent(*parent);
	//		} else {
	//			subtask->unsetParent();
	//		}
	//		TaskPersistence& tp = getPersistentTask(*subtaskIdIt);
	//		tp.save();
	//	}
	//}

	// delete the whole subtree
	std::vector<id_t>& subtaskIds = task->getSubtaskIds();
	for (std::vector<id_t>::iterator it = subtaskIds.begin();
		it != subtaskIds.end(); ++it)
	{
		deleteTask(*it); // recursion
	}

	signal_task_removed(*task);

	// erase from database
	TaskPersistence& tp = getPersistentTask(taskId);
	tp.erase();
	// erase from task manager
	tasks.erase(taskId);
}

std::vector<Task*>& TaskManager::getTasks() {
	return convertMapToVector<id_t, Task>(tasks);
}

std::vector<Task*>& TaskManager::getTopLevelTasks() {
	std::vector<Task*>& allTasks = getTasks();
	allTasks.erase(std::remove_if(allTasks.begin(), allTasks.end(),
		boost::bind(&Task::hasParent, _1)), allTasks.end());
	return allTasks;
}

// ----- Tag operations -----

id_t TaskManager::addTag(const Tag& tag) {
	// for now tag names are not unique
	if (hasTag(tag.id)) {
		Tag* existingTag = tags[tag.id];
		if (existingTag && (existingTag->name == tag.name)) {
			return tag.id; // already in TaskManager
			// or this could throw an exception...
		}
	}
	// if wa have unique tag name this would be sufficient:
	//if (hasTag(tag.name)) { return; }

	TagPersistence tp(conn);
	Tag* tagCopy = new Tag(tag);
	// when saving, id is assigned by database
	if (!tp.insert(*tagCopy)) {
		return Tag::INVALID_ID;
	}
	tags[tagCopy->id] = tagCopy;
	signal_tag_inserted(*tagCopy);
	return tagCopy->id;
}

bool TaskManager::hasTag(id_t tagId) {
	std::map<id_t,Tag*>::iterator it = tags.find(tagId);
	return (it != tags.end());
}

bool TaskManager::hasTag(const std::string& tagName) {
	std::map<id_t, Tag*>::iterator it;
	for (it = tags.begin(); it != tags.end(); ++it) {
		if (it->second && (it->second->name == tagName)) { return true; }
	}
	return false;
}

Tag& TaskManager::getTag(id_t tagId) {
	std::map<id_t,Tag*>::iterator foundTag = tags.find(tagId);
	if (foundTag == tags.end()) {
		throw new getodo::GetodoError("No such a tag: "
			+ boost::lexical_cast<std::string, id_t>(tagId));
	}
	return *(foundTag->second);

}

Tag& TaskManager::getTag(const std::string& tagName) {
	std::map<id_t, Tag*>::iterator it;
	for (it = tags.begin(); it != tags.end(); ++it) {
		if (it->second && (it->second->name == tagName)) { return *(it->second); }
	}
	throw new getodo::GetodoError("No such a tag: " + tagName);
}

Tag& TaskManager::editTag(id_t tagId, const Tag& tag) {
	if (!hasTag(tagId)) {
		throw new getodo::GetodoError("No such a tag to edit: "
			+ boost::lexical_cast<std::string, id_t>(tagId));
	}

	Tag* tagCopy = new Tag(tag);
	// Delete original tag from tags
	delete tags[tagId];
	tags[tagId] = 0;
	// correct new tag's tagId to be the same as the former's one
	tagCopy->id = tagId;
	// Copy new tag there
	tags[tagId] = tagCopy;
	// Save it to database
	TagPersistence p(conn);
	p.update(*tagCopy);
	signal_tag_updated(*tagCopy);
	return *tagCopy;
}

void TaskManager::deleteTag(id_t tagId) {
	std::map<id_t,Tag*>::iterator foundTag = tags.find(tagId);
	if (foundTag == tags.end()) {
		throw new getodo::GetodoError("No such a tag to delete: "
			+ boost::lexical_cast<std::string, id_t>(tagId));
	}
	signal_tag_removed(*foundTag->second);
	// erase from database
	TagPersistence p(conn);
	p.erase(tagId);
	// erase from task manager
	tags.erase(tagId);
}

std::vector<Tag*>& TaskManager::getTags() {
	return convertMapToVector<id_t, Tag>(tags);
}

// ----- FilterRule operations -----

id_t TaskManager::addFilterRule(const FilterRule& rule) {
	FilterRulePersistence p(conn);
	// when saving, filterRuleId is assigned by database
	FilterRule* ruleCopy = new FilterRule(rule);
	p.insert(*ruleCopy);
	filters[ruleCopy->id] = ruleCopy;
	signal_filter_inserted(*ruleCopy);
	return ruleCopy->id;
}

bool TaskManager::hasFilterRule(id_t filterRuleId) {
	std::map<id_t,FilterRule*>::iterator it = filters.find(filterRuleId);
	return (it != filters.end());
}

bool TaskManager::hasFilterRule(const std::string& filterRuleName) {
	std::pair<id_t, FilterRule*> pair;
	foreach(pair, filters) {
		if (pair.second && (pair.second->name == filterRuleName)) { return true; }
	}
	return false;
}

FilterRule& TaskManager::getFilterRule(id_t filterRuleId) {
	if (!hasFilterRule(filterRuleId)) {
		throw new GetodoError("No such a filter rule.");
	}
	return *(filters[filterRuleId]);
}

FilterRule& TaskManager::editFilterRule(id_t filterRuleId, const FilterRule& filter) {
	if (!hasFilterRule(filterRuleId)) {
		throw new getodo::GetodoError("No such a filter rule.");
	}

	FilterRule* ruleCopy = new FilterRule(filter);
	// correct new FilterRule's filterRuleId to be the same as former's one
	ruleCopy->id = filterRuleId;
	// Delete original FilterRule from filters
	delete filters[filterRuleId];
	filters[filterRuleId] = 0;
	// Copy new FilterRule there
	filters[filterRuleId] = ruleCopy;
	// Save it to database
	FilterRulePersistence p(conn);
	p.update(*ruleCopy);
	signal_filter_updated(*ruleCopy);
	return *ruleCopy;
}


void TaskManager::deleteFilterRule(id_t filterRuleId) {
	std::map<id_t,FilterRule*>::iterator foundFilter = filters.find(filterRuleId);
	if (foundFilter == filters.end()) {
		throw new getodo::GetodoError("No such a filter rule to delete: "
			+ boost::lexical_cast<std::string, id_t>(filterRuleId));
	}
	signal_filter_removed(*foundFilter->second);
	// erase from database
	FilterRulePersistence p(conn);
	p.erase(filterRuleId);
	// erase from task manager
	filters.erase(filterRuleId);
}

std::vector<FilterRule*>& TaskManager::getFilterRules() {
	return convertMapToVector<id_t, FilterRule>(filters);
}

idset_t& TaskManager::filterTasks(FilterRule& filterRule) {
	// TODO: rethrow the exception
	return filterRule.filter(*conn);
}

void TaskManager::setActiveFilterRule(const FilterRule& filter) {
	activeFilterRule = filter;
	try {
		visibleTasksCache = filterTasks(activeFilterRule);
	} catch (GetodoError& ex) {
		resetActiveFilterRule();
		// TODO: rethrow the exception
	}
	// DEBUG:
	//std::cout << "visibleTasksCache: ";
	//for (idset_t::iterator it = visibleTasksCache.begin();
	//	it != visibleTasksCache.end(); ++it)
	//{
	//		std::cout << *it << ", ";
	//}
	//std::cout << std::endl;
}

void TaskManager::resetActiveFilterRule() {
	activeFilterRule = FilterRule();
	visibleTasksCache.clear();
}

//FilterRule* TaskManager::getActiveFilterRule() const {
//	return activeFilterRule;
//}

bool TaskManager::hasActiveFilterRule() const {
	return !activeFilterRule.isEmpty();
}

bool TaskManager::isTaskVisible(id_t taskId) {
	return !hasActiveFilterRule()
		|| (visibleTasksCache.find(taskId) != visibleTasksCache.end());
}

idset_t TaskManager::getFilteredTasks() {
	return visibleTasksCache;
}

// ----- Other things -----

void TaskManager::tryLoadingFromDatabase() {
	if (checkDatabaseStructure()) {
		loadAllFromDatabase();
	} else {
		createEmptyDatabase();
	}
}

void TaskManager::loadAllFromDatabase() {
	if (!conn) { throw new GetodoError("No database connection."); }

	sqlite3_command cmd(*conn);
	sqlite3_cursor cursor;
	databaseRow_t row;
	int columnsCount;

	// load Tasks
	cmd.prepare("SELECT * FROM Task;");
	cursor = cmd.executecursor();
	columnsCount = cursor.colcount();
	while (cursor.step()) {
		for (int i = 0; i < columnsCount; i++) {
			std::string columnData;
			if (!cursor.isnull(i)) {
				columnData = cursor.getstring(i);
			}
			row[cursor.getcolname(i)] = columnData;
		}
		Task* task = Task::fromDatabaseRow(row);
		// what if there's an exception
		tasks[task->getTaskId()] = task;
		row.clear();
	}
	cursor.close();

	// load Tags
	cmd.prepare("SELECT * FROM Tag;");
	cursor = cmd.executecursor();
	columnsCount = cursor.colcount();
	while (cursor.step()) {
		// Remark: Don't assume any order of columns to make things
		// more robust with future changes in mind.
		for (int i = 0; i < columnsCount; i++) {
			std::string columnData;
			if (!cursor.isnull(i)) {
				columnData = cursor.getstring(i);
			}
			row[cursor.getcolname(i)] = columnData;
		}
		id_t tagId = boost::lexical_cast<id_t, std::string>(row["tagId"]);
		tags[tagId] = new Tag(tagId, row["tagName"]);
		row.clear();
	}
	cursor.close();

	// load Task-Tag relations
	cmd.prepare("SELECT * FROM Tagged;");
	cursor = cmd.executecursor();
	columnsCount = cursor.colcount();
	while (cursor.step()) {
		for (int i = 0; i < columnsCount; i++) {
			std::string columnData;
			if (!cursor.isnull(i)) {
				columnData = cursor.getstring(i);
			}
			row[cursor.getcolname(i)] = columnData;
		}
		id_t taskId = boost::lexical_cast<id_t, std::string>(row["taskId"]);
		id_t tagId = boost::lexical_cast<id_t, std::string>(row["tagId"]);
		if (hasTask(taskId) && hasTag(tagId)) {
			// at first check if the referenced task and tag really exist
			tasks[taskId]->addTag(tagId);
		}
		row.clear();
	}
	cursor.close();

	// load parent-subtask relations
	for (std::map<id_t,Task*>::iterator it = tasks.begin();
		it != tasks.end(); ++it)
	{
		if (!it->second) { continue; }
		cmd.prepare("SELECT taskId FROM Task WHERE parentId = ?;");
		cmd.bind(1, it->second->getTaskId());
		cursor = cmd.executecursor();
		while (cursor.step()) {
			id_t id = cursor.getint(0);
			//if (hasTask(id) { // TODO: think of if it is needed to check this
				it->second->addSubtask(id);
			//}
		}
		cursor.close();
	}

	// load FilterRules
	cmd.prepare("SELECT * FROM FilterRule;");
	cursor = cmd.executecursor();
	columnsCount = cursor.colcount();
	while (cursor.step()) {
		for (int i = 0; i < columnsCount; i++) {
			std::string columnData;
			if (!cursor.isnull(i)) {
				columnData = cursor.getstring(i);
			}
			row[cursor.getcolname(i)] = columnData;
		}
		id_t filterRuleId = boost::lexical_cast<id_t, std::string>(row["filterRuleId"]);
		filters[filterRuleId] = new FilterRule(filterRuleId, row["name"], row["rule"]);
		row.clear();
	}
	cursor.close();
}

// return true, if there exist all the tables needed
bool TaskManager::checkDatabaseStructure() {
	if (!conn) { throw new GetodoError("No database connection."); }
	// TODO
	// * this code could be optimized, using a set may be overkill
	// * table names shouldn't be hard-coded

	// ATTENTION: when changing number of tables, change
	// the upper limit in tablesNeeded!
	std::string tables[] = {"Task","Tag","Tagged","FilterRule"};
	std::set<std::string> tablesNeeded(&tables[0],&tables[4]);

	sqlite3_command cmd(*conn,"SELECT name FROM sqlite_master "
		"WHERE type='table' ORDER BY name;");
	sqlite3_cursor cursor = cmd.executecursor();
	std::set<std::string>::iterator tableIt;
	while (cursor.step()) {
		if (!cursor.isnull(0)) { // this check may not be needed here
			std::string table = cursor.getstring(0);
			tableIt = tablesNeeded.find(table);
			if (tableIt != tablesNeeded.end()) {
				tablesNeeded.erase(tableIt);
			}
		}
	}
	cursor.close();
	return tablesNeeded.empty();
}

void TaskManager::createEmptyDatabase() {
	if (!conn) { throw new GetodoError("No database connection."); }

	// TODO: better would be to include this SQL in the compile-time
	// from an external file
	sqlite3_command cmd(*conn);
	// Note: the command has to be split into separate queries
	cmd.prepare(
		"CREATE TABLE Task ("
		"taskId	  INTEGER	  NOT NULL,"
		"parentId	  INTEGER DEFAULT '-1' NOT NULL," //-1 -> Task::INVALID_ID
		"description	  STRING	  NOT NULL,"
		"longDescription	  STRING,"
		"dateCreated	  STRING	  NOT NULL,"
		"dateLastModified	  STRING	  NOT NULL,"
		"dateStarted	  STRING,"
		"dateDeadline	  STRING,"
		"dateCompleted	  STRING,"
		"estDuration	  STRING,"
		"recurrence	  STRING,"
		"priority	  STRING	  NOT NULL,"
		"completedPercentage	  INTEGER	  DEFAULT '0'  NOT NULL,"
		"done	  STRING	  DEFAULT '0'  NOT NULL,"
		"CONSTRAINT pk_Task PRIMARY KEY (taskId)"
		");"
	);
	cmd.executenonquery();

	cmd.prepare(
		"CREATE TABLE Tag ("
		"tagId	  INTEGER	  NOT NULL,"
		"tagName	  STRING	  NOT NULL,"  // +UNIQUE
		"CONSTRAINT pk_Tag PRIMARY KEY (tagId)"
		");"
	);
	cmd.executenonquery();

	cmd.prepare(
		"CREATE TABLE Tagged ("
		"taskId	  INTEGER	  NOT NULL,"
		"tagId	  INTEGER	  NOT NULL,"
		"CONSTRAINT pk_Tagged PRIMARY KEY (taskId, tagId),"
		"CONSTRAINT fk_Tagged_Task FOREIGN KEY (taskId) REFERENCES Task(taskId),"
		"CONSTRAINT fk_Tagged_Tag FOREIGN KEY (tagId) REFERENCES Tag(tagId)"
		");"
	);
	cmd.executenonquery();

	cmd.prepare(
		"CREATE TABLE FilterRule ("
		"filterRuleId	  INTEGER	  NOT NULL,"
		"name	  STRING	  NOT NULL," // +UNIQUE (?)
		"rule	  STRING	  NOT NULL,"
		"CONSTRAINT pk_FilterRule PRIMARY KEY (filterRuleId)"
		");"
	);
	cmd.executenonquery();
}

} // namespace getodo
