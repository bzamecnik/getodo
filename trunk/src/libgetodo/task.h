// $Id$
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

#include "getodo.h"
#include "tag.h"
#include <set>

namespace getodo {

typedef std::set<id_t> taskset_t;

// ----- class Task --------------------

class Task {
private:
	id_t taskId; // -1, if not already in database
	std::string description;
	std::string longDescription; // including links, attachments, etc.

	// Using sets of id_t here is better than Task& because of possible
	// cycles in Subtask relation while loading from database.
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
	// ----- Constructors -----
	Task();
	Task(const Task& t); // copy constructor
	virtual ~Task();

	// ----- Getters & Setters -----

	id_t getTaskId() const;
	void setTaskId(id_t taskId);

	std::string getDescription() const;
	void setDescription(const std::string description);

	std::string getLongDescription() const;
	void setLongDescription(const std::string longDescription);

	void addTag(id_t tagId); //should throw an exception on failure (?)
	bool hasTag(id_t tagId) const;
	void removeTag(id_t tagId); //should throw an exception on failure
	std::list<id_t> getTagsList() const;

	void addSubtask(id_t taskId); //should throw an exception on failure (?)
	bool hasSubtask(id_t taskId) const;
	void removeSubtask(id_t taskId); //should throw an exception on failure
	std::list<id_t> getSubtasksList() const;

	DateTime getDateCreated() const;
	void setDateCreated(const DateTime& dateCreated);

	DateTime getDateLastModified() const;
	void setDateLastModified(const DateTime& dateLastModified);

	Date getDateStarted() const;
	void setDateStarted(const Date& dateStarted);

	Date getDateDeadline() const; // should be FuzzyDate
	void setDateDeadline(const Date& dateDeadline);

	Date getDateCompleted() const;
	void setDateCompleted(const Date& dateCompleted);

	int getPriority() const;
	void setPriority(int priority);

	int getCompletedPercentage() const;
	void setCompletedPercentage(int completedPercentage);
	void setDone();

	// ----- object-relation representation conversion ----------
	static databaseRow_t toDatabaseRow(const Task& task);
	databaseRow_t toDatabaseRow() const;
	static Task* fromDatabaseRow(databaseRow_t);
};

// ----- class Task --------------------

class TaskPersistence {
private:
	sqlite3x::sqlite3_connection* conn;
	Task* task;
public:
	// Constructor for loading new Tasks
	TaskPersistence(sqlite3x::sqlite3_connection* conn);
	// Constructor for modifying particular things in a Task
	TaskPersistence(sqlite3x::sqlite3_connection* conn, Task* task);
	~TaskPersistence();
	
	// save whole Task to database
	void save(); // TODO
	// load Task from database
	Task* load(id_t taskId);

	void erase();

	Task* getTask() const;
	void setTask(Task* task);

	// Wrappers of member functions from Task that modify the Task
	// - will call original function
	// - will make SQL query to reflect changes into database
	// Reason: We don't want to always save whole object, while making small changes.

	void setDescription(const std::string description); // TODO
	void setLongDescription(const std::string longDescription); // TODO

	void addTag(id_t tagId);  // TODO //should throw an exception on failure (?)
	void removeTag(id_t tagId);  // TODO //should throw an exception on failure

	void addSubtask(id_t taskId);  // TODO //should throw an exception on failure (?)
	void removeSubtask(id_t taskId);  // TODO //should throw an exception on failure

	void setDateCreated(const DateTime& dateCreated); // TODO
	void setDateLastModified(const DateTime& dateLastModified); // TODO
	void setDateStarted(const Date& dateStarted); // TODO
	void setDateDeadline(const Date& dateDeadline); // TODO
	void setDateCompleted(const Date& dateCompleted); // TODO

	void setPriority(int priority); // TODO

	void setCompletedPercentage(int completedPercentage); // TODO
	void setDone(); // TODO
};

} // namespace getodo

#endif // LIBGETODO_TASK_H
