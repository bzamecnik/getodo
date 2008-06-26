#ifndef sq3_LOG_DB_HPP_INCLUDED
#define sq3_LOG_DB_HPP_INCLUDED 1

#include "sq3.hpp"
namespace sq3 {

	/**
	   log_db is a simple logging database for use with arbitrary
	   applications. It is intended to be a small logger for
	   simple systems, and not a powerful logging system. The main
	   advantages of using an sqlite3 db instead of a conventional
	   log file are:

	   - We get log parsing for free by using compatible sqlite3 tools.

	   - Since each log entry is its own database record, the log
	   can contain arbitrarily-formatted messages without causing
	   any problems should we want to re-parse the log file later
	   on.

	   - The actual i/o is handled by sqlite3, so we don't need to
	   deal with any of that tedium.

	   In the case that you want to query the data, this type creates
	   a table named 'log' with the fields (ts,msg), containing the timestamp
	   and log message, respectively.
	*/
	class log_db : public database
	{
	public:
		/**
		   Opens/creates a db from the given filename.
		   Note that this function cannot notify the user
		   if the file cannot be created or if the file
		   is not really a DB file (in which case all logging
		   will silently fail)! Use this->is_open() to find out
		   if the open() worked!

		   Note that it is not legal to have more than one instance
		   with the same filename - the second instance will not
		   be able to open the database! It is also not strictly
		   legal to re-use an existing non-log_db database.
		*/
		explicit log_db( std::string const & filename );

		/**
		   Creates an unopened database. Use this->open() to
		   open it.
		*/
		log_db();

		/**
		   Closes this db.
		*/
		virtual ~log_db();

		/**
		   Empties the log database. Returns SQLITE_OK on
		   success or some other value on error (e.g., db is
		   not open).
		*/
		virtual int clear();
		/**
		   Logs a message to the log database. Returns true
		   on success, false on error.

		   If the format string evaluates to an empty string,
		   this function returns true but does not log the
		   entry.
		*/
		bool log( std::string const & msg );

		/**
		   Logs the given printf-formatted string to the
		   database.  If the resulting string is "too long"
		   then it is internally truncated. "Too long" is
		   rather arbitrarily chosen to be 2k of text.

		   If the format string evaluates to an empty string,
		   this function returns true but does not log the
		   entry.
		*/
		bool log( char const * format, ...  );

		/**
		   Shows the last count entries using a subclass-specific
		   method. The default is to send the items to std::cout.
		   A subclass could override this to show a dialog box,
		   for example. A subclass which does so may also want
		   to call the base implementation but certainly doesn't
		   need to.

		   Subclasses are requested to respect the howMany
		   argument, but may of course format the data to suit
		   their desires.
		*/
		virtual void show_last( int howMany );

		/**
		   Deletes all entries in the log except the leaveThisMany
		   most recent. e.g. Trim( 5 ) leaves only the most recent
		   5 entries in the database. The intent of this function
		   is to give applications a way of keeping the log size small
		   without having to use Clear() to empty the whole database.

		   Returns true on success, false on error.
		*/
		bool trim( int leaveThisMany );

	protected:
		/**
		   Called when open() succeeds. Reimplemented
		   to create the logging table if it's not already
		   in the db.
		*/
		virtual int on_open();
	private:
		log_db( log_db const & ); // not implemented
		log_db & operator=( log_db const & ); // not implemented
	};


} // namespace sq3


#endif // sq3_LOG_DB_HPP_INCLUDED
