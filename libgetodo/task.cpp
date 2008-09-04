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

#include "task.h"

using namespace sqlite3x;

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
	dateCreated = DateTime::now();
	dateLastModified = dateCreated;
	dateStarted = gregorian::date(date_time::not_a_date_time);
	dateDeadline = gregorian::date(date_time::not_a_date_time);
	dateCompleted = gregorian::date(date_time::not_a_date_time);
	recurrence = new RecurrenceOnce();
}

// copy constructor
Task::Task(const Task& t) :
	taskId(t.taskId),
	description(t.description),
	longDescription(t.longDescription),
	tags(t.tags), // TODO: ok?
	// subtasks(t.subtasks), // don't allow to make circles
	// * OR: copy the whole subtree
	dateCreated(t.dateCreated),
	dateLastModified(t.dateLastModified),
	dateStarted(t.dateStarted),
	dateDeadline(t.dateDeadline),
	dateCompleted(t.dateCompleted),
	priority(t.priority),
	completedPercentage(t.completedPercentage)
{
	if (t.recurrence) {
		// recurrence must be cloned
		recurrence = t.recurrence->clone();
	} else {
		recurrence = new RecurrenceOnce();
	}
}

Task::~Task() {
	// is it really needed?
	tags.clear();
	subtasks.clear();
	if (recurrence) {
		delete recurrence;
		recurrence = 0;
	}
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
	tags.insert(tagId);
}
bool Task::hasTag(id_t tagId) const {
	return (tags.find(tagId) != tags.end());
}
void Task::removeTag(id_t tagId) {
	//should throw an exception on failure
	tags.erase(tagId);
}
std::list<id_t> Task::getTagsList() const {
	return convertSetToList<id_t>(tags);
}

