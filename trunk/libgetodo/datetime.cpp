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

Recurrence* Recurrence::fromString(std::string str) {
	Recurrence* recurrence = 0;
	if (str.length() <= 0) {
		recurrence = new RecurrenceOnce();
	} else if(str.length() >= 2) {
		// Recurrence type is recognized by the first character.
		// The string representation itself is separeted by a space.
		std::string chopped = str.substr(2); // chop identifier and space
		
		// TODO & NOTES: there must be parsers inside
		//
		// If the input string is in incorrect format the parser should throw
		// an exception and the default type (RecurrenceOnce) should be used.
		//
		// So parsing should be done not in the constructor but in
		// static fromString() method.
		//
		// But such a method shouldn't be confused with Recurrence::fromString().

		//// example
		//try {
		//	switch(str[0]) {
		//		case 'd': recurrence = RecurrenceDaily::fromString(chopped); break;
		//		case 'w': recurrence = RecurrenceWeekly::fromString(chopped); break;
		//		case 'm': recurrence = RecurrenceMonthly::fromString(chopped); break;
		//		case 'y': recurrence = RecurrenceYearly::fromString(chopped); break;
		//		case 'i': recurrence = RecurrenceIntervalDays::fromString(chopped); break;
		//	}
		//} catch (parser_exception) {
		//	recurrence = new RecurrenceOnce();
		//}

		switch(str[0]) {
			case 'd': recurrence = new RecurrenceDaily(chopped); break;
			case 'w': recurrence = new RecurrenceWeekly(chopped); break;
			case 'm': recurrence = new RecurrenceMonthly(chopped); break;
			case 'y': recurrence = new RecurrenceYearly(chopped); break;
			case 'i': recurrence = new RecurrenceIntervalDays(chopped); break;
		}
	}
	return recurrence;
}

// ----- class Recurrence --------------------

Recurrence::~Recurrence() {}

// ----- class RecurrenceOnce --------------------

// stub
RecurrenceOnce::RecurrenceOnce() {}
RecurrenceOnce::~RecurrenceOnce() {}
Date RecurrenceOnce::next(Date start) { return start; }
std::string RecurrenceOnce::toString() const { return std::string(); }

// ----- class RecurrenceDaily --------------------

// stub
RecurrenceDaily::RecurrenceDaily(std::string s) {
	try {
		period = boost::lexical_cast<int, std::string>(s);
	} catch (boost::bad_lexical_cast &e) {
		period = 1; // default - every day
	}
}
RecurrenceDaily::~RecurrenceDaily() {}
Date RecurrenceDaily::next(Date start) { return start; }
std::string RecurrenceDaily::toString() const {
	return boost::lexical_cast<std::string, int>(period);
}

// ----- class RecurrenceWeekly --------------------

// stub
RecurrenceWeekly::RecurrenceWeekly(std::string s) {
	// TODO: parse input string
	period = 1;
	// weekday selection
}
RecurrenceWeekly::~RecurrenceWeekly() {}
Date RecurrenceWeekly::next(Date start) { return start; }
std::string RecurrenceWeekly::toString() const { return std::string(); }

// ----- class RecurrenceMonthly --------------------

// stub
RecurrenceMonthly::RecurrenceMonthly(std::string s) {
	// TODO: parse input string
	period = 1;
	// day in month
}
RecurrenceMonthly::~RecurrenceMonthly() {}
Date RecurrenceMonthly::next(Date start) { return start; }
std::string RecurrenceMonthly::toString() const { return std::string(); }

// ----- class RecurrenceYearly --------------------

// stub
RecurrenceYearly::RecurrenceYearly(std::string s) {
	// TODO: parse input string
	period = 1;
	// day in year
}
RecurrenceYearly::~RecurrenceYearly() {}
Date RecurrenceYearly::next(Date start) { return start; }
std::string RecurrenceYearly::toString() const { return std::string(); }

// ----- class RecurrenceIntervalDays --------------------

// stub
RecurrenceIntervalDays::RecurrenceIntervalDays(std::string s) {
	// TODO: parse input string
	// * date start
	// * date end
}
RecurrenceIntervalDays::~RecurrenceIntervalDays() {}
Date RecurrenceIntervalDays::next(Date start) { return start; }
std::string RecurrenceIntervalDays::toString() const { return std::string(); }

// ----- class Duration --------------------

}  // namespace getodo
