class Task {
	int taskID; // TODO: int or some abstraction?
	string description;

	List<Tag&> tags;
	List<Task&> subtasks;

	Date dateCreated;
	Date dateStarted;
	FuzzyDate dateDeadline;
	Date dateCompleted;
	Duration estDuration; // estimated duration
	Recurrence recurrence;
	
	int priority; // TODO: number or symbol?
	int completedPercentage;
	
	string comment;
}

class Tag {
	int tagID; // TODO: int or some abstraction?
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

class Date {
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
		*  every Nth day in an interval between two days		 		
	*/		
}

class Duration {
	/*
		Duration should express:
			* unit or order (minutes, hours, days, weeks, months, years)
			* count of units
			* or no information					
	*/
}

class FilterRule {
	int filterRuleID;
	string name;
	string rule;
}

class TaskManager {}
