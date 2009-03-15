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

bool DateTime::operator==(const DateTime& rhs) const {
	return date == rhs.date;
}

bool DateTime::operator!=(const DateTime& rhs) const {
	return !(*this == rhs);
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

bool Date::operator==(const Date& rhs) const {
	return date == rhs.date;
}

bool Date::operator!=(const Date& rhs) const {
	return !(*this == rhs);
}

// ----- class Duration --------------------

}  // namespace getodo
