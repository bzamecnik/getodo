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
	taskId(INVALID_ID),
	parentId(INVALID_ID),
	priority(0),
	completedPercentage(0),
	done(false)
{
	using namespace boost;
	description = std::string();
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
	parentId(t.parentId),
	description(t.description),
	longDescription(t.longDescription),
	tags(t.tags), // TODO: ok?
	// Don't allow to make circles
	// * OR: copy the whole subtree
	// When task is copyied to a brand new one, delete subtasks explicitly!
	subtasks(t.subtasks),
	dateCreated(t.dateCreated),
	dateLastModified(t.dateLastModified),
	dateStarted(t.dateStarted),
	dateDeadline(t.dateDeadline),
	dateCompleted(t.dateCompleted),
	priority(t.priority),
	completedPercentage(t.completedPercentage),
	done(t.done)
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

bool Task::isValidId(id_t id) {
	return id >= 0;
}

// ----- access methods ----------

id_t Task::getTaskId() const { return taskId; }
void Task::setTaskId(id_t taskId) { this->taskId = taskId; }
bool Task::hasValidId() const { return Task::isValidId(taskId); }

id_t Task::getParentId() const { return parentId; }
//void Task::setParentId(id_t parentId) { this->parentId = parentId; }

void Task::setParent(id_t newParentId, TaskManager& manager) {
	if ((newParentId != parentId) && (newParentId != taskId)) {
		// unset previous parent
		if (hasParent()) {
			Task* parentTask = manager.getTask(parentId);
			if (parentTask) {
				parentTask->removeSubtask(taskId);
			}
		}
		Task* newParentTask = manager.getTask(newParentId);
		id_t oldParentId = parentId;
		if (newParentTask) {
			newParentTask->addSubtask(taskId);
			parentId = newParentId;
		} else {
			parentId = Task::INVALID_ID;
		}
		TaskPersistence persistence(manager.getConnection(), this);
		persistence.setParentId();
		// problem: we know this task id only after it is persisted!
		manager.signal_task_moved(*this, oldParentId);
	}
}

void Task::unsetParent(TaskManager& manager) {
	setParent(Task::INVALID_ID, manager);
}

bool Task::hasParent() const {
	return parentId >= 0; // parentId is a valid task id
}

std::string Task::getDescription() const { return description; }
void Task::setDescription(const std::string& description) {
	this->description = description;
}

std::string Task::getLongDescription() const { return longDescription; }
void Task::setLongDescription(const std::string& longDescription) {
	this->longDescription = longDescription;
}

void Task::addTag(id_t tagId) {
	tags.insert(tagId);
}
bool Task::hasTag(id_t tagId) const {
	return (tags.find(tagId) != tags.end());
}
void Task::removeTag(id_t tagId) {
	// tags.erase() returns number of elements erased.
	// when the tagId is bad, it might be reported
	tags.erase(tagId);
}
std::vector<id_t>& Task::getTagIds() const {
	return convertSetToVector<id_t>(tags);
}

std::string Task::getTagsAsString(TaskManager& manager) const {
	// eg.: "hello, world, three word tag, other tag"
	std::vector<std::string> tagNames;
	std::set<id_t>::const_iterator it;
	for (it = tags.begin(); it != tags.end(); ++it) {
		Tag& tag = manager.getTag(*it);
		if (!tag.name.empty()) {
			tagNames.push_back(tag.name);
		}
	}
	std::sort(tagNames.begin(), tagNames.end());
	return boost::algorithm::join(tagNames, ", "); // or join with ';'
}

void Task::setTagsFromString(TaskManager& manager, const std::string& tagsString) {
	// synchronize the tags (from string to Task and TaskManager)

	using namespace boost::algorithm;
	std::vector<std::string> newTags;
	// split the string with separator "," and discard empty parts
	split(newTags, tagsString, is_any_of(","), token_compress_on);
	// trim the parts
	for (std::vector<std::string>::iterator it = newTags.begin();
		it != newTags.end(); ++it)
	{
		trim(*it);
		// possibly remove empty elements
	}
	
	// synchronize the tags
	std::set<id_t> oldTagIds;
	std::copy(tags.begin(), tags.end(), std::inserter(oldTagIds, oldTagIds.begin()));
	
	for (std::vector<std::string>::iterator it = newTags.begin();
		it != newTags.end(); ++it)
	{
		if (it->empty()) {
			continue; // ingnore empty tag names
		}

		if (manager.hasTag(*it)) {
			Tag& tag = manager.getTag(*it);
			if (hasTag(tag.id)) {
				// the task already has this tag
				oldTagIds.erase(tag.id);
			} else {
				// a newly added tag (but it is already in TaskManager)
				addTag(tag.id);
			}
		} else {
			// a newly added tag (even to TaskManager)
			id_t newTagId = manager.addTag(Tag(*it));
			addTag(newTagId);
		}
	}

	// delete old tags that were not in the new tags list
	// but not from TaskManger (they might be used somewhere else)
	for (std::set<id_t>::iterator it = oldTagIds.begin();
		 it != oldTagIds.end(); ++it)
	{	
		removeTag(*it);
	}
}

void Task::addSubtask(id_t subtaskId) {
	if (!Task::isValidId(subtaskId) || (subtaskId == taskId)) {
		throw new GetodoError("Invalid subtask id.");
	}
	subtasks.insert(subtaskId);
}
bool Task::hasSubtask(id_t subtaskId) const {
	return (subtasks.find(subtaskId) != subtasks.end());
}
void Task::removeSubtask(id_t taskId) {
	//should throw an exception on failure (?)
	subtasks.erase(taskId);
}
std::vector<id_t>& Task::getSubtaskIds() const {
	return convertSetToVector<id_t>(subtasks);
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
		return *(new RecurrenceOnce()); // default
	} else {
		return *recurrence;
	}
}
void Task::setRecurrence(Recurrence* r) {
	if (r) {
		delete recurrence;
		recurrence = r;
	} else {
		r = new RecurrenceOnce();
	}
}

