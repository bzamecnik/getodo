
#include "sq3_settings_db.hpp"

namespace sq3
{

	settings_db::settings_db()
		: database()
	{
	}

	settings_db::settings_db( std::string const & dbname )
		: database()
	{
		this->open( dbname );
	}

	settings_db::~settings_db()
	{
	}

	int settings_db::clear()
	{
		return this->execute( "delete from settings" );
	}

	int settings_db::clear( std::string const & where )
	{
		return this->execute( "delete from settings " + where );
	}


	static std::string SettingsDb_Set_SQL = "insert into settings values(?,?)";

	void settings_db::set( std::string const & key, int val )
	{
		statement st( *this, SettingsDb_Set_SQL );
		st.bind( 1, key );
		st.bind( 2, val );
		st.execute();
	}

	void settings_db::set( std::string const & key, sqlite_int64 val )
	{
		statement st( *this, SettingsDb_Set_SQL );
		st.bind( 1, key );
		st.bind( 2, val );
		st.execute();
	}

	void settings_db::set( std::string const & key, bool val )
	{
		statement st( *this, SettingsDb_Set_SQL );
		st.bind( 1, key );
		st.bind( 2, val ? 1 : 0 );
		st.execute();
	}

	void settings_db::set( std::string const & key, double val )
	{
		statement st( *this, SettingsDb_Set_SQL );
		st.bind( 1, key );
		st.bind( 2, val );
		st.execute();
	}

	void settings_db::set( std::string const & key, std::string const & val )
	{
		statement st( *this, SettingsDb_Set_SQL );
		st.bind( 1, key );
		st.bind( 2, val );
		st.execute();
	}

	void settings_db::set( std::string const & key, char const * val )
	{
		statement st( *this, SettingsDb_Set_SQL );
		st.bind( 1, key );
		st.bind( 2, val ? val : "" );
		st.execute();
	}

	int settings_db::on_open()
	{
		int rc = this->execute( "create table if not exists settings(key PRIMARY KEY ON CONFLICT REPLACE,value)" );
		this->execute( "PRAGMA temp_store = MEMORY" ); // i don't like this, but want to speed up access
		this->pragma( "synchronous = OFF" ); // again: i don't like this but want more speed
		return rc_is_okay(rc) ? SQLITE_OK : rc;
	}

	static std::string SettingsDb_Get_SQL = "select value from settings where key = ?";

	bool settings_db::get( std::string const & key, int & val )
	{
		try
		{
			statement st( *this, SettingsDb_Get_SQL );
			st.bind( 1, key );
			return rc_is_okay( st.execute(val) );
		}
		catch( ... )
		{
			return false;
		}
		return true;
	}

	bool settings_db::get( std::string const & key, sqlite_int64 & val )
	{
		try
		{
			statement st( *this, SettingsDb_Get_SQL );
			st.bind( 1, key );
			return rc_is_okay( st.execute( val ) );
		}
		catch( ... )
		{
			return false;
		}
		return true;
	}
	bool settings_db::get( std::string const & key, bool & val )
	{
		try
		{
			statement st( *this, SettingsDb_Get_SQL );
			st.bind( 1, key );
			int foo;
			int rc = st.execute(foo);
			if( rc_is_okay( rc ) )
			{
				val = ( foo ? true : false);
				return true;
			}
			return false;
		}
		catch( ... )
		{
			return false;
		}
	}
	bool settings_db::get( std::string const & key, double & val )
	{
		try
		{
			statement st( *this, SettingsDb_Get_SQL );
			st.bind( 1, key );
			return rc_is_okay( st.execute(val) );
		}
		catch( ... )
		{
			return false;
		}
		return true;
	}
	bool settings_db::get( std::string const & key, std::string & val )
	{
		try
		{
			statement st( *this, SettingsDb_Get_SQL );
			st.bind( 1, key );
			return rc_is_okay( st.execute(val) );
		}
		catch( ... )
		{
			return false;
		}
		return true;
	}



} // namespace
