
#include "sq3_log_db.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdio> // vsnprint()
namespace sq3 {

	log_db::log_db( std::string const & filename )
		: database()
	{
		this->database::open(filename);
	}

	log_db::~log_db()
	{
	}

	int log_db::on_open()
	{
		if( ! this->is_open() )
		{
			return SQLITE_ERROR;
		}
		std::string sql( "create table if not exists log(ts,msg TEXT)" );
		this->execute( sql );
		/**
		   enable temp_store=MEMORY to speed this up considably on PocketPC
		   devices writing to SD cards.
		*/
		this->pragma( "temp_store = MEMORY" );
		return SQLITE_OK;
	}

	int log_db::clear()
	{
		return this->execute( "delete from log" );
	}

	static char const * LOG_DB_LOG_INSERT_SQL = "insert into log (ts,msg) values(strftime('%Y-%m-%d %H:%M:%f','now'),?)";
		// "insert into log (ts,msg) values(current_timestamp,?)"

	bool log_db::log( std::string const & msg )
	{
		if( ! this->is_open() )
		{
			return false;
		}
		if( msg.empty() ) return true;
		statement st( *this, LOG_DB_LOG_INSERT_SQL );
		st.bind( 1, msg );
		int rc = st.execute();
		return rc_is_okay( rc );
		// In theory, if the count_changes PRAGMA is on then SQLITE_ROW will be returned from execute()
	}

	bool log_db::log(const char *format,...)
	{
		if( ! this->is_open() )
		{
			return false;
		}
		const int buffsz = static_cast<int>( std::max( (size_t) 2048, strlen(format) * 2 ) );
		std::vector<char> buffer( buffsz, '\0' );
		va_list vargs;
		va_start ( vargs, format );
		using namespace std;
		/** In gcc, vsnprintf() is in the std namespace, but in MSVC it is not, so we use 'using'
		    to accomodate both cases. */
		int size = vsnprintf(&buffer[0], buffsz, format, vargs);
		va_end( vargs );
		if (size > (buffsz-1))
		{
			// replace tail of msg with "..."
			size = buffsz-1;
			for( int i = buffsz-4; i < buffsz-1; ++i )
			{
				buffer[i] = '.';
			}
		}
		buffer[size] = '\0';
		if( size )
		{
			statement st( *this, LOG_DB_LOG_INSERT_SQL );
			st.bind( 1, &buffer[0], size );
			int rc = st.execute();
			//std::cout << "FYI: rc from an INSERT is " << rc << '\n'; // == SQLITE_DONE
			return SQLITE_DONE == rc;
		}
		return true;
	}

#undef LOG_DB_LOG_INSERT_SQL

	void log_db::show_last( int count )
	{
		if( ! this->is_open() )
		{
			return;
		}
		std::ostream & os = std::cout;
		os << "sq3::log_db: most recent "
		   << count << " entries:\n";
		if( ! this->is_open() )
		{
			os << "ERROR: Log database is not opened!";
			return;
		}
		std::ostringstream fmt;
		if( 0 )
		{ // newest entries at the top:
		fmt << "select /*DATETIME(ts)*/ts,msg from log "
		    << "order by ts desc, rowid desc"
		    <<" limit " << count
			;
		}
		else
		{ // in "natural order":
			fmt << "select /*DATETIME(ts)*/ts,msg from log "
			    << "order by ts asc, rowid asc"
			    <<" limit " << count
				;
		}
		std::string sql(fmt.str());
		statement st( *this, sql );
		cursor r = st.get_cursor();
		std::string buff;
		while( SQLITE_ROW == r.step() )
		{
			std::string tmp;
			r.get( 0, tmp );
			os << tmp << ": ";
			r.get( 1, tmp );
			os << tmp << '\n';
		}
	}

	bool log_db::trim( int count )
	{
		if( this->is_open() )
		{
			std::ostringstream os;
			os << "delete from log where rowid not in (select rowid from log order by ts desc, rowid desc limit "<<count<<")";
			std::string sql( os.str() );
			if( SQLITE_OK == this->execute( sql.c_str() ) )
			{
				this->vacuum();
			}
			return true; // delete will fail if the db is empty, but we'll consider that to be success
		}
		return false;
	}

} // namespace
