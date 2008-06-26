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

namespace getodo {

Task::Task() :
	taskId(0),
	priority(0),
	completedPercentage(0)
{
	 dateCreated = boost::posix_time::ptime(boost::gregorian::day_clock::local_day());
	 dateLastModified = dateCreated;
	 dateStarted = boost::gregorian::date(boost::date_time::not_a_date_time);
	 dateDeadline = boost::gregorian::date(boost::date_time::not_a_date_time);
	 dateCompleted = boost::gregorian::date(boost::date_time::not_a_date_time);
}

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

id_t Task::getTaskId() {
	return taskId;
}

} // namespace getodo
