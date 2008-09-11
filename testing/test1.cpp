#include "test1.h"

using namespace getodo;

// TIP: use Boost.Test http://www.boost.org/doc/libs/1_35_0/libs/test/doc/index.html
// TODO: avoid duplicating code!

// ----- tests --------------------

void testDateTime() {
	using namespace std;
	
	cout << "----- DateTime -----" << endl;

	DateTime dt1;
	cout << "DateTime dt1: " << dt1.toString() << endl;
	DateTime dt2 = DateTime::now();
	cout << "DateTime dt2 now(), copy: " << dt2.toString() << endl;
	DateTime dt3 = DateTime(dt2.toString());
	cout << "DateTime dt3 from string: " << dt3.toString() << endl;
}

void testDate() {
	using namespace std;

	cout << "----- Date -----" << endl;

	Date date1;
	cout << "Date date1: " << date1.toString() << endl;
	Date date2 = Date::now();
	cout << "Date date2 now(), copy: " << date2.toString() << endl;
	Date date3 = Date(date2.toString());
	cout << "Date date3 from string: " << date3.toString() << endl;
}

void testRecurrence() {
	using namespace std;

	cout << "----- Recurrence -----" << endl;
	
	Recurrence* r1 = Recurrence::fromString("");
	cout << "r1 Recurrence::fromString(\"\")" << endl;
	cout << typeid(*r1).name() << " r1: " << r1->toString() << endl;

	Recurrence* r2 = Recurrence::fromString("w   2 Tue		Sat Fri  Fri ");
	cout << "r2 Recurrence::fromString(\"  2 Tue		Sat Fri  Fri \")" << endl;
	cout << typeid(*r2).name() << " r2: " << r2->toString() << endl;


}

template<typename T>
void testRecurrenceType(std::vector<std::string> parameters) {
	using namespace std;

	cout << "----- " << typeid(T).name() << " -----" << endl;
	
	for(vector<string>::iterator it = parameters.begin();
		it != parameters.end(); ++it) {
		T r(*it);
		cout << "\"" << *it << "\"" << endl;
		cout << "  toString(): \"" << r.toString() << "\"" << endl;
		cout << "  next(Date::now(): " << r.next(Date::now()).toString() << endl;
	}
}
void testRecurrenceOnce() {
	using namespace std;

	cout << "----- RecurrenceOnce -----" << endl;
	
	RecurrenceOnce r1;
	cout << "r1.toString(): " << r1.toString() << endl;
	cout << "r1.next(Date::now()): " << r1.next(Date::now()).toString() << endl;
}

void testRecurrenceDaily() {
	using namespace std;
	string params[2] = {"", "5"};
	vector<string> parameters = vector<string>(&params[0], &params[2]);
	testRecurrenceType<RecurrenceDaily>(parameters);
}

void testRecurrenceWeekly() {
	using namespace std;
	string params[2] = {"", "  2 Tue		Sat Fri  Fri "};
	vector<string> parameters = vector<string>(&params[0], &params[2]);
	testRecurrenceType<RecurrenceWeekly>(parameters);
}

void testRecurrenceMonthly() {
	using namespace std;
	string params[4] = {"", "5", "5 14", "5 40"};
	vector<string> parameters = vector<string>(&params[0], &params[4]);
	testRecurrenceType<RecurrenceMonthly>(parameters);
}

void testRecurrenceYearly() {
	using namespace std;
	//string params[4] = {"", "29 Feb", "30 Feb", "14 "};
	//// "14" causes Debug assertion in Debug mode, but the following
	//// is handled correctly
	string params[4] = {"", "29 Feb", "30 Feb"};
	vector<string> parameters = vector<string>(&params[0], &params[4]);
	testRecurrenceType<RecurrenceYearly>(parameters);
}

void testRecurrenceIntervalDays() {
	using namespace std;
	string params[2] = {"", "[2008-Aug-18/2008-Sep-04]"};
	vector<string> parameters = vector<string>(&params[0], &params[2]);
	testRecurrenceType<RecurrenceIntervalDays>(parameters);
}

