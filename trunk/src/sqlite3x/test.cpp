/*
  Test/demo code for the sqlite3x and sq3 APIs.
  Public Domain code by stephan@s11n.net
*/

#include <iostream>
#include <vector>
#define COUT std::cout << __FILE__ << ":" << std::dec<<__LINE__ << ": "

#include "sqlite3x.hpp"

using namespace std;


#include <cstdlib> // rand()
#include <ctime>
#include <cstdio>
#include <sstream> // ostringstring class
/**
   A simple example of writing a custom SQL function for sqlite3.

   It provides results of a so-called Fudge-dice roll. Fudge dice (dF)
   are used by the Fudge roleplaying system (http://fudgerpg.com).

   Call it with 0, 1 or 2 arguments:

   0 args: same as calling dF(4).

   1 arg: arg1 is an integer telling how many dice to roll.

   2 args: arg2 is any value. The TYPE of the value is used to
   dynamically set the return type. If arg2 is an INTEGER (the
   default) or REAL then a number of the appropriate type is
   returned to the caller via sqlite_result_xxx(context,...). If arg2
   is a string then a description showing the results of each die roll
   of the set, plus the total, is sent back.

*/
void sqlite_func_dF(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv )
{
	static bool seeded = false;
	if( !seeded && (seeded=true) )
	{
		std::srand( std::time(NULL) );
	}

	if( argc > 2 )
	{
		sqlite3_result_error( context, "dF() function requires 0, 1, or 2 arguments: (0==roll 4dF and return int), (1==roll ARG0dF and return int), (2=roll ARG0dF and return result as TYPEOF(ARG1))", -1 );
		return;
	}

	int count = 0;
	int returnT = (argc<2) ? SQLITE_INTEGER : sqlite3_value_type(argv[1]);
	if( 0 == argc )
	{
		count = 4;
	}
	else
	{
		count = sqlite3_value_int(argv[0]);
	}
	if( count < 1 ) count = 4;

	int reti = 0;
	std::ostringstream rets;
	if( SQLITE_TEXT == returnT )
	{
		rets << count << "dF: ";
	}
	{
		int rnd;
		char marker;
		while( count-- )
		{
			rnd = std::rand() % 3 - 1;
			if( SQLITE_TEXT == returnT )
			{
				marker = ((0==rnd) ? '0' : ((1==rnd) ? '+' : '-'));
				rets << marker << ' ';
			}
			reti += rnd;
		}
	}
	if( SQLITE_TEXT == returnT )
	{
		rets << " = "<<reti;
		std::string rs = rets.str();
		sqlite3_result_text( context, rs.c_str(), rs.size(), SQLITE_TRANSIENT );
	}
	else if( SQLITE_FLOAT == returnT )
	{
		sqlite3_result_double(context, 1.00001 * reti);
	}
	else
	{
		sqlite3_result_int(context, reti);
	}
	return;
}

// For use with my_sq3_callback()
struct row_pos {
	int row;
	row_pos() : row(0) {}
};
int my_sq3_callback(void *data ,int colc, char** argv, char** colNames)
{
	row_pos * rp = static_cast<row_pos *>( data );
	int atrow = rp ? rp->row : -1;
	if( rp ) ++(rp->row);
	if( 0 == atrow )
	{
		COUT << "my_sq3_callback(void*,"<<colc<<",char**,char**)\n";
		for( int i = 0; i < colc; ++i )
		{
			std::cout << colNames[i] << '\t';
		}
		std::cout << '\n';
	}
	for( int i = 0; i < colc; ++i )
	{
		std::cout << (argv[i] ? argv[i] : "NULL") << '\t';
	}
	std::cout << std::endl;
	return 0;
}

