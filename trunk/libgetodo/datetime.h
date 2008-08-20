// $Id$
//
// C++ Interface: DateTime, Date
//
// Description: classes DateTime, Date, Recurrence*, Duration
//
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef LIBGETODO_DATETIME_H
#define LIBGETODO_DATETIME_H

#include <string>

#include <boost/date_time/date.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <boost/lexical_cast.hpp>

namespace getodo {

// THINK:
//	* Make AbstractDate a common ancestor to Date and DateTime?
//  * How to cope with not_a_date_time invalid dates from bad input strings?

class DateTime {
public:
	boost::posix_time::ptime date;
	
	DateTime();
	DateTime(const boost::posix_time::ptime& d);
	DateTime(const DateTime& d); // copy
	explicit DateTime(std::string d);
	// explicit DateTime(Date d);
	
	static DateTime now();

	static std::string toString(const DateTime& date);
	std::string toString() const;
private:
	// format for string representation
	// eg. YYYY-MM-DD HH:MM:SS
	static std::string format;
};

class Date {
public:
	boost::gregorian::date date;
	
	Date();
	Date(const boost::gregorian::date& d);
	Date(const Date& d);
	explicit Date(std::string d);

	static Date now();
	
	// Format: YYYY-MM-DD
	static std::string toString(const Date& date);
	std::string toString() const;

	static std::string format;
};

//class FuzzyDate {
	/* TODO
	Date should be able to express:
		* date + time exactly given
		* date alone
		* indefinite date
		* no information
	*/
//}

class Recurrence {
	// Recurrence types:
	//   * once - no recurrence - default
	//   * every Nth day
	//   * every Nth week - optionally given selected weekdays (default all)
	//   * every Nth month - optionally given a day in month
	//   * every Nth year - optionally given a day and month
	//   * every day in an interval between two days
public:
	virtual ~Recurrence();

	// Use boost::gregorian::date_iterator inside and return date
	// or make a custom iterator
	virtual Date next(Date start)=0;
	virtual std::string toString()const =0;
	
	// Given a string create proper Recurrence* object
	// TODO: If the input string is not in correct format
	// create RecurrenceOnce as a default type
	static Recurrence* fromString(std::string str);
};

class RecurrenceOnce : public Recurrence {
public:
	RecurrenceOnce();
	virtual ~RecurrenceOnce();
	virtual Date next(Date start);
	virtual std::string toString() const;
};

class RecurrenceDaily : public Recurrence {
private:
	int period;
public:
	explicit RecurrenceDaily(std::string s);
	virtual ~RecurrenceDaily();
	virtual Date next(Date start);
	virtual std::string toString() const;
};

class RecurrenceWeekly : public Recurrence {
private:
	int period;
	// weekday selection:
	// std::set of boost::gregorian::greg_weekday
	// (enum boost::date_time::weekdays)
	// * operator >> a << pro I/O
	// boost::date_time::weekdays;

	// * format:
	// %a - Abbreviated weekday name ("Mon")
	// %w - Weekday as decimal number 0 to 6

public:
	explicit RecurrenceWeekly(std::string s);
	virtual ~RecurrenceWeekly();
	virtual Date next(Date start);
	virtual std::string toString() const;
};

class RecurrenceMonthly : public Recurrence {
private:
	int period;
	// day in month:
	// boost::gregorian::greg_day dayInMonth;
	// format:
	// %d - Day of the month as decimal 01 to 31
	// %e - Like %d, the day of the month as a decimal number, but a leading zero is replaced by a space
public:
	explicit RecurrenceMonthly(std::string s);
	virtual ~RecurrenceMonthly();
	virtual Date next(Date start);
	virtual std::string toString() const;
};

class RecurrenceYearly : public Recurrence {
private:
	int period;
	// day of year:
	// boost::gregorian::day_of_year_type
	// format: %j - (1..366)
public:
	explicit RecurrenceYearly(std::string s);
	virtual ~RecurrenceYearly();
	virtual Date next(Date start);
	virtual std::string toString() const;
};

class RecurrenceIntervalDays : public Recurrence {
private:
	Date start;
	Date end;
	// or boost::gregorian::date_period
public:
	explicit RecurrenceIntervalDays(std::string s);
	virtual ~RecurrenceIntervalDays();
	virtual Date next(Date start);
	virtual std::string toString() const;
};

class Duration {
	/* TODO
	Duration should express:
		* units of given magnitude (minutes, hours, days, weeks, months, years)
		* - lesser magnitude => greater precision
		* or no information
	Use:
		boost::gregorian::years, boost::gregorian::months, boost::gregorian::weeks
		boost::gregorian::date_duration
		boost::posix_time::hours, boost::posix_time::minutes, boost::posix_time::seconds
	*/
};

}

#endif // LIBGETODO_DATETIME_H
