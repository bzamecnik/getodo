#ifndef s11n_net_SQ3_SETTINGS_DB_INCLUDED
#define s11n_net_SQ3_SETTINGS_DB_INCLUDED 1
// License: Public Domain
// Author: stephan at s11n net

#include "sq3.hpp"

namespace sq3 {

	/**
	   settings_db ia a very simplistic key/value pair database
	   for use with the sq3 database layer. It is intended to
	   be used as a simple config-file class.

	   Usage:
<pre>
	   settings_db db("my.db");
	   db.set("one", 1 );
	   db.set("two", 2.0 );
	   db.set("a_string", "a string" );

	   std::string sval;
	   assert( db.get( "a_string", sval ) );
</pre>

	Obviously, an assert may be too harsh for what you're doing.

	When doing lots of set() calls you can gain a lot of speed by
	adding an sq3::transaction before you start and calling
	commit() on that transaction when you're done.
	*/
	class settings_db : public database
	{
	public:
		/**
		   Calls open(dbname). Use is_open() to find out
		   if the open succeeded.
		*/
		explicit settings_db( std::string const & dbname );
		/**
		   Creates an unopened database. You must call open()
		   before you can use this object.
		 */
		settings_db();
		/**
		   Closes this database.
		*/
		~settings_db();
		/**
		   Overridden to just empty the settings db.
		   Does not remove the db file.
		   Returns SQLITE_OK on success, else false.
		*/
		virtual int clear();
		/**
		   Empties the database items matching the given WHERE
		   clause. Does not remove the db file.

		   'where' should be a full SQL where statement, e.g.:

		   "WHERE KEY LIKE 'x%'"

		   The field names in this db are KEY and VALUE.
		*/
		int clear( std::string const & where );

		/**
		   Sets the given key/value pair.
		 */
		void set( std::string const & key, int val );
		/**
		   Sets the given key/value pair.
		 */
		void set( std::string const & key, sqlite_int64 val );
		/**
		   Sets the given key/value pair.
		 */
		void set( std::string const & key, bool val );
		/**
		   Sets the given key/value pair.
		 */
		void set( std::string const & key, double val );
		/**
		   Sets the given key/value pair.
		 */
		void set( std::string const & key, std::string const & val );
		/**
		   Sets the given key/value pair.
		 */
		void set( std::string const & key, char const * val );

		/**
		   Fetches the given key from the db. If it is found,
		   it is converted to the data type of val, val is
		   assigned that value, and true is returned. If false
		   is returned then val is unchanged.
		*/
		bool get( std::string const & key, int & val );
		/** See get(string,int). */
		bool get( std::string const & key, sqlite_int64 & val );
		/** See get(string,int). */
		bool get( std::string const & key, bool & val );
		/** See get(string,int). */
		bool get( std::string const & key, double & val );
		/** See get(string,int). */
		bool get( std::string const & key, std::string & val );

	private:
		/** Reimplemented to initialize the settings table and enable some speed tweaks. */
		virtual int on_open();
	};

} // namespace


#endif // s11n_net_SQ3_SETTINGS_DB_INCLUDED