void Task::addSubtask(id_t taskId) {
	//should throw an exception on failure (?)
	subtasks.insert(taskId);
}
bool Task::hasSubtask(id_t taskId) const {
	return (subtasks.find(taskId) != subtasks.end());
}
void Task::removeSubtask(id_t taskId) {
	//should throw an exception on failure
	subtasks.erase(taskId);
}
std::list<id_t> Task::getSubtasksList() const {
	return convertSetToList<id_t>(subtasks);
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

Date Task::getDateDeadline() const { return dateDeadline; } // should be a FuzzyDate
void Task::setDateDeadline(const Date& dateDeadline) {
	this->dateDeadline = dateDeadline;
}

Date Task::getDateCompleted() const { return dateCompleted; }
void Task::setDateCompleted(const Date& dateCompleted) {
	this->dateCompleted = dateCompleted;
}

Recurrence& Task::getRecurrence() const {
	if (!recurrence) {
		return *(new RecurrenceOnce());
	} else {
		return *recurrence;
	}
}
void Task::setRecurrence(Recurrence* r) {
	if (!r) {
		r = new RecurrenceOnce();
	} else {
		delete recurrence;
		recurrence = 0;
	}
	recurrence = r;
}

int Task::getPriority() const { return priority; }
void Task::setPriority(int priority) { this->priority = priority; }

int Task::getCompletedPercentage() const { return completedPercentage; }
void Task::setCompletedPercentage(int completedPercentage) {
	this->completedPercentage = completedPercentage;
}
void Task::setDone() { setCompletedPercentage(100); } // TODO: ugly constant here

// ----- object-relation representation conversion ----------

databaseRow_t Task::toDatabaseRow() const {
	databaseRow_t row;

	row["taskId"] = boost::lexical_cast<std::string, id_t>(taskId);

	row["description"] = description;
	row["longDescription"] = longDescription;

	row["dateCreated"] = dateCreated.toString();
	row["dateLastModified"] = dateLastModified.toString();
	row["dateStarted"] = dateStarted.toString();
	row["dateDeadline"] = dateDeadline.toString();
	row["dateCompleted"] = dateCompleted.toString();

	// row["estDuration"] = estDuration;
	row["recurrence"] = Recurrence::toString(*recurrence);

	row["priority"] = boost::lexical_cast<std::string, int>(priority);
	row["completedPercentage"] = boost::lexical_cast<std::string, int>(completedPercentage);

	return row;
}

Task* Task::fromDatabaseRow(databaseRow_t row) {
	Task* task = new Task();
	
	try {
		task->taskId = boost::lexical_cast<id_t, std::string>(row["taskId"]);
	} catch (boost::bad_lexical_cast &e) {
		task->taskId = -1;
	}
	
	task->description = row["description"];
	task->longDescription = row["longDescription"];
	
	task->dateCreated = DateTime(row["dateCreated"]);
	task->dateLastModified = DateTime(row["dateLastModified"]);
	task->dateStarted = Date(row["dateStarted"]);
	task->dateDeadline = Date(row["dateDeadline"]);
	task->dateCompleted = Date(row["dateCompleted"]);
	
	// estDuration = Duration(row["estDuration"]);
	task->recurrence = Recurrence::fromString(row["recurrence"]);
	
	try {
		task->priority = boost::lexical_cast<int, std::string>(row["priority"]);
	} catch (boost::bad_lexical_cast &e) {
		task->priority = 0; // TODO: default priority
	}

	try {
		task->completedPercentage = boost::lexical_cast<int, std::string>(row["completedPercentage"]);
	} catch (boost::bad_lexical_cast &e) {
		task->completedPercentage = 0; // TODO: default completedPercentage
	}
	
	return task;
}

// ----- class TaskPersistence --------------------

// Constructor for loading new Tasks
TaskPersistence::TaskPersistence(sqlite3_connection* c)
	: conn(c), task(0) {}

// Constructor for modifying particular things in a Task
TaskPersistence::TaskPersistence(sqlite3_connection* c, Task* t)
	: conn(c), task(t) {}

TaskPersistence::~TaskPersistence() {}

// save whole Task to database
void TaskPersistence::save() {
 	// if(!conn) { TODO: throw }
	
	// save the task
	
	databaseRow_t row = task->toDatabaseRow();
	// TODO: delete this when Duration and Recurence will be ready
	row["estDuration"] = "";
	//row["recurrence"] = "";
	row["dateLastModified"] = DateTime::now().toString();
	
	int count = 0;
	if (task->getTaskId() >= 0) {
		// task has already its taskId, find out if it is in the db
		sqlite3_command cmd(*conn, "SELECT count(*) FROM Task "
			"WHERE taskId = (?);");
		cmd.bind(1, row["taskId"]);
		count = cmd.executeint();
	}
	if (count > 0) {
		// it is already there -> update
		
		// build the SQL command
		std::ostringstream ss;
		ss << "UPDATE Task SET ";
		databaseRow_t::const_iterator col = row.begin();
		for (; col != (row.end()--); col++) {
			// don't overwrite the taskId
			if (col->first == "taskId") { continue; }
			ss << col->first << " = '" << col->second << "', ";
		}
		if (col != row.end()) {
			// the last column without comma
			ss << col->first << " = '" << col->second << "' ";
		}
		ss << "WHERE taskId = " << row["taskId"] << ";";
		sqlite3_command cmd(*conn, ss.str());
		ss.str(""); // clear the stream
		cmd.executenonquery();
	} else {
		// it is not in the db -> insert
		
		std::ostringstream sql;
		std::ostringstream values;
		sql << "INSERT INTO Task (";
		databaseRow_t::const_iterator col = row.begin();
		for (; col != (row.end()--); col++) {
			sql << col->first << ", ";
			values << "'" << col->second << "',";
		}
		if (col != row.end()) {
			// the last column without comma
			sql << col->first << ' ';
			values << "'" << col->second << "'";
		}
		sql << ") VALUES (" << values.str() << ");";
		sqlite3_command cmd(*conn, sql.str());
		sql.str(""); // clear the stream
		values.str(""); // clear the stream
		cmd.executenonquery();
		
		// the database automatically created a new taskId
		// set it to the task object
		task->setTaskId(sqlite3_last_insert_rowid(conn->db()));
		
		// set it to the newly created database row too
		// this row is identified by a special ROWID column
		cmd.prepare("INSERT INTO Task (taskId) VALUES (?) "
			"WHERE ROWID = ?");
		cmd.bind(1, task->getTaskId());
		cmd.executenonquery();
	}
	
 	// save tags
	
	std::list<id_t> tags = task->getTagsList();
	std::list<id_t>::const_iterator tag;
	for (tag = tags.begin(); tag != tags.end(); tag++) {
		addTag(*tag);
	}
	
 	// save subtasks
	
	std::list<id_t> subtasks = task->getSubtasksList();
	std::list<id_t>::const_iterator subtask;
	for (subtask = subtasks.begin(); subtask != subtasks.end(); subtask++) {
		addSubtask(*subtask);
	}
}

Task* TaskPersistence::load(id_t taskId) {
	// if(!conn) { TODO: throw ...}
	
	// Load task itself
	
	sqlite3_command cmd(*conn, "SELECT * FROM Task WHERE taskId = (?);");
	cmd.bind(1, taskId);
	sqlite3_cursor cursor = cmd.executecursor();
	if (!cursor.step()) {
		// TODO: throw, if there is not record  with this tagId
		return 0;
	}
	databaseRow_t row;
	int columnsCount = cursor.colcount();
	for (int i = 0; i < columnsCount; i++) {
		row[cursor.getcolname(i)] = cursor.getstring(i);
	}
	cursor.close();
	task = Task::fromDatabaseRow(row);
	if(!task) {
		// TODO: throw an exception
		return 0;
	}
	
	// Load its tags
	id_t id;
	
	cmd.prepare("SELECT tagId FROM Tagged WHERE taskId = ?;");
	cmd.bind(1, taskId);
	cursor = cmd.executecursor();
	
	while (cursor.step()) {
		id = cursor.getint(0);
		task->addTag(id);
	}
	cursor.close();
	
	// Load its subtasks
	
	cmd.prepare("SELECT sub_taskId FROM Subtask WHERE super_taskId = ?;");
	cmd.bind(1, taskId);
	cursor = cmd.executecursor();
	while (cursor.step()) {
		id = cursor.getint(0);
		task->addSubtask(id);
	}
	cursor.close();

	return task;
}

void TaskPersistence::erase() {
	if(!conn) { return; } // throw
	if(!task) { return; } // throw
	if(task->getTaskId() < 0) { return; } // throw
	
	sqlite3_command cmd(*conn, "DELETE FROM Tagged WHERE taskId = ?;");
	cmd.bind(1, task->getTaskId());
	cmd.executenonquery();
	
	// TODO: what about subtasks?
	// * delete them too
	// * make them level 1 tasks

	cmd.prepare("DELETE FROM Subtasks WHERE super_taskId = ?;");
	cmd.bind(1, task->getTaskId());
	cmd.executenonquery();
	
	cmd.prepare("DELETE FROM Task WHERE taskId = ?;");
	cmd.bind(1, task->getTaskId());
	cmd.executenonquery();
	
	delete task;
}

Task* TaskPersistence::getTask() const {
	return task;
}

void TaskPersistence::setTask(Task* task) {
	this->task = task;
}

void TaskPersistence::setDescription(const std::string description) {
	setColumn<std::string>("description", description);
}

void TaskPersistence::setLongDescription(const std::string longDescription) {
	setColumn<std::string>("longDescription", longDescription);
}

void TaskPersistence::addTag(id_t tagId) {
	// if(!conn || !task) { TODO: throw }
	sqlite3_command cmd(*conn);
	cmd.prepare("SELECT count(*) FROM Tagged WHERE (taskId = ? AND tagId = ?);");
	cmd.bind(1, task->getTaskId());
	cmd.bind(2, tagId);
	int count = cmd.executeint();
	if (count <= 0) {
		cmd.prepare("INSERT INTO Tagged (taskId,tagId) VALUES (?,?);");
		cmd.bind(1, task->getTaskId());
		cmd.bind(2, tagId);
		cmd.executenonquery();
	}
}

void TaskPersistence::removeTag(id_t tagId) {
	// if(!conn || !task) { TODO: throw }
	// check if the task and the tag exist in the database
	sqlite3_command cmd(*conn);
	cmd.prepare("DELETE FROM Tagged WHERE (taskId = ? AND tagId = ?);");
	cmd.bind(1, task-> getTaskId());
	cmd.bind(2, tagId);
	cmd.executenonquery();
}

void TaskPersistence::addSubtask(id_t taskId) {
	// if(!conn || !task) { TODO: throw }
	sqlite3_command cmd(*conn);
	cmd.prepare("SELECT count(*) FROM Subtasks WHERE (sub_taskId = ? AND super_taskId = ?);");
	cmd.bind(1, taskId);
	cmd.bind(2, task-> getTaskId());
	int count = cmd.executeint();
	if (count <= 0) {
		cmd.prepare("INSERT INTO Subtasks (sub_taskId, super_taskId) VALUES (?,?);");
		cmd.bind(1, taskId);
		cmd.bind(2, task-> getTaskId());
		cmd.executenonquery();
	}
}

void TaskPersistence::removeSubtask(id_t taskId) {
	// if(!conn || !task) { TODO: throw }
	sqlite3_command cmd(*conn);
	cmd.prepare("DELETE FROM Subtask WHERE (sub_taskId = ? AND super_taskId = ?);");
	cmd.bind(1, taskId);
	cmd.bind(2, task-> getTaskId());
	cmd.executenonquery();
}

void TaskPersistence::setDateCreated(const DateTime& dateCreated) {
	// check if dateCreated is ok
	setColumn<std::string>("dateCreated", dateCreated.toString());
}

void TaskPersistence::setDateLastModified(const DateTime& dateLastModified) {
	// check if dateLastModified is ok
	setColumn<std::string>("dateLastModified", dateLastModified.toString());
}
void TaskPersistence::setDateStarted(const Date& dateStarted) {
	// check if dateStarted is ok
	setColumn<std::string>("dateStarted", dateStarted.toString());
}
void TaskPersistence::setDateDeadline(const Date& dateDeadline) {
	// check if dateDeadline is ok
	setColumn<std::string>("dateDeadline", dateDeadline.toString());
}
void TaskPersistence::setDateCompleted(const Date& dateCompleted) {
	// check if dateCompleted is ok
	setColumn<std::string>("dateCompleted", dateCompleted.toString());
}

void TaskPersistence::setRecurrence(const Recurrence& recurrence) {
	setColumn<std::string>("recurrence", Recurrence::toString(recurrence));
}


void TaskPersistence::setPriority(int priority) {
	// check if priority is ok
	setColumn<int>("priority", priority);
}
 
void TaskPersistence::setCompletedPercentage(int completedPercentage) {
	// check if completedPercentage is ok (in interval [0;100])
	setColumn<int>("completedPercentage", completedPercentage);
}
void TaskPersistence::setDone() {
	// TODO: put this constant into config
	setCompletedPercentage(100);
}

} // namespace getodo
