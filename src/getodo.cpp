// $Id$

#include "getodo.h"

namespace getodo {

Date Date::fromString(std::string str) {
	return boost::gregorian::from_string(str);
}

std::string Date::toString(Date const& date) {
	return boost::gregorian::to_iso_extended_string(date.date);
}

std::string Date::toString() {
	Date::toString(*this);
}


Task::Task()
:	taskId(0),
	priority(0),
	completedPercentage(0)
{
	 dateCreated = boost::posix_time::ptime(boost::gregorian::day_clock::local_day());
	 dateLastModified = dateCreated;
	 dateStarted = boost::gregorian::date(boost::date_time::not_a_date_time);
	 dateDeadline = boost::gregorian::date(boost::date_time::not_a_date_time);
	 dateCompleted = boost::gregorian::date(boost::date_time::not_a_date_time);
}

Task::Task(Task const& t)
:	taskId(t.taskId),
	description(t.description),
	longDescription(t.longDescription),
	tags(t.tags), // ok?
	subtasks(t.subtasks), // ok?
	dateCreated(t.dateCreated),
	dateLastModified(t.dateLastModified),
	dateStarted(t.dateStarted),
	dateDeadline(t.dateDeadline),
	dateCompleted(t.dateCompleted),
	priority(t.priority),
	completedPercentage(t.completedPercentage)
{}

Task::~Task() {
	tags.clear(); // is it needed?
	subtasks.clear(); // -//-
}

id_t Task::getTaskId() {
	return taskId;
}

TaskManager::TaskManager(std::string const& dbname) {
	//try {
	db = new sqlite3x::sqlite3_connection(dbname);
	//} catch(...) { return; }
	
	loadFromDatabase();
}

Task& TaskManager::addTask(Task* task) {
	if(task) tasks.insert(std::pair<id_t,Task*>(task->getTaskId(),task));
}

void TaskManager::loadFromDatabase() {
	
}

}  // namespace getodo
