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

#include "common.h"
#include "tag.h"
#include <sstream>

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
	// id_t parent; // TODO

	// bool done; // TODO
	// this might be variant in future, let's have it private
	DateTime dateCreated;
	DateTime dateLastModified;
	Date dateStarted;
	Date dateDeadline; // should be FuzzyDate
	Date dateCompleted;
//	Duration& estDuration; // estimated duration
	Recurrence* recurrence;
	
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
	void setDescription(const std::string& description);

	std::string getLongDescription() const;
	void setLongDescription(const std::string& longDescription);

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

	Recurrence& getRecurrence() const;
	void setRecurrence(Recurrence* r);

	int getPriority() const;
	void setPriority(int priority);

	int getCompletedPercentage() const;
	void setCompletedPercentage(int completedPercentage);
	//void setDone();
	// TODO:
	//bool isDone();
	//void setDone(bool done = true);

	// ----- object-relation representation conversion ----------
	databaseRow_t toDatabaseRow() const;
	static Task* fromDatabaseRow(databaseRow_t);
private:
	template<typename T>
	std::list<T> convertSetToList(std::set<T> s) const {
		std::list<T> list;
		std::set<T>::const_iterator it;
		for (it = s.begin(); it != s.end(); it++) {
			list.push_front(*it);
		}
		return list;
	}
};

// ----- class TaskPersistence --------------------

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
	// TODO: split into insert() and update()
	void save();
	// load Task from database
	Task* load(id_t taskId);

	void erase();

	Task* getTask() const;
	void setTask(Task* task);

	// Wrappers of member functions from Task that modify the Task
	// - will call original function
	// - will make SQL query to reflect changes into database
	// Reason: We don't want to always save whole object, while making small changes.

	void setDescription(const std::string& description);
	void setLongDescription(const std::string& longDescription);

	void addTag(id_t tagId);
	void removeTag(id_t tagId);

	void addSubtask(id_t taskId);
	void removeSubtask(id_t taskId);

	void setDateCreated(const DateTime& dateCreated);
	void setDateLastModified(const DateTime& dateLastModified);
	void setDateStarted(const Date& dateStarted);
	void setDateDeadline(const Date& dateDeadline);
	void setDateCompleted(const Date& dateCompleted);

	void setRecurrence(const Recurrence& r);

	void setPriority(int priority);

	void setCompletedPercentage(int completedPercentage);
	//void setDone();
private:
	// update a single column and also last modified date
	template<typename T>
	void setColumn(std::string columnName, T value) {
		if(!conn || !task || (task->getTaskId() >= 0)) {
			return; //TODO: throw
		}
		sqlite3_command cmd(*conn);
		std::ostringstream ss;
		ss << "UPDATE Task SET " << columnName << " = ?, "
			"dateLastModified = ? WHERE taskId = ?;";
		cmd.prepare(ss.str());
		cmd.bind(1, value);
		cmd.bind(2, DateTime::now().toString());
		cmd.bind(3, task->getTaskId());
		cmd.executenonquery();
	}
};

} // namespace getodo

#endif // LIBGETODO_TASK_H