int Task::getPriority() const { return priority; }
void Task::setPriority(int priority) { this->priority = priority; }

int Task::getCompletedPercentage() const { return completedPercentage; }
void Task::setCompletedPercentage(int completedPercentage) {
	this->completedPercentage = completedPercentage;
}

bool Task::isDone() { return done; }
void Task::setDone(bool done) {
	this->done = done;
	setCompletedPercentage(100);  // TODO: ugly constant here
}

// ----- object-relation representation conversion ----------

databaseRow_t& Task::toDatabaseRow() const {
	databaseRow_t& row = *(new databaseRow_t());

	row["taskId"] = boost::lexical_cast<std::string, id_t>(taskId);
	row["parentId"] = boost::lexical_cast<std::string, id_t>(parentId);

	row["description"] = sqliteEscapeString(description);
	row["longDescription"] = sqliteEscapeString(longDescription);

	row["dateCreated"] = dateCreated.toString();
	row["dateLastModified"] = dateLastModified.toString();
	row["dateStarted"] = dateStarted.toString();
	row["dateDeadline"] = dateDeadline.toString();
	row["dateCompleted"] = dateCompleted.toString();

	// row["estDuration"] = estDuration;
	row["recurrence"] = Recurrence::toString(*recurrence);

	row["priority"] = boost::lexical_cast<std::string, int>(priority);
	row["completedPercentage"] = boost::lexical_cast<std::string, int>(completedPercentage);
	row["done"] = boost::lexical_cast<std::string, bool>(done);

	return row;
}

