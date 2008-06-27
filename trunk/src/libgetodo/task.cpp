// $Id$
//
// C++ Implementation: task
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
#include "task.h"
#include <sstream>

namespace getodo {

// ----- class Task --------------------

// ----- constructors ----------

// empty constructor
Task::Task() :
	taskId(-1),
	priority(0),
	completedPercentage(0)
{
	using namespace boost;
	dateCreated = posix_time::ptime(gregorian::day_clock::local_day());
	dateLastModified = dateCreated;
	dateStarted = gregorian::date(date_time::not_a_date_time);
	dateDeadline = gregorian::date(date_time::not_a_date_time);
	dateCompleted = gregorian::date(date_time::not_a_date_time);
}

// copy constructor
Task::Task(const Task& t) :
	taskId(t.taskId),
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

// ----- access methods ----------

id_t Task::getTaskId() const { return taskId; }
void Task::setTaskId(id_t taskId) { this->taskId = taskId; }


std::string Task::getDescription() const { return description; }
void Task::setDescription(const std::string description) {
	this->description = description;
}

std::string Task::getLongDescription() const { return longDescription; }
void Task::setLongDescription(const std::string longDescription) {
	this->longDescription = longDescription;
}

void Task::addTag(id_t tagId) {
	//should throw an exception on failure (?)
	tags.insert(tagId);
}
bool Task::hasTag(id_t tagId) const {
	return (tags.find(tagId) != tags.end());
}
void Task::removeTag(id_t tagId) {
	//should throw an exception on failure
	tags.erase(tagId);
}
// should share a common function set->list with getSubtasksList
std::list<id_t> Task::getTagsList() const {
	std::list<id_t> list;
	std::set<id_t>::const_iterator it;
	for(it = tags.begin(); it != tags.end(); it++) {
		list.push_front(*it);
	}
	return list;
}

void Task::addSubTask(id_t taskId) {
	//should throw an exception on failure (?)
	subtasks.insert(taskId);
}

bool Task::hasTask(id_t taskId) const {
	return (subtasks.find(taskId) != subtasks.end());
}

void Task::removeSubTask(id_t taskId) {
	//should throw an exception on failure
	subtasks.erase(taskId);
}

std::list<id_t> Task::getSubtasksList() const {
	std::list<id_t> list;
	std::set<id_t>::const_iterator it;
	for(it = subtasks.begin(); it != subtasks.end(); it++) {
		list.push_front(*it);
	}
	return list;
}

DateTime Task::getDateCreated() const { return dateCreated; }

void Task::setDateCreated(const DateTime& dateCreated) {
	this->dateCreated = dateCreated;
}

DateTime Task::getDateLastModified() const { return dateLastModified; }
void Task::setDateLastModified(const DateTime& dateLastModified) {
	this->dateLastModified = dateLastModified;
}

Date Task::getDateStarted() const { return dateStarted; }
void Task::setDateStarted(const Date& dateStarted) {
	this->dateStarted = dateStarted;
}

Date Task::getDateDeadline() const { return dateDeadline; } // should be FuzzyDate
void Task::setDateDeadline(const Date& dateDeadline) {
	this->dateDeadline = dateDeadline;
}

Date Task::getDateCompleted() const { return dateCompleted; }
void Task::setDateCompleted(const Date& dateCompleted) {
	this->dateCompleted = dateCompleted;
}

int Task::getPriority() const { return priority; }
void Task::setPriority(int priority) { this->priority = priority; }

int Task::getCompletedPercentage() const { return completedPercentage; }
void Task::setCompletedPercentage(int completedPercentage) {
	this->completedPercentage = completedPercentage;
}
void Task::setDone() { setCompletedPercentage(100); } // TODO: ugly constant here

// ----- object-relation representation conversion ----------

databaseRow_t Task::toDatabaseRow(const Task& task) {
	std::ostringstream ss;
	databaseRow_t row;

	ss << task.taskId;
	std::string taskIdStr = ss.str();
	ss.clear();
	row["taskId"] = taskIdStr; // (?)
	row["description"] = task.description;
	row["longDescription"] = task.longDescription;
	row["dateCreated"] = task.dateCreated.toString();
	row["dateLastModified"] = task.dateLastModified.toString();
	row["dateStarted"] = task.dateStarted.toString();
	row["dateDeadline"] = task.dateDeadline.toString();
	row["dateCompleted"] = task.dateCompleted.toString();
	// row["estDuration"] = task.estDuration;
	// row["recurrence"] = task.recurrence;
	ss << task.priority;
	std::string priorityStr = ss.str();
	ss.clear();
	row["priority"] = priorityStr;
	ss << task.completedPercentage;
	std::string completedPercentageStr = ss.str();
	row["completedPercentage"] = completedPercentageStr;
	return row;
}

databaseRow_t Task::toDatabaseRow() const {
	return toDatabaseRow(*this);
}

Task& Task::fromDatabaseRow(databaseRow_t row) {
	std::istringstream ss;
	Task* task = new Task();
	
	ss.str(row["taskId"]);
	ss >> task->taskId;
	ss.clear();
	
	task->description = row["description"];
	task->longDescription = row["longDescription"];
	
	task->dateCreated = DateTime::fromString(row["dateCreated"]);
	task->dateLastModified = DateTime::fromString(row["dateLastModified"]);
	task->dateStarted = Date::fromString(row["dateStarted"]);
	task->dateDeadline = Date::fromString(row["dateDeadline"]);
	task->dateCompleted = Date::fromString(row["dateCompleted"]);
	
	// estDuration = Duration.fromString(row["estDuration"]);
	// recurrence = Recurrence.fromString(row["recurrence"]);
	
	ss.str(row["priority"]);
	ss >> task->priority;
	ss.clear();
	
	ss.str(row["completedPercentage"]);
	ss >> task->completedPercentage;
	ss.clear();
	
	return *task;
}

// ----- class TaskPersistence --------------------

// Constructor for loading new Tasks
TaskPersistence::TaskPersistence(sqlite3_connection* c)
	: conn(c), task(0) {}

// Constructor for modifying particular things in a Task
TaskPersistence::TaskPersistence(sqlite3x::sqlite3_connection* c, Task* t)
	: conn(c), task(t) {}

// save whole Task to database
// void TaskPersistence::save() {
// 	// - task.toDatabaseRow() --> Task table (one row)
// 	// - task.getTagsList() --> Tagged table in a loop
// 	// - task.getSubtasksList() --> Subtask table in a loop
// }

// void TaskPersistence::save() {
// 
// }

Task* TaskPersistence::getTask() {
	return task;
}

} // namespace getodo

