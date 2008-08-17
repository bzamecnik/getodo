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
#include <sstream>

namespace getodo {

using namespace sqlite3x;

// ----- class TaskManager --------------------

// ----- Constructors & destructor -----

TaskManager::TaskManager(std::string dbname) {
	//try {
	conn = new sqlite3_connection(dbname);
	//} catch(...) { return; }
	if (checkDatabaseStructure()) {
		loadAllFromDatabase();
	} else {
		createEmptyDatabase();
	}
}

TaskManager::TaskManager(sqlite3_connection* c) : conn(c) {}

TaskManager::~TaskManager() {
	if(conn) { conn->close(); }
	// is it needed?
	tasks.clear();
	tags.clear();
	filters.clear();
}


// ----- SQLite connection -----

sqlite3_connection* TaskManager::getConnestion() {
	return conn;
}

// ----- Task operations -----

Task* TaskManager::addTask(Task* task) {
	if(!task) { return 0; } // throw an exception
	// save it to database and get the new taskID
	TaskPersistence tp(conn, task);
	tp.save();
	// insert the task into TaskManager
	tasks[task->getTaskId()] = task;
	return task;
}

bool TaskManager::hasTask(id_t taskId) {
	std::map<id_t,Task*>::iterator it = tasks.find(taskId);
	return (it != tasks.end());
}

Task* TaskManager::getTask(id_t taskId) {
	// TODO: how to find out the type of task automatically?
	// ie. without copying the type from the header file.
	std::map<id_t,Task*>::iterator it = tasks.find(taskId);
	if(it != tasks.end()) {
		return it->second;
	} else {
		return 0;
	}
}

TaskPersistence& TaskManager::getPersistentTask(id_t taskId) {
	return *(new TaskPersistence(conn, getTask(taskId)));
}

Task* TaskManager::editTask(Task* task) {
	// taskId is specified inside the task
	Task* t = getTask(task->getTaskId());
	if (!t) { return 0; }
	delete t;
	t = task;
	TaskPersistence tp(conn, t);
	tp.save();
	return task;
}

Task* TaskManager::editTask(id_t taskId, const Task& task) {
	Task * t = new Task(task); // copy
	t->setTaskId(taskId);
	return editTask(t);
}

void TaskManager::deleteTask(id_t taskId) {
	TaskPersistence& tp = getPersistentTask(taskId);
	tp.erase();
	tasks.erase(taskId);
}

// ----- Tag operations -----

Tag& TaskManager::addTag(const Tag& tag) {
	TagPersistence tp(conn);
	// when saving, tagId is assigned by database
	Tag* tagCopy = new Tag(tp.save(tag));
	tags[tagCopy->tagId] = tagCopy;
	return *tagCopy;
}

bool TaskManager::hasTag(id_t tagId) {
	std::map<id_t,Tag*>::iterator it = tags.find(tagId);
	return (it != tags.end());
}

Tag& TaskManager::getTag(id_t tagId) {
	if (hasTag(tagId)) {
		return *(tags[tagId]);
	} else {
		return *(new Tag()); // throw
	}
}

Tag& TaskManager::editTag(id_t tagId, const Tag& tag) {
	if (!hasTag(tagId)) { return *(new Tag()); } //throw
	
	// Delete original tag from tags
	delete tags[tagId];
	tags[tagId] = 0;
	// Copy new tag there
	Tag* tagCopy = new Tag(tag);
	tags[tagId] = tagCopy;
	// correct new tag's tagID to be the same as former's one
	tags[tagId]->tagId = tagId;
	// Save it to database
	TagPersistence p(conn);
	p.save(*tagCopy);
	return *tagCopy;
}

void TaskManager::deleteTag(id_t tagId) {
	// erase from database
	TagPersistence p(conn);
	p.erase(tagId);
	// erase from task manager
	tags.erase(tagId);
}

// ----- FilterRule operations -----

FilterRule& TaskManager::addFilterRule(const FilterRule& filter) {
	FilterRulePersistence p(conn);
	// when saving, filterRuleId is assigned by database
	FilterRule* ruleCopy = new FilterRule(p.save(filter));
	filters[ruleCopy->filterRuleId] = ruleCopy;
	return *ruleCopy;
}

bool TaskManager::hasFilterRule(id_t filterRuleId) {
	std::map<id_t,FilterRule*>::iterator it = filters.find(filterRuleId);
	return (it != filters.end());
}

FilterRule& TaskManager::getFilterRule(id_t filterRuleId) {
	if (hasFilterRule(filterRuleId)) {
		return *(filters[filterRuleId]);
	} else {
		return *(new FilterRule()); // throw
	}
}

FilterRule& TaskManager::editFilterRule(id_t filterRuleId, const FilterRule& filter) {
	if (!hasFilterRule(filterRuleId)) { return *(new FilterRule()); } //throw
	
	// Delete original FilterRule from filters
	delete filters[filterRuleId];
	filters[filterRuleId] = 0;
	// Copy new FilterRule there
	FilterRule* ruleCopy = new FilterRule(filter);
	filters[filterRuleId] = ruleCopy;
	// correct new FilterRule's filterRuleId to be the same as former's one
	filters[filterRuleId]->filterRuleId = filterRuleId;
	// Save it to database
	FilterRulePersistence p(conn);
	p.save(*ruleCopy);
	return *ruleCopy;
}

void TaskManager::deleteFilterRule(id_t filterRuleId) {
	// erase from database
	FilterRulePersistence p(conn);
	p.erase(filterRuleId);
	// erase from task manager
	filters.erase(filterRuleId);
}

// ----- Other things -----

void TaskManager::loadAllFromDatabase() {
	if (!conn) { return; } // throw
	
	sqlite3_command cmd(*conn);
	sqlite3_cursor cursor;
	databaseRow_t row;
	int columnsCount;
	std::stringstream ss;
	
	// load Tasks
	cmd.prepare("SELECT * FROM Task;");
	cursor = cmd.executecursor();
	columnsCount = cursor.colcount();
	while (cursor.step()) {
		for (int i = 0; i < columnsCount; i++) {
			row[cursor.getcolname(i)] = cursor.getstring(i);
		}
		Task* task = Task::fromDatabaseRow(row);
		// what if there's an exception
		addTask(task);
		row.clear();
	}
	
	// load Tags
	cmd.prepare("SELECT * FROM Tag;");
	cursor = cmd.executecursor();
	columnsCount = cursor.colcount();
	while (cursor.step()) {
		// Remark: Don't assume any order of columns to make things
		// more robust with future changes in mind.
		for (int i = 0; i < columnsCount; i++) {
			row[cursor.getcolname(i)] = cursor.getstring(i);
		}
		ss.str(row["tagId"]);
		id_t tagId;
		ss >> tagId;
		ss.clear();
		addTag(Tag(tagId, row["tagName"]));
		row.clear();
	}
	
	// load Task-Tag relations
	cmd.prepare("SELECT * FROM Tagged;");
	cursor = cmd.executecursor();
	columnsCount = cursor.colcount();
	while (cursor.step()) {
		for (int i = 0; i < columnsCount; i++) {
			row[cursor.getcolname(i)] = cursor.getstring(i);
		}
		// convert string -> id_t
		ss.str(row["taskId"]);
		id_t taskId;
		ss >> taskId;
		ss.clear();
		ss.str(row["tagId"]);
		id_t tagId;
		ss >> tagId;
		ss.clear();
		if (hasTask(taskId) && hasTag(tagId)) {
			// at first check if the referenced task and tag really exist
			tasks[taskId]->addTag(tagId);
		}
		row.clear();
	}
	
	// load Subtask relations
	cmd.prepare("SELECT * FROM Subtask;");
	cursor = cmd.executecursor();
	columnsCount = cursor.colcount();
	while (cursor.step()) {
		for (int i = 0; i < columnsCount; i++) {
			row[cursor.getcolname(i)] = cursor.getstring(i);
		}
		// convert string -> id_t
		ss.str(row["super_taskId"]);
		id_t super_taskId;
		ss >> super_taskId;
		ss.clear();
		ss.str(row["sub_taskId"]);
		id_t sub_taskId;
		ss >> sub_taskId;
		ss.clear();
		if (hasTask(super_taskId) && hasTask(sub_taskId)) {
			// at first check if the referenced tasks really exist
			tasks[super_taskId]->addSubtask(sub_taskId);
		}
		row.clear();
	}
	
	// load FilterRules
	cmd.prepare("SELECT * FROM FilterRule;");
	cursor = cmd.executecursor();
	columnsCount = cursor.colcount();
	while (cursor.step()) {
		for (int i = 0; i < columnsCount; i++) {
			row[cursor.getcolname(i)] = cursor.getstring(i);
		}
		// convert string -> id_t
		ss.str(row["filterRuleId"]);
		id_t filterRuleId;
		ss >> filterRuleId;
		ss.clear();
		addFilterRule(FilterRule(filterRuleId, row["name"], row["rule"]));
		row.clear();
	}
}

// return true, if there exist all the tables needed
bool TaskManager::checkDatabaseStructure() {
	if (!conn) { return false; } // throw
	// TODO
	// * this code could be optimized, using sets may be overkill
	// * tables named shouldn't be hard-coded
	std::set<std::string> tablesNeeded;
	tablesNeeded.insert("Task");
	tablesNeeded.insert("Tag");
	tablesNeeded.insert("Subtask");
	tablesNeeded.insert("Tagged");
	tablesNeeded.insert("FilterRule");
	sqlite3_command cmd(*conn,"SELECT name FROM sqlite_master "
		"WHERE type='table' ORDER BY name;");
	sqlite3_cursor cursor = cmd.executecursor();
	std::set<std::string>::iterator tableIt;
	while (cursor.step()) {
		std::string table = cursor.getstring(0);
		tableIt = tablesNeeded.find(table);
		if (tableIt != tablesNeeded.end()) {
			tablesNeeded.erase(tableIt);
		}
	}
	return tablesNeeded.empty();
}

void TaskManager::createEmptyDatabase() {
	if (!conn) { return; } // throw
	// TODO: better would be to include this SQL in the compile-time
	// from an external file
	sqlite3_command cmd(*conn);
	// the command has to be split into separate queries
	cmd.prepare(
		"CREATE TABLE Task ("
		"taskId      INTEGER      NOT NULL,"
		"description      STRING      NOT NULL,"
		"longDescription      STRING,"
		"dateCreated      STRING      NOT NULL,"
		"dateLastModified      STRING      NOT NULL,"
		"dateStarted      STRING,"
		"dateDeadline      STRING,"
		"dateCompleted      STRING,"
		"estDuration      STRING,"
		"recurrence      STRING,"
		"priority      STRING      NOT NULL,"
		"completedPercentage      INTEGER      DEFAULT '0'  NOT NULL,"
		"CONSTRAINT pk_Task PRIMARY KEY (taskId)"
		");"
	);
	cmd.executenonquery();
	
	cmd.prepare(
		"CREATE TABLE Tag ("
		"tagId      INTEGER      NOT NULL,"
		"tagName      STRING      NOT NULL  UNIQUE,"
		"CONSTRAINT pk_Tag PRIMARY KEY (tagId)"
		");"
	);
	cmd.executenonquery();
	
	cmd.prepare(
		"CREATE TABLE Subtask ("
		"sub_taskId      INTEGER      NOT NULL,"
		"super_taskId      INTEGER      NOT NULL,"
		"CONSTRAINT pk_Subtask PRIMARY KEY (sub_taskId, super_taskId),"
		"CONSTRAINT fk_Subtask_Task FOREIGN KEY (sub_taskId, super_taskId) "
		"REFERENCES Task(taskId,taskId)"
		");"
	);
	cmd.executenonquery();
	
	cmd.prepare(
		"CREATE TABLE Tagged ("
		"taskId      INTEGER      NOT NULL,"
		"tagId      INTEGER      NOT NULL,"
		"CONSTRAINT pk_Tagged PRIMARY KEY (taskId, tagId),"
		"CONSTRAINT fk_Tagged_Task FOREIGN KEY (taskId) REFERENCES Task(taskId),"
		"CONSTRAINT fk_Tagged_Tag FOREIGN KEY (tagId) REFERENCES Tag(tagId)"
		");"
	);
	cmd.executenonquery();
	
	cmd.prepare(
		"CREATE TABLE FilterRule ("
		"filterRuleId      INTEGER      NOT NULL,"
		"name      STRING      NOT NULL  UNIQUE,"
		"rule      STRING      NOT NULL,"
		"CONSTRAINT pk_FilterRule PRIMARY KEY (filterRuleId)"
		");"
	);
	cmd.executenonquery();
}

} // namespace getodo