Task* Task::fromDatabaseRow(databaseRow_t& row) {
	Task* task = new Task();
	
	try {
		task->taskId = boost::lexical_cast<id_t, std::string>(row["taskId"]);
	} catch (boost::bad_lexical_cast&) {
		task->taskId = Task::INVALID_ID;
	}

	try {
		task->parentId = boost::lexical_cast<id_t, std::string>(row["parentId"]);
	} catch (boost::bad_lexical_cast&) {
		task->parentId = Task::INVALID_ID;
	}
	
	task->description = sqliteUnescapeString(row["description"]);
	task->longDescription = sqliteUnescapeString(row["longDescription"]);
	
	task->dateCreated = DateTime(row["dateCreated"]);
	task->dateLastModified = DateTime(row["dateLastModified"]);
	task->dateStarted = Date(row["dateStarted"]);
	task->dateDeadline = Date(row["dateDeadline"]);
	task->dateCompleted = Date(row["dateCompleted"]);
	
	// estDuration = Duration(row["estDuration"]);
	task->recurrence = Recurrence::fromString(row["recurrence"]);
	
	try {
		task->priority = boost::lexical_cast<int, std::string>(row["priority"]);
	} catch (boost::bad_lexical_cast&) {
		task->priority = 0; // TODO: default priority
	}

	try {
		task->completedPercentage = boost::lexical_cast<int, std::string>(row["completedPercentage"]);
	} catch (boost::bad_lexical_cast&) {
		task->completedPercentage = 0; // TODO: default completedPercentage
	}

	try {
		task->done =  boost::lexical_cast<bool, std::string>(row["done"]);
	} catch (boost::bad_lexical_cast&) {
		task->done = false;
	}
	
	return task;
}

std::string Task::toString() const {
	return boost::lexical_cast<std::string, Task>(*this);
}

std::ostream& operator<< (std::ostream& o, const Task& task) {
	o << "Task [" << std::endl;
	databaseRow_t row = task.toDatabaseRow();
	for (databaseRow_t::iterator it = row.begin(); it != row.end(); ++it) {
		o << "  " << it->first << " => " << it->second << std::endl;
	}

	std::vector<id_t>& tags = task.getTagIds();
	o << "  tags [";
	join(o, tags.begin(), tags.end(), ",");
	o << "]" << std::endl;
	
	//std::vector<id_t> subtasks = task.getSubtaskIds();
	//o << "  subtasks [";
	//join(o, subtasks.begin(), subtasks.end(), ",");
	//o << "]" << std::endl;

	o << "]" << std::endl;
	return o;
}

// ----- class TaskPersistence --------------------

// Constructor for loading new Tasks
TaskPersistence::TaskPersistence(sqlite3_connection* c)
	: conn(c), task(0) {}

// Constructor for modifying particular things in a Task
TaskPersistence::TaskPersistence(sqlite3_connection* c, Task* t)
	: conn(c), task(t) {}

TaskPersistence::~TaskPersistence() {}

