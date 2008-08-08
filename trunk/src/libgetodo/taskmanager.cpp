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

#include "getodo.h"
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
	
	loadAllFromDatabase();
}

TaskManager::TaskManager(sqlite3_connection* c) : conn(c) {}

TaskManager::~TaskManager() {
	if(conn) { conn->close(); }
	// is it needed?
	tasks.clear();
	tags.clear();
	filters.clear();
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
	while(cursor.step()) {
		for (int i = 1; i <= columnsCount; i++) {
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
	while(cursor.step()) {
		// Remark: Don't assume any order of columns to make things
		// more robust with future changes in mind.
		for (int i = 1; i <= columnsCount; i++) {
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
	while(cursor.step()) {
		for (int i = 1; i <= columnsCount; i++) {
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
	while(cursor.step()) {
		for (int i = 1; i <= columnsCount; i++) {
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
	while(cursor.step()) {
		for (int i = 1; i <= columnsCount; i++) {
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

} // namespace getodo