void testTag() {
	using namespace std;
	cout << "----- Tag -----" << endl;

	Tag tag1;
	Tag tag2("photos");
	Tag tag3(tag2);
	Tag tag4(123, "photos");
	cout << "tag1: " << tag1.toString() << endl;
	cout << "tag2: " << tag2.toString() << endl;
	cout << "tag3: " << tag3.toString() << endl;
	cout << "tag4: " << tag4.toString() << endl;
	
	cout << "----- TagPersistence -----" << endl;

	try {
		TaskManager tm("test1.db");
		sqlite3_connection *conn = tm.getConnection();
		cout << "TaskManager created" << endl;
	
		TagPersistence tagp(conn);
		
		// insert()
		tagp.insert(tag2);
		cout << "tag2 saved: " << tag2.toString() << endl;
		
		// TODO: update()

		// load()
		Tag tag5 = tagp.load(tag2.id);
		cout << "tag5 loaded from tag2: " << tag5.toString() << endl;
		tag5 = tagp.load(42); // load non-existent tagId
		cout << "tag5 load non-existent: " << tag5.toString() << endl;

		// erase()
		tagp.erase(tag2.id);
		cout << "tag2 erased" << endl;
		tag5 = tagp.load(tag2.id);
		cout << "tag5 loading erased tag: " << tag5.toString() << endl;

		//tag1 = tm.addTag(tag1);
		//cout << "tm.addTag: " << tag1.tagId << ", " << tag1.tagName << endl;
		//cout << "tm.hasTag: " << tm.hasTag(tag1.tagId) << endl;

		conn->close();
	} catch (database_error e) {
		cout << e.what() << endl;
	}
}

void testFilterRule() {
	using namespace std;

	cout << "----- FilterRule -----" << endl;

	FilterRule rule1;
	FilterRule rule2("rule name","filter rule");
	FilterRule rule3(rule2);
	FilterRule rule4(123, "other rule name"," other filter rule");
	cout << "rule1: " << rule1.toString() << endl;
	cout << "rule2: " << rule2.toString() << endl;
	cout << "rule3: " << rule3.toString() << endl;
	cout << "rule4: " << rule4.toString() << endl;
	
	cout << "----- FilterRulePersistence -----" << endl;

	try {
		TaskManager tm("test1.db");
		sqlite3_connection *conn = tm.getConnection();
		cout << "TaskManager created" << endl;

		FilterRulePersistence rulep(conn);
		
		// save()
		rulep.save(rule2);
		cout << "rule2 saved: " << rule2.toString() << endl;
		
		// load()
		FilterRule rule5;
		rulep.load(rule5, rule2.id);
		cout << "rule5 loaded from rule2: " << rule5.toString() << endl;
		rulep.load(rule5, 42); // load non-existent ruleId
		cout << "rule5 load non-existent: " << rule5.toString() << endl;

		// setName()
		rulep.setName(rule2, "modified rule name");
		cout << "rule2 setName: " << rule2.toString() << endl;

		// setRule()
		rulep.setRule(rule2, "modified filter rule");
		cout << "rule2 setRule: " << rule2.toString() << endl;

		// erase()
		rulep.erase(rule2.id);
		cout << "rule2 erased" << endl;
		rulep.load(rule5, rule2.id);
		cout << "rule5 loading erased rule: " << rule5.toString() << endl;

		conn->close();
	} catch (database_error e) {
		cout << e.what() << endl;
	}
}

