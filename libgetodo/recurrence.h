// $Id$
//
// Description: classes Recurrence*
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef LIBGETODO_RECURRENCE_H
#define LIBGETODO_RECURRENCE_H

#include "common.h"
#include "datetime.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <sstream>

// TODO:
// Support operator==() on Recurrence. It would be useful in TaskPersistence.

namespace getodo {

class DateTime;
class Date;

/** %Recurrence.
 * Representation of repeating date events.
 * Abstract base for various recurrence types.
 *
 * Recurrence types:
 * - once - no recurrence (default)
 * - every Nth day
 * - every Nth week (optionally given selected weekdays)
 * - every Nth month (optionally given a day of month)
 * - every Nth year (optionally given a day and month)
 * - every day in an interval between two days
 */
class Recurrence {
public:
	virtual ~Recurrence();
	/** Virtual cloning support.
	 * Used as a virtual copy constructor.
	 */
	virtual Recurrence* clone() const = 0;

	// Maybe use boost::gregorian::date_iterator inside and return date
	// or make a custom iterator
	/** Return next occurence of the event after given date. */
	virtual Date next(Date start)=0;
	
	/** Serialization.
	 * Note: this function adds a correct recurrence type identifier as a prefix.
	 */
	static std::string toString(const Recurrence& r);
	/** Serialization. Without any type identifier. */
	std::string toString() const;
	/** Deserialization.
	 * Given a string create proper Recurrence* object using
	 * recurrence type identifier.
	 * This is a kind of Factory Method.
	 */
	static Recurrence* fromString(std::string str);

	friend std::ostream& operator<< (std::ostream& o, const Recurrence& r);
	/** Recurrence type name. Used in user interface. */
	virtual std::string getTypeLongName() const = 0;
protected:
	/** Type identifier for serialization. */
	virtual std::string getTypeId() const = 0;
	/** Serialization to an output stream. operator<< cannot be virtual, so
	 * the core code is separated into printOn().
	 */
	virtual void printOn(std::ostream& o) const = 0;
};

/** One time recurrence.
 * Recurrence with no repeating.
 */
class RecurrenceOnce : public Recurrence {
public:
	RecurrenceOnce();
	RecurrenceOnce(const RecurrenceOnce& r);
	virtual ~RecurrenceOnce();
	virtual RecurrenceOnce* clone() const;
	virtual Date next(Date start);
	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	/** Serialization. Eg. "" */
	virtual void printOn(std::ostream& o) const;
};

/** Daily recurrence.
 * Occurs every n-th day.
 */
class RecurrenceDaily : public Recurrence {
private:
	int period;
public:
	RecurrenceDaily(int period);
	explicit RecurrenceDaily(std::string s);
	RecurrenceDaily(const RecurrenceDaily& r);
	virtual ~RecurrenceDaily();
	virtual RecurrenceDaily* clone() const;
	virtual Date next(Date start);

	/* Period in days. */
	int getPeriod() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	/** Serialization. Eg. "2" */
	virtual void printOn(std::ostream& o) const;
};

/** Weelky recurrence.
 * Occurs every week. Two modes can be used: every week after specified date
 * or using a selection of weekdays.
 */
class RecurrenceWeekly : public Recurrence {
public:
	typedef std::set<boost::gregorian::greg_weekday> weekdaySet_t;
private:
	int period;
	weekdaySet_t weekdaySelection;
	// Switch for optionally specified weekday selection:
	// true - next date will be computed using weekday selection
	// false - next date will be a week after date given in next()
	bool useWeekdaySelection;
public:
	RecurrenceWeekly(int period);
	RecurrenceWeekly(int period, weekdaySet_t weekdays);
	explicit RecurrenceWeekly(std::string s);
	RecurrenceWeekly(const RecurrenceWeekly& r);
	virtual ~RecurrenceWeekly();
	virtual RecurrenceWeekly* clone() const;
	virtual Date next(Date start);

	/* Period in weeks. */
	int getPeriod() const;
	/* On which days of week the event can happen. If empty the recurrence
	 * is computed on that weekday as in the date given. */
	weekdaySet_t getWeekdaySelection() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	/** Serialization. Eg. "1 Mon Tue" */
	virtual void printOn(std::ostream& o) const;
};

/** Monthly recurrence.
 * Occurs every month. Two modes can be used: every month after specified date
 * or on given day of month.
 */
class RecurrenceMonthly : public Recurrence {
private:
	int period;
	boost::gregorian::greg_day dayOfMonth;
	// Switch for optionally specified day of month:
	// true - next date will be on dayOfMonth next month
	// false - next date will be a month after date given in next()
	bool useDayOfMonth;
public:
	RecurrenceMonthly(int period);
	RecurrenceMonthly(int period, boost::gregorian::greg_day dayOfMonth);
	explicit RecurrenceMonthly(std::string s);
	RecurrenceMonthly(const RecurrenceMonthly& r);
	virtual ~RecurrenceMonthly();
	virtual RecurrenceMonthly* clone() const;
	virtual Date next(Date start);
	
	int getPeriod() const;
	boost::gregorian::greg_day getDayOfMonth() const;
	bool isDayOfMonthSet() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	/** Serialization. Eg. "3 14" */
	virtual void printOn(std::ostream& o) const;
};

/** Yearly recurrence.
 * Occurs every year. Two modes can be used: every year after specified date
 * or on given day and month of year.
 */
class RecurrenceYearly : public Recurrence {
private:
	boost::gregorian::partial_date dayAndMonth;
	/** A switch for optionally specified day and month:
	 *  * true - next date will be on useDayAndMonth next year
	 *  * false - next date will be a year after date given in next()
	 */
	bool useDayAndMonth;
public:
	RecurrenceYearly();
	RecurrenceYearly(boost::gregorian::partial_date dayAndMonth);
	explicit RecurrenceYearly(std::string s);
	RecurrenceYearly(const RecurrenceYearly& r);
	virtual ~RecurrenceYearly();
	virtual RecurrenceYearly* clone() const;
	virtual Date next(Date start);
	
	boost::gregorian::partial_date getDayAndMonth() const;
	bool isDayAndMonthUsed() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	/** Serialization. Eg. "25 Dec" */
	virtual void printOn(std::ostream& o) const;
};

/** %Recurrence on interval of days.
 * Occurs every day between two dates.
 * If input is bad, next() will return not_a_date_time,
 * just like RecurrenceOnce::next().
 * Period will be invalid, eg. of zero length.
 */
class RecurrenceIntervalDays : public Recurrence {
private:
	boost::gregorian::date_period interval;
public:
	RecurrenceIntervalDays(boost::gregorian::date_period datePeriod);
	explicit RecurrenceIntervalDays(std::string s);
	RecurrenceIntervalDays(const RecurrenceIntervalDays& r);
	virtual ~RecurrenceIntervalDays();
	virtual RecurrenceIntervalDays* clone() const;
	virtual Date next(Date start);

	boost::gregorian::date getDateStart() const;
	boost::gregorian::date getDateEnd() const;

	virtual std::string getTypeLongName() const;
protected:
	virtual std::string getTypeId() const;
	virtual void printOn(std::ostream& o) const;
};
	
}

#endif // LIBGETODO_RECURRENCE_H
