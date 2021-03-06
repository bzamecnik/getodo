/**

@page libsqlite3x libsqlite3x API docs

This is the documentation for sqlite3x and sq3, two slightly different
C++ wrappers for the sqlite3 C API. Both are very small and suitable
for direct inclusion into arbitrary source projects.

The main difference between the APIs is that the sqlite3x makes very
heavy use of exceptions, throwing on any error. The sq3 API, on the
other hand, was designed for environments where exception support
may not be completely enabled (e.g., WinCE).

Caveat: The wide-char code assumes wchar_t is a 2-byte UTF-16 data
type, so Unicode support isn't likely to work on *nix (gcc/Linux
defines wchar_t as UTF-32). This means that all of the
w_char/wstring-based APIs should be avoided. The wide-string API is
only compiled in if the macro SQLITE3X_USE_WCHAR is set to a true
value at build time. That macro is turned on by default /if/ the
header file thinks it can be safely enabled. See sqlite3x.hpp for the
#if blocks which control this.

The sqlite3x code was originally written by Cory Nelson (phrostbyte at users
sourceforge net). In July 2006 it was adopted, hacked upon, and documented,
and is now maintained by stephan beal (stephan at s11n net).

The original sqlite3x source code can be found on Cory's site:

http://dev.int64.org/sqlite.html

The hacked copy can be found somewhere under:

http://s11n.net/sqlite/

and includes a basic Makefile demonstrating how to build it. It has no
dependencies on 3rd-party libraries other than sqlite3
(available from http://sqlite.org and comes preinstalled on many Linux
distributions) and an STL implementation.

Changes between the original sqlite3x and this hacked copy are described
in sqlite3.hpp.

sq3 is based off of sqlite3x, but is a different API which is
100% independent of sqlite3x. sq3 is released into the Public
Domain, whereas sqlite3x uses a zlib-like license (see sqlite3x.hpp
for details).

If you just want one or the other of sq3 or sqlite3x, simply take the source
files named sq3*.* or sqlite3x*.*, as appropriate. Also take refcount.hpp
if you use sq3, as it provide reference-counted pointer sharing (which makes
"copying" of databases objects possible).


<h1>An example of how to use sqlite3x:</h1>

\code
#include <iostream>
#define COUT std::cout << __FILE__ << ":" << __LINE__ << ": "

#include "sqlite3x.hpp"

using namespace sqlite3x;
using namespace std;

int main(void) {
	try {
		sqlite3_connection con(":memory:");

		// Demonstrate two ways of setting up a new table:
		if( 1 )
		{
			table_generator( con, "t_test" )( "number" )( "string" ).create();
		}
		else
		{
			int count=con.executeint("select count(*) from sqlite_master where name='t_test';");
			if(count==0) con.executenonquery("create table t_test(number,string);");
		}

		sqlite3_transaction trans(con);
		// Always use transactions when doing multiple writes because the provide
		// a huge speed increase.

		{
			// commands should be in their own scope
			// because a transaction or connection can't
			// close if a command is still open.

			sqlite3_command cmd(con, "insert into t_test values(?,?);");
			cmd.bind(2, "foobar");

			COUT << "Inserting...\n";
			for(int i=0; i<100; i++) {
				cmd.bind(1, (int)i);
				cmd.executenonquery();
			}
		}
		trans.commit();
		COUT << "Insertions appear to have worked.\n";
		COUT << "Last insert ID = " << con.insertid() << '\n';
		COUT << "Selecting some records...\n";
		{
			sqlite3_command cmd(con, "select rowid,* from t_test where number<10;");
			sqlite3_cursor reader=cmd.executecursor();
			const int colcount = reader.colcount();
			for( int i = 0; i < colcount; ++i )
			{
				std::cout << reader.getcolname( i ) << '\t';
			}
			std::cout << std::endl;
			while( reader.step() )
			{
				for( int i = 0; i < colcount; ++i )
				{
					std::cout << reader.getstring(i) << '\t';
				}
				std::cout << std::endl;
			}
		}

		con.close();
	}
	catch(exception &ex) {
		COUT << "Exception Occured: " << ex.what() << endl;
		return 1;
	}

	COUT << "Done.\n";
	return 0;
}

\endcode


<h1>An example of how to use sq3:</h1>

The sq3 API is quite similar to the sqlite3x API but is designed differently
to accomodate that is is intended for environments where throwing exceptions
is not allowed or not desired. An example:

\code
#include "sq3.hpp"
#include <iostream>
#define COUT std::cout << __FILE << ":"<<__LINE__<<": "
void sq3_example()
{
	using namespace sq3;

	std::string fname("db.db");
	COUT << "Starting tests using file ["<<fname<<"]\n";
	database db;
	if( SQLITE_OK != db.open( fname ) )
	{
		throw std::runtime_error( "do_nothrow_api() could not open db." );
	}
	COUT << db.execute( "create table if not exists t1(a,b,c)" ) << '\n';
	int rvi = -1;
	db.execute( "select count(*) from t1", rvi );
	if( rvi < 5 )
	{ // insert some data...
	  // We use an extra nested scope to ensure that the statement object
	  // is destroyed before the next is created.
		COUT << db.execute( "insert into t1 values(1,2,3)" ) << '\n';
		COUT << db.execute( "insert into t1 values(4,5,6)" ) << '\n';
		statement st(db);
		if( SQLITE_OK != st.prepare( "insert into t1 values(?,?,?)") )
		{
			throw std::runtime_error( "st.prepare() failed!" );
		}
		st.bind(1,7);
		st.bind(2,8);
		st.bind(3,std::string("ho, baby"));
		COUT << "insertion via bind... " << st.execute() << '\n';
	}

	{ // Run a query and display results on std::cout...
	  // Again, we use an extra nested scope to control the lifetime
	  // of statement and cursor objects.
		statement q1( db, "select rowid,* from t1" );
		int colcount = q1.colcount();
		int at = 0;
		std::string buffer;
		char unsigned const * cpt = 0;
		int anint = 0;
		char const * colname = 0;
		cursor rd( q1 );
		char const * separator = "\t";
		for( ; SQLITE_ROW == rd.step(); ++at )
		{ // dump the data in tab-delimited format:
			if( 0 == at )
			{
				for( int i = 0; i < colcount; ++i )
				{
					rd.colname( i, &colname );
					std::cout << colname;
					if( i < (colcount-1) )
					{
						std::cout << separator;
					}
				}
				std::cout << '\n';
			}
			for( int i = 0; i < colcount; ++i )
			{
				cpt = 0;
				rd.get( i, &cpt, anint );
				if( cpt ) std::cout << '['<<cpt<<']';
				else std::cout << "[null]";
				if( i < (colcount-1) )
				{
					std::cout << separator;
				}
			}
			std::cout << '\n';
		}
	}

	COUT << "Ending ["<<fname<<"] tests.\n";
}

\endcode

If you're really into sqlite3 <em>and</em> JavaScript, you may be interested in
the JavaScript wrapper for the sqlite3 API, available as part of
<a href='http://spiderape.sf.net/plugins/sqlite/'>SpiderApe project</a>.


<h1>License:</h1>

The sq3 API is released into the Public Domain.

The sqlite3x code has effectively the same as the zlib license:

<pre>
Copyright (C) 2004-2005 Cory Nelson
Copyright (C) 2006 stephan beal

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
</pre>


*/