void do_conventional_api()
{
	using namespace sqlite3x;
	sqlite3 * dbh = 0;
	sqlite3_open( ":memory:", &dbh );
	sqlite3_connection con(dbh);
	con.executenonquery( "PRAGMA synchronous = OFF" );

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
	{
		// commands should be in their own scope
		// because a transaction or connection can't
		// close if a command is still open.
		sqlite3_command cmd(con, "insert into t_test values(?,?);");
		cmd.bind(2, "foobar");
		const int icount = 100;
		COUT << "Inserting " << icount << " records...\n";
		for(int i=0; i<icount; i++) {
			cmd.bind(1, i);
			cmd.executenonquery();
		}
	}
	trans.commit();


	{
		sqlite3_transaction ftrans(con);
		sqlite3_command cmd( con, "update t_test set number='-99' where rowid=1" );
		cmd.executenonquery();
		// Do not commit - let the transaction roll back.
		// ftrans.commit();
	}

	COUT << "Insertions appear to have worked.\n";
	COUT << "Last insert ID = " << con.insertid() << '\n';
	COUT << "Selecting some records...\n";
	{
		sqlite3_command cmd(con, "select rowid,* from t_test where rowid<10;");
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

	table_generator( con, "t_test2" )( "number" )("value")( "string" ).create();
	row_pos rp;
	con.executecallback( "select * from sqlite_master", my_sq3_callback, &rp );

	con.close();

}


#include "sqlite3x_settings_db.hpp"
#include "sq3.hpp"
#include "sq3_settings_db.hpp"
void do_settings_db()
{
	COUT << "do_settings_db() starting...\n";
	using namespace sqlite3x;

	settings_db db("settings.db");

	sqlite3_transaction tr(*db.db());
	db.set("f1", 42 );
	db.set("f2", 42.24 );
	db.set("f1", 43 );
	db.set("f3", "fourty two" );
	db.set("f3.5", std::string("fourty two") );
	db.set("f4", true );
	tr.commit();

	bool got;
#define GET(K,V) got = db.get( K, V );	\
	COUT << "got == "<< got << ": " << K << " == " << V << '\n';

	int i1;
	double d1;
	std::string s1;
	bool b1;
	
	GET("f1",i1);
	GET("f2",d1);
  	GET("f3",s1);
 	GET("f3.5",s1);
 	GET("f4",b1);
 	GET("f5",s1);
 	GET("f6",d1);
#undef GET

	COUT << "Now try sq3::settings_db...\n";

	sq3::settings_db sset( "settings2.db" );
	if( ! sset.is_open() )
	{
		throw std::runtime_error( "do_settings_db(): sset.is_open() failed." );
	}
	sset.set( "foo", "bar" );
	sset.set( "bar", "42" );
	s1 = "error";
	i1 = -1;
	sset.get( "foo", s1 );
	sset.get( "bar", i1 );
	COUT << "foo=="<<s1<<"\nbar=="<<i1<<'\n';

	COUT << "do_settings_db() ending\n";
}

void do_nothrow_api()
{
	using namespace sq3;

	std::string fname("db.db");
	COUT << "Starting tests using file ["<<fname<<"]\n";
	database db;
	if( SQLITE_OK != db.open( fname ) )
	{
		throw std::runtime_error( "do_nothrow_api() could not open db." );
	}
	sqlite3_create_function( db.handle(), "dF",-1,SQLITE_ANY,0,sqlite_func_dF,0, 0 );

	COUT << db.execute( "create table if not exists t1(a,b,c)" ) << '\n';
	int rvi = -1;
	COUT << db.execute( "select count(*) from t1", rvi ) << '\n';
	COUT << "rvi == " << rvi << '\n';
	double rvd = -1.0;
	COUT << db.execute( "select (count(*)*10/3.2) from t1", rvd ) << '\n';
	COUT << "rvd == " << rvd << '\n';

	if( rvi < 5 )
	{
		COUT << db.execute( "insert into t1 values(1,2,3)" ) << '\n';
		COUT << db.execute( "insert into t1 values(4,5,NULL)" ) << '\n';
		statement st(db);
		if( SQLITE_OK != st.prepare( "insert into t1 values(?1,?3,?2)") )
		{
			throw std::runtime_error( "st.prepare() failed!" );
		}
		st.bind(1,7);
		st.bind(2,8);
		st.bind(3,"hey, baby",-1);
		st.bind(3,std::string("ho, baby"));
		COUT << "insertion via bind... " << st.execute() << '\n';
	}

	{
		statement q1( db, "select rowid,* from t1" );
		cursor rd = q1.get_cursor();
		int colcount = rd.colcount();
		int at = 0;
		std::string buffer;
		char unsigned const * cpt = 0;
		int anint = 0;
		char const * colname = 0;
		bool nullcheck =false;
		for( ; SQLITE_ROW == rd.step(); ++at )
		{
			if( 0 == at )
			{
				for( int i = 0; i < colcount; ++i )
				{
					//rd.colname(i,buffer);
					colname = 0;
					rd.colname( i, &colname );
					std::cout << (colname ? colname : "EMPTY_COLNAME");
					if( i < (colcount-1) )
					{
						std::cout << '\t';
					}
				}
				std::cout << '\n';
			}
			for( int i = 0; i < colcount; ++i )
			{
				if( 0 )
				{
					buffer = "";
					rd.get( i, buffer );
					std::cout << "["<<(buffer.empty() ? "null" : buffer)<<"]";
				}
				else
				{
					rd.isnull(i,nullcheck);
					if( nullcheck )
					{
						std::cout << "[NULL]";
					}
					else
					{
						cpt = 0;
						rd.get( i, &cpt, anint );
						if( cpt ) std::cout << '['<<cpt<<']';
						else std::cout << "[null]";
					}
				}
				if( i < (colcount-1) )
				{
					std::cout << '\t';
				}
			}
			std::cout << '\n';
		}
	}

	{
		std::string buf("error");
		db.execute( "select dF(4,'')", buf );
		COUT << "dF(): " << buf << std::endl;
		db.execute( "select dF(7)", buf );
		COUT << "dF(): " << buf << std::endl;
// 		sqlite3_text_char_t const * buf;
// 		int bufsz = 0;
// 		rd.get( 0, &buf,  bufsz );
// 		if( buf ) COUT << buf << std::endl;
// 		else COUT << "(WTF?)" << std::endl;

	}


	{
		COUT << "Now trying to fetch from a result set by column name:\n";
		statement st(db,"select dF(4) as left, dF(4,'') as right");
		if( ! st.is_prepared() )
		{
			throw std::runtime_error( "SELECT with named arguments failed to compile." );
		}
		cursor cur( st.get_cursor() );
		for( int i = 0; i < 6; ++i )
		{
			std::string left = "UNEXPECTED_ERROR";
			std::string right = left;
			std::string oops = "AN_EXPECTED_ERROR";
			while( SQLITE_ROW == cur.step() )
			{
				cur.get("left",left);
				cur.get("right",right);
				cur.get("oops",oops);
				COUT << "#"<<(i+1)<<" dF() results: "<<left<<" and " << right <<" (and... "<<oops<<")\n";
			}
			cur.reset();
		}
	}


	COUT << "Ending ["<<fname<<"] tests.\n";
}

#include "sq3_log_db.hpp"
void do_log_db()
{
	using namespace sq3;
	log_db log( std::string("log.db") );
	for( int i = 0; i < 5; ++i )
	{
		log.log( "log db, entry %d", i );
	}
	log.log( "log db, logging out..." );
	log.show_last( 4 );
	log.trim(2);
	log.log( "log db trimmed." );
	log.show_last( 5 );

}

void do_clear_db()
{
	COUT << "do_clear_db() creating a db and clear()ing it...\n";
	using namespace sq3;
	std::string dbname("clear.db");
	database db(dbname);
	transaction trans(db);
	COUT << db.is_open() << '\n';
	COUT << db.execute("create table if not exists foo(a,ts)") << '\n';
	COUT << db.execute("create table if not exists bar(b,ts)") << '\n';
	COUT << db.execute("insert into foo values(1,current_timestamp)") << '\n';
	COUT << db.execute("insert into foo values(2,current_timestamp)") << '\n';
	int count = 0;
	COUT << db.execute("select count(*) from foo",count) << '\n';
	COUT << "Record count BEFORE copying database handle: " << count << '\n';
	
#if 1
	if( 1 ) {
		// test copy ops...
		database d2(db);
		int count = 0;
		d2.execute("insert into foo values(1.2,current_timestamp)");
		d2.execute("insert into foo values(2.2,current_timestamp)");
		d2.execute("select count(*) from foo", count);
		COUT << "we just 'copied' the db. It worked if the following number is >=4: "<<count<<'\n';
	}
#endif // 0|1
	count = 0; db.execute("select count(*) from foo",count);
	COUT << "Record count AFTER database copy fiddled with it: " << count << '\n';
	COUT << "Copy of db should be gone, but db handle should still be open...\n";
	COUT << "db.is_open() == "<<db.is_open()<<'\n';
	int rc = db.execute("insert into bar (b,ts) values(3,current_timestamp)");
	COUT << "insert into bar: " << rc << '\n';
	if( ! sq3::rc_is_okay( rc ) )
	{
		throw std::runtime_error("do_clear_db(): db.execute() failed.");
	}
	count = 0; db.execute("select count(*) from foo",count);
	COUT << "Total record count: " << count << '\n';
	trans.commit();
 	db.clear();
 	db.vacuum();

	COUT << "Check "<<dbname<<" to make sure it's really empty.\n";
}


struct RCPtrTest
{
	RCPtrTest()
	{
		COUT << "RCPtrTest @ " << std::hex<<this<<'\n';
	}
	~RCPtrTest()
	{
		COUT << "~RCPtrTest @ " << std::hex<<this<<'\n';
	}
};

void do_test_rcptr()
{
	using namespace sq3;
	using namespace refcount;
	typedef rcptr<RCPtrTest,plain_delete_finalizer> TestT1;
	typedef rcptr<sqlite3,sqlite3_finalizer> Sqrc;
	COUT << "Should see a single ctor/dtor pair in the next lines:\n";
	{
		TestT1 t2;
		{
			TestT1 t1( new RCPtrTest );
			t2 = t1;
		}
		{
			TestT1 t3; t3 = t2;
		}
		if( 1 )
		{
			sqlite3 * sq = 0;
			int rc = sqlite3_open( ":memory:", &sq );
			Sqrc s1(0);
			if( SQLITE_OK == rc )
			{
				s1.take( sq );
			}
			Sqrc s2 = s1;
		}
		//t2.take(); // cause a leak, if all works properly.
	}
	COUT << "^^^^ should see a single dtor call there for each tested rcptr<> type.\n";
}

#define DO_BOGO_TESTS 0
#if DO_BOGO_TESTS
#include "refcount.hpp"

void do_bogo_tests()
{

	COUT << "do_bogo_tests()\n";
// 	using namespace refcount;
// 	typedef rcptr<RCPtrTest> RCP;
// 	RCP p1( new RCPtrTest );
// 	RCP p2;
// 	{
// 		p2.copy( p1.get() );
// 		RCP p3( p2 );
// 		RCP p4( p3.get() );
// 		p4.copy( p4.get() );
// 	}
}
#endif // DO_BOGO_TESTS

int main(void)
{
	try
	{
#if DO_BOGO_TESTS
		do_bogo_tests();
#else
		do_test_rcptr();
		do_conventional_api();
		do_nothrow_api();
		do_log_db();
		do_settings_db(); // this is slow!
		do_clear_db();
#endif
	}
	catch(std::exception const & ex)
	{
		COUT << "Exception Occurred: " << ex.what() << endl;
		return 1;
	}

	COUT << "Done.\n";
	return 0;
}
