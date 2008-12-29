// $Id$

#include "datetime.h"

namespace getodo {

// ----- class DateTime --------------------

std::string DateTime::format("%Y-%m-%d %H:%M:%S");

DateTime::DateTime() {} //empty
// or:
//DateTime::DateTime() : date(boost::posix_time::ptime(boost::date_time::not_a_date_time)) {}
DateTime::DateTime(const boost::posix_time::ptime& d) : date(d) {}
DateTime::DateTime(const DateTime& d) : date(d.date) {}
DateTime::DateTime(std::string d) {
	// previous code:
	//return DateTime(boost::posix_time::from_iso_string(str)); // separator: 'T'
	// or:
	//time_from_string() // separator: ' '

	using namespace boost::posix_time;
	time_input_facet* time_input = new time_input_facet(DateTime::format);
	std::istringstream ss(d);
	ss.imbue(std::locale(ss.getloc(), time_input));
	ss >> this->date;
	if (ss.fail()) {
		this->date = ptime(boost::date_time::not_a_date_time);
	}
}

DateTime DateTime::now() {
	return DateTime(boost::posix_time::second_clock::local_time());
}

std::string DateTime::toString() const {
	return boost::lexical_cast<std::string, DateTime>(*this);
}

std::ostream& operator<< (std::ostream& o, const DateTime& date) {
	//return boost::posix_time::to_iso_extended_string(date.date);

	// TODO: handle special values (not_a_date_time) -> output ""
	using namespace boost;
	posix_time::time_facet* time_output = new posix_time::time_facet(DateTime::format.c_str());
	
	// NOTE: We don't want to affect the output stream o, so use a local one.
	// TODO: Save original locale a then restore it.
	std::ostringstream ss;
	ss.imbue(std::locale(ss.getloc(), time_output));
	ss << date.date;
	o << ss.str();
	return o;
}

// ----- class Date --------------------

std::string Date::format("%Y-%m-%d");

//Date::Date() : date(boost::gregorian::date(boost::date_time::not_a_date_time)) {}
Date::Date() {} //empty - not_a_date_time is the default date value
Date::Date(const boost::gregorian::date& d) : date(d) {}
Date::Date(const Date& d) : date(d.date) {}
Date::Date(std::string d) {
	// date = boost::gregorian::from_string(d); // previous code: default format

	using namespace boost::gregorian;
	date_input_facet* date_input = new date_input_facet(Date::format);
	std::istringstream ss(d);
	ss.imbue(std::locale(ss.getloc(), date_input));
	ss >> this->date;
	if (ss.fail()) {
		this->date = boost::gregorian::date(boost::date_time::not_a_date_time);
	}
}

Date Date::now() {
	return Date(boost::gregorian::day_clock::local_day());
}

std::string Date::toString() const {
	return boost::lexical_cast<std::string, Date>(*this);
}

std::ostream& operator<< (std::ostream& o, const Date& date) {
	//return boost::gregorian::to_iso_extended_string(date.date); // previous code: default format

	using namespace boost;
	gregorian::date_facet* date_output = new gregorian::date_facet(Date::format.c_str());
	
	// Output special date values as "".
	// THINK: This code should be done elsewhere (in a special decorator or so),
	// not here because some day we may need to use the special values.
	// NOTE: Date input treats empty string as not-a-date-time,
	// so there's no need to modify the facet settings there.
	std::string sv[5] = {"","", "", "", "" };
	std::vector<std::string> sv_names(&sv[0], &sv[5]);
	gregorian::special_values_formatter sv_formatter(sv_names.begin(), sv_names.end());
	date_output->special_values_formatter(sv_formatter);

	// NOTE: We don't want to affect the output stream o, so use a local one.
	// TODO: Save original locale a then restore it.
	std::ostringstream ss;
	ss.imbue(std::locale(ss.getloc(), date_output));
	ss << date.date;
	o << ss.str();
	return o;
}

// ----- class Recurrence --------------------

Recurrence::~Recurrence() {} //empty

std::string Recurrence::toString(const Recurrence& r) {
	std::ostringstream ss;
	ss << r.getTypeId() << ' ' << r;
	return ss.str();
}

std::string Recurrence::toString() const {
	std::ostringstream ss;
	printOn(ss);
	return ss.str();
}

Recurrence* Recurrence::fromString(std::string str) {
	Recurrence* recurrence = 0;
	if (str.length() >= 2) {
		// Recurrence type is recognized by the first character.
		// The string representation itself is separeted by a space.
		std::string chopped = str.substr(2); // chop identifier and space
		switch(str[0]) {
			case 'D': recurrence = new RecurrenceDaily(chopped); break;
			case 'W': recurrence = new RecurrenceWeekly(chopped); break;
			case 'M': recurrence = new RecurrenceMonthly(chopped); break;
			case 'Y': recurrence = new RecurrenceYearly(chopped); break;
			case 'I': recurrence = new RecurrenceIntervalDays(chopped); break;
			default: recurrence = new RecurrenceOnce(); break;
		}
	} else {
		recurrence = new RecurrenceOnce(); // default
	}
	return recurrence;
}

inline std::ostream& operator<< (std::ostream& o, const Recurrence& r) {
	r.printOn(o);
	return o;
}

// ----- class RecurrenceOnce --------------------

RecurrenceOnce::RecurrenceOnce() {} //empty
RecurrenceOnce::RecurrenceOnce(const RecurrenceOnce& r) {} //empty
RecurrenceOnce::~RecurrenceOnce() {} //empty
RecurrenceOnce* RecurrenceOnce::clone() const {
	return new RecurrenceOnce(*this);
}

Date RecurrenceOnce::next(Date start) {
	return Date(); // not_a_date_time
}

std::string RecurrenceOnce::getTypeLongName() const {
	return std::string("Once");
}

std::string RecurrenceOnce::getTypeId() const { return std::string(); }

void RecurrenceOnce::printOn(std::ostream& o) const { } // empty

// ----- class RecurrenceDaily --------------------

RecurrenceDaily::RecurrenceDaily(int _period)
:	period(_period) {}

RecurrenceDaily::RecurrenceDaily(std::string s) {
	try {
		period = boost::lexical_cast<int, std::string>(s);
	} catch (boost::bad_lexical_cast e) {
		period = 1; // default: every day
	}
}

RecurrenceDaily::RecurrenceDaily(const RecurrenceDaily& r)
:	period(r.period) {}

RecurrenceDaily::~RecurrenceDaily() {} //empty

RecurrenceDaily* RecurrenceDaily::clone() const {
	return new RecurrenceDaily(*this);
}

Date RecurrenceDaily::next(Date start) {
	return Date(start.date + boost::gregorian::days(period));
}

int RecurrenceDaily::getPeriod() const { return period; }

std::string RecurrenceDaily::getTypeLongName() const {
	return std::string("Daily");
}

std::string RecurrenceDaily::getTypeId() const { return std::string("D"); }

void RecurrenceDaily::printOn(std::ostream& o) const {
	o << period;
} 

// ----- class RecurrenceWeekly --------------------

RecurrenceWeekly::RecurrenceWeekly(int _period)
:	period(_period), useWeekdaySelection(false) {}

RecurrenceWeekly::RecurrenceWeekly(int _period, weekdaySet_t weekdays)
:	period(_period), weekdaySelection(weekdays)
{
	useWeekdaySelection = !weekdaySelection.empty();
}

RecurrenceWeekly::RecurrenceWeekly(std::string s) {
	std::istringstream ss(s);
	// period
	ss >> this->period;
	if (ss.fail()) {
		this->period = 1;
	}
	// weekday selection
	boost::gregorian::greg_weekday weekday(0);
	while (!ss.eof()) { // TODO: should be: while(ss >> weekday) {...}
		ss >> weekday;
		if (!ss.fail()) {
			useWeekdaySelection = true;
			this->weekdaySelection.insert(weekday);
		} else {
			break;
		}
	}
}

RecurrenceWeekly::RecurrenceWeekly(const RecurrenceWeekly& r)
:	period(r.period), 
	weekdaySelection(r.weekdaySelection), 
	useWeekdaySelection(r.useWeekdaySelection) {}

RecurrenceWeekly::~RecurrenceWeekly() {} //empty

RecurrenceWeekly* RecurrenceWeekly::clone() const {
	return new RecurrenceWeekly(*this);
}

Date RecurrenceWeekly::next(Date start) {
	// TODO
	return start; // stub
}

int RecurrenceWeekly::getPeriod() const { return period; }

RecurrenceWeekly::weekdaySet_t RecurrenceWeekly::getWeekdaySelection() const {
	return weekdaySelection;
}

std::string RecurrenceWeekly::getTypeLongName() const {
	return std::string("Weekly");
}

std::string RecurrenceWeekly::getTypeId() const { return std::string("W"); }

void RecurrenceWeekly::printOn(std::ostream& o) const {
	//period
	o << period;
	// weekday selection
	if (!weekdaySelection.empty()) {
		o << ' ';
		join(o, weekdaySelection.begin(), weekdaySelection.end(), " ");
	}
}

// ----- class RecurrenceMonthly --------------------

RecurrenceMonthly::RecurrenceMonthly(int _period)
:	period(_period), dayOfMonth(1), useDayOfMonth(false) {}

RecurrenceMonthly::RecurrenceMonthly(int _period, boost::gregorian::greg_day _dayOfMonth)
:	period(_period), dayOfMonth(_dayOfMonth), useDayOfMonth(true) {}

RecurrenceMonthly::RecurrenceMonthly(std::string s)
: dayOfMonth(1), useDayOfMonth(false) {
	std::istringstream ss(s);
	// period
	ss >> this->period;
	if (ss.fail()) {
		this->period = 1;
	}
	// day of month
	// NOTE: Direct assignment to dayOfMonth can cause a crash.	
	unsigned short dayOfMonthInt = 1;
	ss >> dayOfMonthInt;
	useDayOfMonth = !ss.fail();
	if (useDayOfMonth) {
		try {
			dayOfMonth = boost::gregorian::greg_day(dayOfMonthInt);
		} catch(boost::gregorian::bad_day_of_month&) {
			useDayOfMonth = false;
		}
	}	
}

RecurrenceMonthly::RecurrenceMonthly(const RecurrenceMonthly& r)
:	period(r.period),
	dayOfMonth(r.dayOfMonth),
	useDayOfMonth(r.useDayOfMonth) {}

RecurrenceMonthly::~RecurrenceMonthly() {} //empty

RecurrenceMonthly* RecurrenceMonthly::clone() const {
	return new RecurrenceMonthly(*this);
}

Date RecurrenceMonthly::next(Date start) {
	// TODO
	return start; // stub
}

int RecurrenceMonthly::getPeriod() const {
	return period;
}

boost::gregorian::greg_day RecurrenceMonthly::getDayOfMonth() const {
	return dayOfMonth;
}

bool RecurrenceMonthly::isDayOfMonthSet() const {
	return useDayOfMonth;
}

std::string RecurrenceMonthly::getTypeLongName() const {
	return std::string("Monthly");
}

std::string RecurrenceMonthly::getTypeId() const { return std::string("M"); }

void RecurrenceMonthly::printOn(std::ostream& o) const {
	o << period;
	if (useDayOfMonth) {
		o << ' ' << dayOfMonth;
	}
} 

// ----- class RecurrenceYearly --------------------

RecurrenceYearly::RecurrenceYearly()
:	dayAndMonth(1), useDayAndMonth(false) {}

RecurrenceYearly::RecurrenceYearly(boost::gregorian::partial_date _dayAndMonth)
:	dayAndMonth(_dayAndMonth), useDayAndMonth(true) {}

RecurrenceYearly::RecurrenceYearly(std::string s)
:	dayAndMonth(1) {
	using namespace boost::gregorian;
	std::istringstream ss(s);
	// day and month of year
	ss >> dayAndMonth;
	useDayAndMonth = !ss.fail();
}

RecurrenceYearly::RecurrenceYearly(const RecurrenceYearly& r)
:	dayAndMonth(r.dayAndMonth),
	useDayAndMonth(r.useDayAndMonth) {}

RecurrenceYearly::~RecurrenceYearly() {} //empty

RecurrenceYearly* RecurrenceYearly::clone() const {
	return new RecurrenceYearly(*this);
}

Date RecurrenceYearly::next(Date start) {
	using namespace boost::gregorian;
	if(!useDayAndMonth) {
		dayAndMonth = partial_date(start.date.day(), start.date.month());
	}
	date nextDate(not_a_date_time);
	try {
		nextDate = dayAndMonth.get_date(start.date.year());
	} catch(std::out_of_range e) {
	}
	return Date(nextDate);
}

boost::gregorian::partial_date RecurrenceYearly::getDayAndMonth() const {
	return dayAndMonth;
}
bool RecurrenceYearly::isDayAndMonthUsed() const {
	return useDayAndMonth;
}

std::string RecurrenceYearly::getTypeLongName() const {
	return std::string("Yearly");
}

std::string RecurrenceYearly::getTypeId() const { return std::string("Y"); }

void RecurrenceYearly::printOn(std::ostream& o) const {
	if (useDayAndMonth) {
		o << dayAndMonth;
	}
}

// ----- class RecurrenceIntervalDays --------------------

RecurrenceIntervalDays::RecurrenceIntervalDays(boost::gregorian::date_period datePeriod)
:	interval(datePeriod) {}

RecurrenceIntervalDays::RecurrenceIntervalDays(std::string s)
: interval(boost::gregorian::date(), boost::gregorian::days(0)) {
	std::istringstream ss(s);
	// TODO: make different settings for facet (?)
	ss >> interval;
}

RecurrenceIntervalDays::RecurrenceIntervalDays(const RecurrenceIntervalDays& r)
:	interval(r.interval) {}

RecurrenceIntervalDays::~RecurrenceIntervalDays() {} //empty

RecurrenceIntervalDays* RecurrenceIntervalDays::clone() const {
	return new RecurrenceIntervalDays(*this);
}

Date RecurrenceIntervalDays::next(Date start) {
	using namespace boost::gregorian;
	date nextDay = start.date + days(1);
	if (!interval.is_null() && interval.contains(nextDay)) {
		return Date(nextDay);
	} else {
		return Date();
	}
}

boost::gregorian::date RecurrenceIntervalDays::getDateStart() const {
	return interval.begin();
}
boost::gregorian::date RecurrenceIntervalDays::getDateEnd() const {
	return interval.last();
}

void RecurrenceIntervalDays::printOn(std::ostream& o) const {
	// TODO: make different settings for facet (is it reasonable?)
	// - now: 2008-Dec-31
	// - i want: 2008-12-31
	
	// eg.: [2008-Aug-18/2008-Oct-04]
	o << interval;
}

std::string RecurrenceIntervalDays::getTypeLongName() const {
	return std::string("Interval of days");
}

std::string RecurrenceIntervalDays::getTypeId() const { return std::string("I"); }

// ----- class Duration --------------------

}  // namespace getodo
