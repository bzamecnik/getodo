// $Id$
//
// C++ Interface: TaskManager
//
// Description: 
//
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef LIBGETODO_TASKMANAGER_H
#define LIBGETODO_TASKMANAGER_H

#include "getodo.h"
#include "tag.h"
#include "filterrule.h"
#include "task.h"

namespace getodo {

class TaskManager {
private:
	std::map<id_t,Task*> tasks;
	std::map<id_t,Tag*> tags;
	std::list<FilterRule> filters;

	sqlite3x::sqlite3_connection* conn;
public:
	// TaskManager(); // for in-memory database (sqlite filename :memory:)
	TaskManager(std::string dbname);
	TaskManager(sqlite3x::sqlite3_connection* conn);
	virtual ~TaskManager();

	// tip: a function for switching database connection
	// eg. when Save As is invoked (from one file to another, or from memory to file)

	Task* addTask(Task* task);
	//bool hasTask(id_t taskId);
	Task* getTask(id_t taskId);
	TaskPersistence& getPersistentTask(id_t taskId); // for modyfing particular things
	Task* editTask(id_t taskId, const Task& task);
	void deleteTask(id_t taskId); //should throw an exception on failure

	Tag& addTag(const Tag& tag);
	bool hasTag(id_t tagId);
	Tag& getTag(id_t tagId);
	// Tag& getTagByName(std::string tagName);
	Tag& editTag(id_t tagId, const Tag& tag);
	void deleteTag(id_t tagId); //should throw an exception on failure

	FilterRule& addFilterRule(const FilterRule& filter);
	// bool hasFilterRule(id_t filterRuleId);
	FilterRule& getFilterRule(id_t filterRuleId);
	FilterRule& editFilterRule(id_t filterRuleId, const FilterRule& filter);
	void deleteFilterRule(id_t filterRuleId); //should throw an exception on failure

	taskset_t filterTasks(id_t filterRuleId);
	taskset_t filterTasks(const FilterRule& filterRule);
private:
	void loadAllFromDatabase(); // to be called by the constructor
	// void loadFromDatabase(const FilterRule& filter);
	
	void fillEmptyDatabase(); // make inital database scheme
};

} // namespace getodo

#endif // LIBGETODO_TASKMANAGER_H
