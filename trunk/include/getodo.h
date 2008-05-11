// $ID $

typedef int id_t;

class Task {
	id_t taskId;
	string description;
	string longDescription; // including links, attachments, etc.

	std::list<Tag&> tags;
	std::list<Task&> subtasks;

	Date dateCreated;
	Date dateLastModified;
	Date& dateStarted;
	FuzzyDate& dateDeadline;
	Date& dateCompleted;
	Duration& estDuration; // estimated duration
	Recurrence recurrence;
	
	int priority; // TODO: number or symbol?
	int completedPercentage;

public:
	Task();
	//{
	//	taskId = 0; // not yet set
	//}
	Task(Task& t); // copy constructor
	virtual ~Task(); // delete tags, subtasks
	// setters and getter for some properties

	// convert representation: database <-> object
	static std::map<string,string> toDatabaseRow(Task& task);
	std::map<string,string> toDatabaseRow();
	static Task& fromDatabaseRow(std::map<string,string>);
}

class Tag {
	id_t tagId;
	string tagName;
}

class Date {
	/*
	Date should be able to express:
		* date + time exactly given
		* date alone
		* no information		
	*/
}

class FuzzyDate {
	/*
	Date should be able to express:
		* date + time exactly given
		* date alone
		* indefinite date
		* no information			
	*/
}

class Recurrence {
	/*
	Recurrence should be:
		*  once - no recurrence - default
		*  every Nth day
		*  every Nth week - given selected weekdays
		*  every Nth month - given day in month
		*  every Nth year - given day and month
		*  every day in an interval between two days		 		
	*/		
}

class Duration {
	/*
		Duration should express:
			* units of given magnitude (minutes, hours, days, weeks, months, years)
			* - lesser magnitude => greater precision			
			* or no information					
	*/
}

class FilterRule {
	id_t filterRuleId;
	string name;
	string rule;
}

class DatabaseConnection {
	// connection to a SQLite file
}

class TaskManager {
	std::map<id_t,Task&> tasks;
	std::map<id_t,Tag&> tags;
	std::list<FilterRule&> filters;

	DatabaseConnection& db;

public:
	//TaskManager();
	TaskManager(DatabaseConnection& db);
	//TaskManager(TaskManager& tm);
	~TaskManager();

	Task& addTask(Task& task);
	Task& getTask(id_t taskId);
	Task& editTask(id_t taskId, Task& task);
	bool deleteTask(id_t taskId);

	Tag& addTag(Tag& tag);
	Tag& editTag(id_t tagId, Tag& tag);
	bool deleteTag(id_t tagId);

	assignTag(id_t tagId, id_t taskId);
	removeTag(id_t tagId, id_t taskId);

	FilterRule& addFilterRule(FilterRule& filter);
	FilterRule& editFilterRule(id_t filterRuleId, FilterRule& filter);
	bool deleteFilterRule(id_t filterRuleId);

	std::map<id_t,Task> filterTask(id_t filterRuleId);
	std::map<id_t,Task> filterTask(FilterRule& filter);

	private loadFromDatabase();
	loadFromDatabase(FilterRule& filter);
}
