// $Id:$
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
	dateCreated = boost::posix_time::ptime(boost::gregorian::day_clock::local_day());
	dateLastModified = dateCreated;
	dateStarted = boost::gregorian::date(boost::date_time::not_a_date_time);
	dateDeadline = boost::gregorian::date(boost::date_time::not_a_date_time);
	dateCompleted = boost::gregorian::date(boost::date_time::not_a_date_time);
}

// copy constructor
Task::Task(Task const& t) :
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

id_t Task::getTaskId() { return taskId; }
void Task::setTaskId(id_t taskId) { this->taskId = taskId; }


std::string Task::getDescription() { return description; }
void Task::setDescription(const std::string description) { this->description = description; }

std::string Task::getLongDescription() { return longDescription; }
void Task::setLongDescription(const std::string longDescription) {
	this->longDescription = longDescription;
}

void Task::addTag(id_t tagId) {
	//should throw an exception on failure (?)
	tags.insert(tagId);
}
bool Task::hasTag(id_t tagId) {
	return (tags.find(tagId) != tags.end());
}
void Task::removeTag(id_t tagId) {
	//should throw an exception on failure
	tags.erase(tagId);
}

void Task::addSubTask(id_t taskId) {
	//should throw an exception on failure (?)
	subtasks.insert(taskId);
}
bool Task::hasTask(id_t taskId) {
	return (subtasks.find(taskId) != subtasks.end());
}
void Task::removeSubTask(id_t taskId) {
	//should throw an exception on failure
	subtasks.erase(taskId);
}

DateTime Task::getDateCreated() { return dateCreated; }
void Task::setDateCreated(const DateTime& dateCreated) { this->dateCreated = dateCreated; }

DateTime Task::getDateLastModified() { return dateLastModified; }
void Task::setDateLastModified(const DateTime& dateLastModified) { this->dateLastModified = dateLastModified; }

Date Task::getDateStarted() { return dateStarted; }
void Task::setDateStarted(const Date& dateStarted) { this->dateStarted = dateStarted; }

Date Task::getDateDeadline() { return dateDeadline; } // should be FuzzyDate
void Task::setDateDeadline(const Date& dateDeadline) { this->dateDeadline = dateDeadline; }

Date Task::getDateCompleted() { return dateCompleted; }
void Task::setDateCompleted(const Date& dateCompleted) { this->dateCompleted = dateCompleted; }

int Task::getPriority() { return priority; }
void Task::setPriority(int priority) { this->priority = priority; }

int Task::getCompletedPercentage() { return completedPercentage; }
void Task::setCompletedPercentage(int completedPercentage) { this->completedPercentage = completedPercentage; }
void Task::setDone() { setCompletedPercentage(100); } // TODO: ugly constant here

databaseRow_t Task::toDatabaseRow() {
	std::ostringstream ss;
	databaseRow_t row;

	ss << taskId;
	std::string taskIdStr = ss.str();
	ss.clear();
	row["taskId"] = taskIdStr; // (?)
	row["description"] = description;
	row["longDescription"] = longDescription;
	row["dateCreated"] = dateCreated.toString();
	row["dateStarted"] = dateStarted.toString();
	row["dateDeadline"] = dateDeadline.toString();
	row["dateCompleted"] = dateCompleted.toString();
	// row["estDuration"] = estDuration;
	// row["recurrence"] = recurrence;
	ss << priority;
	std::string priorityStr = ss.str();
	ss.clear();
	row["priority"] = priorityStr;
	ss << completedPercentage;
	std::string completedPercentageStr = ss.str();
	row["completedPercentage"] = completedPercentageStr;
	return row;
}

// static Task& Task::fromDatabaseRow(databaseRow_t) {
// 	std::istringstream ss;
// 	Task task = new Task();
// 	
// 	// TODO
// 	task.taskID = row["taskId"];
// }

// ----- object-relation representation conversion ----------


// ----- class TaskPersistence --------------------

} // namespace getodo