bool TaskPersistence::insert() {
	databaseRow_t& row = prepareRowToSave();

	int count = 0;
	if (task->hasValidId()) {
		// task has already its taskId, find out if it is in the db
		sqlite3_command cmd(*conn, "SELECT count(*) FROM Task "
			"WHERE taskId = (?);");
		cmd.bind(1, row["taskId"]);
		count = cmd.executeint();
		if (count > 0) {
			return false; // it is already in the db
		}
	}
	row["dateCreated"] = row["dateLastModified"];
	row.erase("taskId"); // don't set id directly, db will set it

	std::ostringstream sql;
	std::ostringstream values;
	sql << "INSERT INTO Task (";
	databaseRow_t::const_iterator col = row.begin();
	for (; col != (--(row.end())); col++) {
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
	cmd.executenonquery();
	
	// the database automatically created a new taskId
	// set it to the task object
	task->setTaskId(sqlite3_last_insert_rowid(conn->db()));
	return true;
}

void TaskPersistence::update() {
	databaseRow_t& row = prepareRowToSave();

	// NOTE: if there is no row with such an id in the db,
	// nothing bad happens

	// build the SQL command
	std::ostringstream ss;
	ss << "UPDATE Task SET ";
	databaseRow_t::const_iterator col = row.begin();
	for (; col != (--row.end()); col++) {
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
	cmd.executenonquery();
}

databaseRow_t& TaskPersistence::prepareRowToSave() {
	if (!conn) { throw new GetodoError("No database connection in the persistence."); }
	if (!task) { throw new GetodoError("No task in the persistence."); }

	databaseRow_t& row = task->toDatabaseRow();
	// TODO: delete this when Duration will be ready
	row["estDuration"] = "";

	// update last modification date
	task->setDateLastModified(DateTime::now());
	row["dateLastModified"] = task->getDateLastModified().toString();
	return row;
}

Task* TaskPersistence::load(id_t taskId) {
	if (!conn) { throw new GetodoError("No database connection in the persistence."); }
	
	// Load task itself
	
	sqlite3_command cmd(*conn, "SELECT * FROM Task WHERE taskId = (?);");
	cmd.bind(1, taskId);
	sqlite3_cursor cursor = cmd.executecursor();
	if (!cursor.step()) {
		throw new GetodoError("No such a task to load: " + taskId);
		return 0;
	}
	databaseRow_t row;
	int columnsCount = cursor.colcount();
	for (int i = 0; i < columnsCount; i++) {
		std::string columnData;
		if (!cursor.isnull(i)) {
			columnData = cursor.getstring(i);
		}
		row[cursor.getcolname(i)] = columnData;
	}
	cursor.close();
	task = Task::fromDatabaseRow(row);
	if(!task) {
		throw new GetodoError("Can't deserialize the task.");
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
	cmd.prepare("SELECT taskId FROM Task WHERE parentId = ?;");
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
	if (!conn) { throw new GetodoError("No database connection in the persistence."); }
	if (!task) { throw new GetodoError("No task in the persistence."); }
	if (!task->hasValidId()) {
		throw new std::invalid_argument("Invalid task id: " + task->getTaskId());
	}
	
	sqlite3_command cmd(*conn, "DELETE FROM Tagged WHERE taskId = ?;");
	cmd.bind(1, task->getTaskId());
	cmd.executenonquery();
	
	// TODO: Think out of what about subtasks?
	// * delete them too? (remove the whole subtree)
	// - make them level 1 tasks?
	// - connect them one level up?

	// assume that all children were already removed

	//cmd.prepare("UPDATE Task SET parentId = ? WHERE parentId = ?;");
	//if (Task::isValidId(task->getParentId())) {
	//	cmd.bind(1, task->getParentId());
	//} else {
	//	cmd.bind(1, Task::INVALID_ID); // make it a root level task
	//}
	//cmd.bind(2, task->getTaskId());
	//cmd.executenonquery();
	
	cmd.prepare("DELETE FROM Task WHERE taskId = ?;");
	cmd.bind(1, task->getTaskId());
	cmd.executenonquery();
	
	delete task;
	task = 0;
}

Task* TaskPersistence::getTask() const {
	return task;
}

void TaskPersistence::setTask(Task* task) {
	this->task = task;
}

void TaskPersistence::setDescription(const std::string& description) {
	if (!task) { return; }
	if (task->getDescription().compare(description) == 0) { return; } // no change
	setColumn<std::string>("description", description);
	task->setDescription(description);
}

void TaskPersistence::setLongDescription(const std::string& longDescription) {
	if (!task) { return; }
	if (task->getLongDescription().compare(longDescription) == 0) { return; } // no change
	setColumn<std::string>("longDescription", longDescription);
	task->setLongDescription(longDescription);
}

void TaskPersistence::addTag(id_t tagId) {
	if (!conn) { throw new GetodoError("No database connection in the persistence."); }
	if (!task) { throw new GetodoError("No task in the persistence."); }
	if (task->hasTag(tagId)) { return; } // task already has this tag

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
	task->addTag(tagId);
}

void TaskPersistence::removeTag(id_t tagId) {
	if (!conn) { throw new GetodoError("No database connection in the persistence."); }
	if (!task) { throw new GetodoError("No task in the persistence."); }
	// TODO: check this out
	//if (!task->hasTag(tagId) { return; } // nothing to remove

	// check if the task and the tag exist in the database
	sqlite3_command cmd(*conn);
	cmd.prepare("DELETE FROM Tagged WHERE (taskId = ? AND tagId = ?);");
	cmd.bind(1, task-> getTaskId());
	cmd.bind(2, tagId);
	cmd.executenonquery();
	task->removeTag(tagId);
}

void TaskPersistence::setParentId() {
	if (!conn) { throw new GetodoError("No database connection in the persistence."); }
	if (!task) { throw new GetodoError("No task in the persistence."); }

	sqlite3_command cmd(*conn);
	cmd.prepare("UPDATE Task SET parentId = ? WHERE taskId = ?;");
	cmd.bind(1, task->getParentId());
	cmd.bind(2, task->getTaskId());
	cmd.executenonquery();
	// TODO: task->setParent() - we don't have reference to the new parent
}

void TaskPersistence::setDateCreated(const DateTime& dateCreated) {
	if (!task) { return; }
	// NOTE: this is a poor man's substitute for operator==() on DateTime
	if (task->getDateCreated().toString().compare(
		dateCreated.toString()) == 0) { return; } // no change
	// check if dateCreated is ok
	setColumn<std::string>("dateCreated", dateCreated.toString());
	task->setDateCreated(dateCreated);
}

void TaskPersistence::setDateLastModified(const DateTime& dateLastModified) {
	if (!task) { return; }
	// NOTE: this is a poor man's substitute for operator==() on DateTime
	if (task->getDateLastModified().toString().compare(
		dateLastModified.toString()) == 0) { return; } // no change
	// check if dateLastModified is ok
	setColumn<std::string>("dateLastModified", dateLastModified.toString());
	task->setDateLastModified(dateLastModified);
}
void TaskPersistence::setDateStarted(const Date& dateStarted) {
	if (!task) { return; }
	// NOTE: this is a poor man's substitute for operator==() on Date
	if (task->getDateStarted().toString().compare(
		dateStarted.toString()) == 0) { return; } // no change
	// check if dateStarted is ok
	setColumn<std::string>("dateStarted", dateStarted.toString());
	task->setDateStarted(dateStarted);
}
void TaskPersistence::setDateDeadline(const Date& dateDeadline) {
	if (!task) { return; }
	// NOTE: this is a poor man's substitute for operator==() on Date
	if (task->getDateDeadline().toString().compare(
		dateDeadline.toString()) == 0) { return; } // no change
	// check if dateDeadline is ok
	setColumn<std::string>("dateDeadline", dateDeadline.toString());
	task->setDateDeadline(dateDeadline);
}
void TaskPersistence::setDateCompleted(const Date& dateCompleted) {
	if (!task) { return; }
	// NOTE: this is a poor man's substitute for operator==() on Date
	if (task->getDateCompleted().toString().compare(
		dateCompleted.toString()) == 0) { return; } // no change
	// check if dateCompleted is ok
	setColumn<std::string>("dateCompleted", dateCompleted.toString());
	task->setDateCompleted(dateCompleted);
}

void TaskPersistence::setRecurrence(const Recurrence& recurrence) {
	if (!task) { return; }
	std::string newRecurrenceStr = Recurrence::toString(recurrence);
	// NOTE: this is a poor man's substitute for operator==() on Recurrence
	if (Recurrence::toString(task->getRecurrence()).compare(
		newRecurrenceStr) == 0) { return; } // no change
	setColumn<std::string>("recurrence", newRecurrenceStr);
	task->setRecurrence(recurrence.clone());
}

void TaskPersistence::setPriority(int priority) {
	if (!task) { return; }
	if (task->getPriority() == priority) { return; } // no change
	// check if priority is ok
	setColumn<int>("priority", priority);
	task->setPriority(priority);
}
 
void TaskPersistence::setCompletedPercentage(int completedPercentage) {
	if (!task) { return; }
	if (task->getCompletedPercentage() == completedPercentage) { return; } // no change
	// check if completedPercentage is ok (in interval [0;100])
	setColumn<int>("completedPercentage", completedPercentage);
	task->setCompletedPercentage(completedPercentage);
}

void TaskPersistence::setDone(bool done) {
	if (!task) { return; }
	if (task->isDone() == done) { return; } // no change
	setColumn<bool>("done", done);
	setCompletedPercentage(task->getCompletedPercentage());
	task->setDone();
}

void TaskPersistence::saveTags() {
	if (!task) { throw new GetodoError("No task in the persistence."); }
	
	// TODO: remove unused tags (ie. the ones deleted in Task but not yet in db)
	std::vector<id_t>& tags = task->getTagIds();
	std::vector<id_t>::const_iterator tag;
	for (tag = tags.begin(); tag != tags.end(); tag++) {
		addTag(*tag);
	}
}

} // namespace getodo