// $Id:$
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

TaskManager::TaskManager(std::string const& dbname) {
	//try {
	db = new sqlite3x::sqlite3_connection(dbname);
	//} catch(...) { return; }
	
	loadAllFromDatabase();
}

Task* TaskManager::addTask(Task* task) {
	if(task) tasks.insert(std::pair<id_t,Task*>(task->getTaskId(),task));
}

void TaskManager::loadAllFromDatabase() {
	
}

} // namespace getodo
