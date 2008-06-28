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

namespace getodo {

using namespace sqlite3x;

// ----- class TaskManager --------------------

TaskManager::TaskManager(std::string dbname) {
	//try {
	conn = new sqlite3_connection(dbname);
	//} catch(...) { return; }
	
	loadAllFromDatabase();
}

TaskManager::TaskManager(sqlite3_connection* c) : conn(c) {}

TaskManager::~TaskManager() {
	if(conn) { conn->close(); }
	// TODO: delete tasks, tags, filterrules
}

Task* TaskManager::addTask(Task* task) {
	if(!task) { return 0; } // throw an exception
	// insert the task into TaskManager...
	tasks.insert(std::pair<id_t,Task*>(task->getTaskId(),task));
	// ...and save it to database
	TaskPersistence tp(conn, task);
	tp.save();
	// taskId was assigned
	return tp.getTask();
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

// Task* TaskManager::editTask(id_t taskId, const Task& task);

void TaskManager::deleteTask(id_t taskId) {
	TaskPersistence& tp = getPersistentTask(taskId);
	tp.erase();
	tasks.erase(taskId);
}

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

void TaskManager::loadAllFromDatabase() {
	
}

} // namespace getodo