void testTask() {
	using namespace std;

	cout << "----- Task -----" << endl;

	// constructor
	Task task1;
	
	//id_t getTaskId() const;
	cout << "task1 getTaskId(): " << boost::lexical_cast<string,id_t>(task1.getTaskId()) << endl;
	//void setTaskId(id_t taskId);
	task1.setTaskId(42);
	cout << "task1 setTaskId(42): " << boost::lexical_cast<string,id_t>(task1.getTaskId()) << endl;

	//std::string getDescription() const;
	cout << "task1 getDescription(): " << task1.getDescription() << endl;
	//void setDescription(const std::string description);
	task1.setDescription("Titulek ukolu");
	cout << "task1 setDescription(): " << task1.getDescription() << endl;

	//std::string getLongDescription() const;
	cout << "task1 getLongDescription(): " << task1.getLongDescription() << endl;
	//void setLongDescription(const std::string longDescription);
	task1.setLongDescription("Detaily ukolu");
	cout << "task1 setLongDescription(): " << task1.getLongDescription() << endl;

	//void addTag(id_t tagId);
	cout << "task1 addTag(17)" << endl;
	task1.addTag(17);
	
	//bool hasTag(id_t tagId) const;
	cout << "task1 hasTag(17): " << task1.hasTag(17) << endl;
	cout << "task1 hasTag(18): " << task1.hasTag(18) << endl;
	
	//void removeTag(id_t tagId);
	cout << "task1 removeTag(17)" << endl;
	task1.removeTag(17);
	cout << "task1 hasTag(17): " << task1.hasTag(17) << endl;
	
	//std::list<id_t> getTagsList() const;
	{
		cout << "task1 getTagsList(): [";
		list<id_t> tags = task1.getTagsList();
		for(list<id_t>::iterator it = tags.begin(); it != tags.end(); ++it) {
			cout << *it << ", ";
		}
		cout << "]" << endl;
	}
	cout << "task1 addTag 19, 20, 20, 21" << endl;
	task1.addTag(19);
	task1.addTag(20);
	task1.addTag(20);
	task1.addTag(21);
	{
		cout << "task1 getTagsList(): [";
		list<id_t> tags = task1.getTagsList();
		for(list<id_t>::iterator it = tags.begin(); it != tags.end(); ++it) {
			cout << *it << ", ";
		}
		cout << "]" << endl;
	}

	//void addSubtask(id_t SubtaskId);
	cout << "task1 addSubtask(117)" << endl;
	task1.addSubtask(117);
	
	//bool hasSubtask(id_t SubtaskId) const;
	cout << "task1 hasSubtask(117): " << task1.hasSubtask(117) << endl;
	cout << "task1 hasSubtask(118): " << task1.hasSubtask(118) << endl;
	
	//void removeSubtask(id_t SubtaskId);
	cout << "task1 removeSubtask(117)" << endl;
	task1.removeSubtask(117);
	cout << "task1 hasSubtask(117): " << task1.hasSubtask(117) << endl;
	
	//std::list<id_t> getSubtasksList() const;
	{
		cout << "task1 getSubtasksList(): [";
		list<id_t> subtasks = task1.getSubtasksList();
		for(list<id_t>::iterator it = subtasks.begin(); it != subtasks.end(); ++it) {
			cout << *it << ", ";
		}
		cout << "]" << endl;
	}
	cout << "task1 addSubtask 119, 120, 120, 121" << endl;
	task1.addSubtask(119);
	task1.addSubtask(120);
	task1.addSubtask(120);
	task1.addSubtask(121);
	{
		cout << "task1 getSubtasksList(): [";
		list<id_t> subtasks = task1.getSubtasksList();
		for(list<id_t>::iterator it = subtasks.begin(); it != subtasks.end(); ++it) {
			cout << *it << ", ";
		}
		cout << "]" << endl;
	}

	//DateTime getDateCreated() const;
	cout << "task1 getDateCreated(): " << task1.getDateCreated().toString() << endl;
	//void setDateCreated(const DateTime& dateCreated);
	task1.setDateCreated(DateTime(
		boost::posix_time::second_clock::local_time()
		+ boost::posix_time::minutes(5)));
	cout << "task1 setDateCreated(now + 5 minutes): " << task1.getDateCreated().toString() << endl;

	//DateTime getDateLastModified() const;
	cout << "task1 getDateLastModified(): " << task1.getDateLastModified().toString() << endl;
	//void setDateLastModified(const DateTime& dateLastModified);
	task1.setDateLastModified(DateTime(
		boost::posix_time::second_clock::local_time()
		+ boost::posix_time::minutes(10)));
	cout << "task1 getDateLastModified(now + 10 minutes): " << task1.getDateLastModified().toString() << endl;

	//Date getDateStarted() const;
	cout << "task1 getDateStarted(): " << task1.getDateStarted().toString() << endl;
	//void setDateStarted(const Date& dateStarted);
	task1.setDateStarted(Date(
		boost::gregorian::day_clock::local_day()
		+ boost::gregorian::days(5)));
	cout << "task1 setDateStarted(now + 5 days): " << task1.getDateStarted().toString() << endl;

	//Date getDateDeadline() const; // should be FuzzyDate
	cout << "task1 getDateDeadline(): " << task1.getDateDeadline().toString() << endl;
	//void setDateDeadline(const Date& dateDeadline);
	task1.setDateDeadline(Date(
		boost::gregorian::day_clock::local_day()
		+ boost::gregorian::days(10)));
	cout << "task1 setDateDeadline(now + 10 days): " << task1.getDateDeadline().toString() << endl;

	//Date getDateCompleted() const;
	cout << "task1 getDateCompleted(): " << task1.getDateCompleted().toString() << endl;
	//void setDateCompleted(const Date& dateCompleted);
	task1.setDateCompleted(Date(
		boost::gregorian::day_clock::local_day()
		+ boost::gregorian::days(15)));
	cout << "task1 setDateCompleted(now + 15 days): " << task1.getDateCompleted().toString() << endl;
	
	// Recurrence& getRecurrence() const;
	cout << "task1 getRecurrence(): \"" << task1.getRecurrence().toString() << "\"" << endl;
	
	// void setRecurrence(Recurrence* r);
	Recurrence* recurrenceWeekly = new RecurrenceWeekly("  2 Tue		Sat Fri  Fri ");
	task1.setRecurrence(recurrenceWeekly);
	cout << "task1 setRecurrence(\"" << recurrenceWeekly->toString() << "\")" << endl;
	cout << "task1 getRecurrence(): \"" << task1.getRecurrence().toString() << "\"" << endl;

	//int getPriority() const;
	cout << "task1 getPriority(): " << boost::lexical_cast<string,int>(task1.getPriority()) << endl;
	//void setPriority(int priority);
	task1.setPriority(7);
	cout << "task1 setPriority(7): " << boost::lexical_cast<string,int>(task1.getPriority()) << endl;

	//int getCompletedPercentage() const;
	cout << "task1 getCompletedPercentage(): " << boost::lexical_cast<string,int>(task1.getCompletedPercentage()) << endl;
	//void setCompletedPercentage(int completedPercentage);
	task1.setCompletedPercentage(51);
	cout << "task1 setCompletedPercentage(51): " << boost::lexical_cast<string,int>(task1.getCompletedPercentage()) << endl;
	//void setDone();
	task1.setDone();
	cout << "task1 setDone(): " << boost::lexical_cast<string,int>(task1.getCompletedPercentage()) << endl;

	// copy constructor
	Task task2(task1);
	cout << "task2 copy from task1" << endl;

	cout << "task2 getTaskId(): " << boost::lexical_cast<string,id_t>(task2.getTaskId()) << endl;
	cout << "task2 getDescription(): " << task2.getDescription() << endl;
	cout << "task2 getLongDescription(): " << task2.getLongDescription() << endl;
	{
		cout << "task2 getTagsList(): [";
		list<id_t> tags = task2.getTagsList();
		for(list<id_t>::iterator it = tags.begin(); it != tags.end(); ++it) {
			cout << *it << ", ";
		}
		cout << "]" << endl;
	}
	{
		cout << "task2 getSubtasksList(): [";
		list<id_t> subtasks = task2.getSubtasksList();
		for(list<id_t>::iterator it = subtasks.begin(); it != subtasks.end(); ++it) {
			cout << *it << ", ";
		}
		cout << "]" << endl;
	}
	cout << "task2 getDateCreated(): " << task2.getDateCreated().toString() << endl;
	cout << "task2 getDateLastModified(): " << task2.getDateLastModified().toString() << endl;
	cout << "task2 getDateStarted(): " << task2.getDateStarted().toString() << endl;
	cout << "task2 getDateDeadline(): " << task2.getDateDeadline().toString() << endl;
	cout << "task2 getDateCompleted(): " << task2.getDateCompleted().toString() << endl;
	cout << "task2 getPriority(): " << boost::lexical_cast<string,int>(task2.getPriority()) << endl;
	cout << "task2 getCompletedPercentage(): " << boost::lexical_cast<string,int>(task2.getCompletedPercentage()) << endl;

	//// ----- object-relation representation conversion ----------
	//static databaseRow_t toDatabaseRow(const Task& task);
	//databaseRow_t toDatabaseRow() const;
	databaseRow_t row1 = task1.toDatabaseRow();
	{
		cout << "row1 = task1.toDatabaseRow(): [" << endl;
		for(databaseRow_t::iterator it = row1.begin(); it != row1.end(); ++it) {
			cout << "  " << it->first << " => " << it->second << endl;
		}
		cout << "]" << endl;
	}
	//static Task* fromDatabaseRow(databaseRow_t);
	Task* task3 = Task::fromDatabaseRow(row1);
	cout << "task3 fromDatabaseRow(row1)" << endl;
	databaseRow_t row2 = task3->toDatabaseRow();
	{
		cout << "row2 = task3->toDatabaseRow(): [" << endl;
		for(databaseRow_t::iterator it = row1.begin(); it != row1.end(); ++it) {
			cout << "  " << it->first << " => " << it->second << endl;
		}
		cout << "]" << endl;
	}
}


