#include "test1.h"

using namespace getodo;

// tip: use Boost.Test http://www.boost.org/doc/libs/1_35_0/libs/test/doc/index.html

int main(int argc, char argv[]) {
	using namespace std;
	TaskManager tm("test1.db");
	sqlite3_connection *conn = tm.getConnection();
	cout << "TaskManager created" << endl;
	
	// ----- Tag -----
	Tag tag1;
	Tag tag2("photos");
	Tag tag3(tag2);
	Tag tag4(123, "photos");
	cout << "tag1: " << tag1.tagId << ", " << tag1.tagName << endl;
	cout << "tag2: " << tag2.tagId << ", " << tag2.tagName << endl;
	cout << "tag3: " << tag3.tagId << ", " << tag3.tagName << endl;
	cout << "tag4: " << tag4.tagId << ", " << tag4.tagName << endl;
	
	// ----- TagPersistence -----
	TagPersistence tagp(conn);
	
	// save()
	tagp.save(tag2);
	cout << "tag2 saved: " << tag2.tagId << ", " << tag2.tagName << endl;
	
	// load()
	Tag tag5 = tagp.load(tag2.tagId);
	cout << "tag5 loaded from tag2: " << tag5.tagId << ", " << tag5.tagName << endl;
	tag5 = tagp.load(42); // load non-existent tagId
	cout << "tag5 load non-existent: " << tag5.tagId << ", " << tag5.tagName << endl;

	// erase()
	tagp.erase(tag2.tagId);
	cout << "tag2 erased" << endl;
	tag5 = tagp.load(tag2.tagId);
	cout << "tag5 loading erased tag: " << tag5.tagId << ", " << tag5.tagName << endl;

	//tag1 = tm.addTag(tag1);
	//cout << "tm.addTag: " << tag1.tagId << ", " << tag1.tagName << endl;
	//cout << "tm.hasTag: " << tm.hasTag(tag1.tagId) << endl;
	return 0;
}