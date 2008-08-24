// $Id$

#include "datetime.h"

namespace getodo {

// ----- class DateTime --------------------

std::string DateTime::format("%Y-%m-%d %H:%M:%S");

DateTime::DateTime() {}
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

std::string DateTime::toString(const DateTime& date) {
	//return boost::posix_time::to_iso_extended_string(date.date);

	using namespace boost;
	posix_time::time_facet* time_output = new posix_time::time_facet(DateTime::format.c_str());
	std::ostringstream ss;
	ss.imbue(std::locale(ss.getloc(), time_output));
	ss << date.date;
	return ss.str();
}

std::string DateTime::toString() const {
	return DateTime::toString(*this);
}

// ----- class Date --------------------

std::string Date::format("%Y-%m-%d");

//Date::Date() : date(boost::gregorian::date(boost::date_time::not_a_date_time)) {}
Date::Date() {}
Date::Date(const boost::gregorian::date& d) : date(d) {}
Date::Date(const Date& d) : date(d.date) {}
Date::Date(std::string d) {
	// date = boost::gregorian::from_string(d);

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

std::string Date::toString(const Date& date) {
	//return boost::gregorian::to_iso_extended_string(date.date);

	using namespace boost;
	gregorian::date_facet* date_output = new gregorian::date_facet(Date::format.c_str());
	std::ostringstream ss;
	ss.imbue(std::locale(ss.getloc(), date_output));
	ss << date.date;
	return ss.str();
}

std::string Date::toString() const { return Date::toString(*this); }

// ----- class Recurrence --------------------

std::string Recurrence::toString(const Recurrence& r) {
	std::ostringstream ss;
	ss << r.getTypeId() << ' ' << r.toString();
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

// ----- class Recurrence --------------------

Recurrence::~Recurrence() {}

// ----- class RecurrenceOnce --------------------

RecurrenceOnce::RecurrenceOnce() {}
RecurrenceOnce::~RecurrenceOnce() {}

Date RecurrenceOnce::next(Date start) {
	return Date(); // not_a_date_time
}

std::string RecurrenceOnce::toString() const { return std::string(); }

std::string RecurrenceOnce::getTypeId() const { return std::string(); }

// ----- class RecurrenceDaily --------------------

RecurrenceDaily::RecurrenceDaily(std::string s) {
	try {
		period = boost::lexical_cast<int, std::string>(s);
	} catch (boost::bad_lexical_cast e) {
		period = 1; // default: every day
	}
}

RecurrenceDaily::~RecurrenceDaily() {}

Date RecurrenceDaily::next(Date start) {
	return Date(start.date + boost::gregorian::days(period));
}

std::string RecurrenceDaily::toString() const {
	return boost::lexical_cast<std::string, int>(period);
}

std::string RecurrenceDaily::getTypeId() const { return std::string("D"); }

// ----- class RecurrenceWeekly --------------------

RecurrenceWeekly::RecurrenceWeekly(std::string s) {
	std::istringstream ss(s);
	// period
	ss >> this->period;
	if (ss.fail()) {
		this->period = 1;
	}
	// weekday selection
	boost::gregorian::greg_weekday weekday(0);
	while (!ss.eof()) {
		ss >> weekday;
		if (!ss.fail()) {
			this->weekdaySelection.insert(weekday);
		}
	}
}

RecurrenceWeekly::~RecurrenceWeekly() {}

Date RecurrenceWeekly::next(Date start) {
	// TODO
	return start; // stub
}

std::string RecurrenceWeekly::toString() const {
	std::ostringstream ss;
	//period
	ss << boost::lexical_cast<std::string, int>(period) << ' ';
	// weekday selection
	for (weekdaySet_t::const_iterator it = weekdaySelection.begin();
		it != weekdaySelection.end(); ++it) {
		ss << *it << ' ';
	}
	// TODO: don't output spaces at the end
	return ss.str();
}

std::string RecurrenceWeekly::getTypeId() const { return std::string("W"); }

// ----- class RecurrenceMonthly --------------------

RecurrenceMonthly::RecurrenceMonthly(std::string s)
: dayOfMonth(1) {
	std::istringstream ss(s);
	// period
	ss >> this->period;
	if (ss.fail()) {
		this->period = 1;
	}
	// day of month
	ss >> dayOfMonth;
	useDayOfMonth = !ss.fail();
}

RecurrenceMonthly::~RecurrenceMonthly() {}

Date RecurrenceMonthly::next(Date start) {
	// TODO
	return start; // stub
}

std::string RecurrenceMonthly::toString() const {
	std::ostringstream ss;
	ss << boost::lexical_cast<std::string, int>(period) << ' ';
	if (useDayOfMonth) {
		ss << dayOfMonth;
	}
	// TODO: don't output spaces at the end
	return ss.str();
}

std::string RecurrenceMonthly::getTypeId() const { return std::string("M"); }

// ----- class RecurrenceYearly --------------------

RecurrenceYearly::RecurrenceYearly(std::string s)
: dayAndMonth(1) {
	using namespace boost::gregorian;
	std::istringstream ss(s);
	// day and month of year
	ss >> dayAndMonth;
	useDayAndMonth = !ss.fail();
}

RecurrenceYearly::~RecurrenceYearly() {}

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

std::string RecurrenceYearly::toString() const {
	std::ostringstream ss;
	//if (useDayOfMonth) {
		ss << dayAndMonth;
	//}
	return ss.str();
}

std::string RecurrenceYearly::getTypeId() const { return std::string("Y"); }

// ----- class RecurrenceIntervalDays --------------------

RecurrenceIntervalDays::RecurrenceIntervalDays(std::string s)
: interval(boost::gregorian::date(), boost::gregorian::days(0)) {
	std::istringstream ss(s);
	// TODO: make different settings for facet (?)
	ss >> interval;
}

RecurrenceIntervalDays::~RecurrenceIntervalDays() {}

Date RecurrenceIntervalDays::next(Date start) {
	using namespace boost::gregorian;
	date nextDay = start.date + days(1);
	if (!interval.is_null() && interval.contains(nextDay)) {
		return Date(nextDay);
	} else {
		return Date();
	}
	
}

std::string RecurrenceIntervalDays::toString() const {
	//// TODO: make different settings for facet (?)
	//// - now: 2008-Dec-31
	//// - i want: 2008-12-31
	//std::ostringstream ss;
	//ss << interval;
	//return ss.str();
	return boost::lexical_cast<std::string, boost::gregorian::date_period>(interval);
}

std::string RecurrenceIntervalDays::getTypeId() const { return std::string("I"); }

// ----- class Duration --------------------

}  // namespace getodo