void printTask(Task& task, std::ostream& os) {
	using namespace std;

	os << "Task [" << endl;
	databaseRow_t row = task.toDatabaseRow();
	for (databaseRow_t::iterator it = row.begin(); it != row.end(); ++it) {
		os << "  " << it->first << " => " << it->second << endl;
	}

	os << "  tags [";
	list<id_t> tags = task.getTagsList();
	join(std::cout, tags.begin(), tags.end(), ", ");
	os << "]" << endl;

	os << "  subtasks [";
	list<id_t> subtasks = task.getSubtasksList();
	join(std::cout, subtasks.begin(), subtasks.end(), ", ");
	os << "]" << endl;

	os << "]" << endl;
}

void testTaskPersistence() {
	using namespace std;
	using namespace sqlite3x;

	cout << "----- TaskPersistence -----" << endl;

	try {
		TaskManager manager("test1.db");

		cout << "creating TaskManager" << endl;
		sqlite3_connection *conn = manager.getConnection();

		cout << "creating TaskPersistence persistence1(conn)" << endl;
		//TaskPersistence(sqlite3x::sqlite3_connection* conn);
		TaskPersistence persistence1(conn);

		//TaskPersistence(sqlite3x::sqlite3_connection* conn, Task* task);
		//cout << "TaskPersistence taskp2(conn, task2) created" << endl;
		
		Task* task1 = new Task();
		task1->setTaskId(42);
		task1->setDescription("A title");
		task1->setLongDescription("Task details.");
		task1->addTag(17);
		task1->addTag(19);
		task1->addSubtask(117);
		task1->addSubtask(68);
		//// not needed, persistence will set this itself
		//task1->setDateCreated(DateTime(
		//	boost::posix_time::second_clock::local_time()
		//	+ boost::posix_time::minutes(5)));
		//task1->setDateLastModified(DateTime(
		//	boost::posix_time::second_clock::local_time()
		//	+ boost::posix_time::minutes(10)));
		task1->setDateStarted(Date(
			boost::gregorian::day_clock::local_day()
			+ boost::gregorian::days(5)));
		task1->setDateDeadline(Date(
			boost::gregorian::day_clock::local_day()
			+ boost::gregorian::days(10)));
		task1->setDateCompleted(Date(
			boost::gregorian::day_clock::local_day()
			+ boost::gregorian::days(15)));
		task1->setRecurrence(new RecurrenceWeekly("  2 Tue		Sat Fri  Fri "));
		task1->setPriority(7);
		task1->setCompletedPercentage(51);

		cout << "task1:" << endl;
		printTask(*task1, std::cout);
		
		//void setTask(Task* task);
		cout << "persistence1 setTask(task1)" << endl;
		persistence1.setTask(task1);

		//Task* getTask() const;
		cout << "persistence1 getTask(task1)" << endl;
		printTask(*(persistence1.getTask()), std::cout);
		
		//void save();
		cout << "persistence1 save()" << endl;
		persistence1.save();

		int task1Id = task1->getTaskId();

		//Task* load(id_t taskId);
		Task* task2 = persistence1.load(task1Id);
		cout << "persistence1 load(" << task1Id << ") -> task2" << endl;
		if (task2) {
			cout << "task2:" << endl;
			printTask(*task2, std::cout);
		}

		cout << "Setting individal properies:" << endl;

		//void setDescription(const std::string description);
		persistence1.setDescription("another description");
		cout << "persistence1.setDescription(\"another description\")" << endl;

		//void setLongDescription(const std::string longDescription);
		persistence1.setLongDescription("Another long description.");
		cout << "persistence1.setLongDescription(\"Another long description.\")" << endl;

		//void addTag(id_t tagId);
		persistence1.addTag(1234);
		cout << "persistence1.addTag(1234)" << endl;

		//void removeTag(id_t tagId);
		persistence1.removeTag(17);
		cout << "persistence1.removeTag(17)" << endl;

		//void addSubtask(id_t taskId);
		persistence1.addSubtask(5678);
		cout << "persistence1.addSubtask(5678)" << endl;

		//void removeSubtask(id_t taskId);
		persistence1.removeSubtask(117);
		cout << "persistence1.removeSubtask(117)" << endl;

		////void setDateCreated(const DateTime& dateCreated);
		////void setDateLastModified(const DateTime& dateLastModified);

		//void setDateStarted(const Date& dateStarted);
		persistence1.setDateStarted(Date(
			boost::gregorian::day_clock::local_day()
			+ boost::gregorian::days(20)));
		cout << "persistence1.setDateStarted(new + 20 days)" << endl;

		//void setDateDeadline(const Date& dateDeadline);
		persistence1.setDateDeadline(Date(
			boost::gregorian::day_clock::local_day()
			+ boost::gregorian::days(25)));
		cout << "persistence1.setDateDeadline(new + 25 days)" << endl;

		//void setDateCompleted(const Date& dateCompleted);
		persistence1.setDateCompleted(Date(
			boost::gregorian::day_clock::local_day()
			+ boost::gregorian::days(30)));
		cout << "persistence1.setDateCompleted(new + 30 days)" << endl;

		//void setRecurrence(const Recurrence& r);
		persistence1.setRecurrence(*(new RecurrenceDaily("3")));
		cout << "persistence1.setRecurrence(new RecurrenceDaily(\"3\"))" << endl;

		//void setPriority(int priority);
		persistence1.setPriority(7);
		cout << "persistence1.setPriority(7)" << endl;

		//void setCompletedPercentage(int completedPercentage);
		persistence1.setCompletedPercentage(75);
		cout << "persistence1.setCompletedPercentage(75)" << endl;

		delete task1;
		task1 = 0;
		cout << "persistence1.load(" << task1Id << ") -> task1" << endl;
		task1 = persistence1.load(task1Id);
		if (task1) {
			cout << "task1:" << endl;
			printTask(*task1, std::cout);
		}

		////void setDone();

		//void erase();
		cout << "persistence1.erase()" << endl;
		persistence1.erase();
		Task* task3 = persistence1.load(task1Id);
		cout << "persistence1.load(" << task1Id << "): "  << task3 << endl;
	} catch (database_error& e) {
		cout << e.what() << endl;
	}
}

void testTaskManager() {
	
}

int main(int argc, char argv[]) {
	//testDateTime();
	//testDate();
	//testRecurrence();
	//testRecurrenceOnce();
	//testRecurrenceDaily();
	//testRecurrenceWeekly();
	//testRecurrenceMonthly();
	//testRecurrenceYearly();
	//testRecurrenceIntervalDays();
	//testTag();
	//testFilterRule();
	//testTask();
	testTaskPersistence();
	//testTaskManager();
	return 0;
}
