// $Id:$
//
// C++ Interface: Task
//
// Description: 
//
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef LIBGETODO_TASK_H
#define LIBGETODO_TASK_H

#include <set>
#include "getodo.h"
#include "tag.h"

namespace getodo {

typedef std::set<id_t> taskset_t;

class Task {
	id_t taskId; // -1, if not already in database
	std::string description;
	std::string longDescription; // including links, attachments, etc.

	// Using sets of id_t here is better than Task& because of possible
	// cycles in SubTask relation while loading from database.
	std::set<id_t> tags;
	std::set<id_t> subtasks;

	DateTime dateCreated;
	DateTime dateLastModified;
	Date dateStarted;
	Date dateDeadline; // should be FuzzyDate
	Date dateCompleted;
//	Duration& estDuration; // estimated duration
//	Recurrence& recurrence;
	
	int priority; // TODO: number or symbol?
	int completedPercentage;

public:
	// Constructors
	Task();
	Task(const Task& t); // copy constructor
	virtual ~Task(); // delete tags, subtasks

	// Getters & Setters

	id_t getTaskId();
	void setTaskId(id_t taskId);

	std::string getDescription();
	void setDescription(const std::string description);

	std::string getLongDescription();
	void setLongDescription(const std::string longDescription);

	void addTag(id_t tagId); //should throw an exception on failure (?)
	bool hasTag(id_t tagId);
	void removeTag(id_t tagId); //should throw an exception on failure

	void addSubTask(id_t taskId); //should throw an exception on failure (?)
	bool hasTask(id_t taskId);
	void removeSubTask(id_t taskId); //should throw an exception on failure

	DateTime getDateCreated();
	void setDateCreated(const DateTime& dateCreated);

	DateTime getDateLastModified();
	void setDateLastModified(const DateTime& dateLastModified);

	Date getDateStarted();
	void setDateStarted(const Date& dateStarted);

	Date getDateDeadline(); // should be FuzzyDate
	void setDateDeadline(const Date& dateDeadline);

	Date getDateCompleted();
	void setDateCompleted(const Date& dateCompleted);

	int getPriority();
	void setPriority(int priority);

	int getCompletedPercentage();
	void setCompletedPercentage(int completedPercentage);
	void setDone();

	// Convert representation: database rows(s) <-> object
	static databaseRow_t toDatabaseRow(const Task& task);
	databaseRow_t toDatabaseRow();
	static Task& fromDatabaseRow(databaseRow_t);

	std::list<databaseRow_t> toSubtaskRelation();
	std::list<databaseRow_t> toTagRelation();
};

class TaskPersistence {
	sqlite3x::sqlite3_connection* conn;
	Task* task;
public:
	// Constructor for loading new Tasks
	TaskPersistence(sqlite3x::sqlite3_connection* conn);
	// Constructor for modifying particular things in a Task
	TaskPersistence(sqlite3x::sqlite3_connection* conn, Task& task);
	// save whole Task to database
	void save();
	// load Task from database
	Task& load(id_t taskId);

	Task* getTask();
	
	// Wrappers of member functions from Task that modify the Task
	// - will call original function
	// - will make SQL query to reflect changes into database
	// Reason: We don't want to always save whole object, while making small changes.

	void setDescription(const std::string description);

	void setLongDescription(const std::string longDescription);

	void addTag(id_t tagId); //should throw an exception on failure (?)
	void removeTag(id_t tagId); //should throw an exception on failure

	void addSubTask(id_t taskId); //should throw an exception on failure (?)
	void removeSubTask(id_t taskId); //should throw an exception on failure

	void setDateCreated(const DateTime& dateCreated);

	void setDateLastModified(const DateTime& dateLastModified);

	void setDateStarted(const Date& dateStarted);

	void setDateDeadline(const Date& dateDeadline);

	void setDateCompleted(const Date& dateCompleted);

	void setPriority(int priority);

	void setCompletedPercentage(int completedPercentage);
	void setDone();
};

} // namespace getodo

#endif // LIBGETODO_TASK_H
