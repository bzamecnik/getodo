#include "test1.h"

using namespace getodo;

// tip: use Boost.Test http://www.boost.org/doc/libs/1_35_0/libs/test/doc/index.html

int main(int argc, char argv[]) {
	using namespace std;
	
	cout << "----- DateTime -----" << endl;

	DateTime dt1;
	cout << "DateTime dt1: " << dt1.toString() << endl;
	DateTime dt2 = DateTime::now();
	cout << "DateTime dt2 now(), copy: " << dt2.toString() << endl;
	DateTime dt3 = DateTime(dt2.toString());
	cout << "DateTime dt3 from string: " << dt3.toString() << endl;

	cout << "----- Date -----" << endl;

	Date date1;
	cout << "Date date1: " << date1.toString() << endl;
	Date date2 = Date::now();
	cout << "Date date2 now(), copy: " << date2.toString() << endl;
	Date date3 = Date(date2.toString());
	cout << "Date date3 from string: " << date3.toString() << endl;

	try {
		TaskManager tm("test1.db");
		sqlite3_connection *conn = tm.getConnection();
		cout << "TaskManager created" << endl;
		
		cout << "----- Tag -----" << endl;

		Tag tag1;
		Tag tag2("photos");
		Tag tag3(tag2);
		Tag tag4(123, "photos");
		cout << "tag1: " << tag1.toString() << endl;
		cout << "tag2: " << tag2.toString() << endl;
		cout << "tag3: " << tag3.toString() << endl;
		cout << "tag4: " << tag4.toString() << endl;
		
		cout << "----- TagPersistence -----" << endl;

		TagPersistence tagp(conn);
		
		// save()
		tagp.save(tag2);
		cout << "tag2 saved: " << tag2.toString() << endl;
		
		// load()
		Tag tag5 = tagp.load(tag2.id);
		cout << "tag5 loaded from tag2: " << tag5.toString() << endl;
		tag5 = tagp.load(42); // load non-existent tagId
		cout << "tag5 load non-existent: " << tag5.toString() << endl;

		// erase()
		tagp.erase(tag2.id);
		cout << "tag2 erased" << endl;
		tag5 = tagp.load(tag2.id);
		cout << "tag5 loading erased tag: " << tag5.toString() << endl;

		//tag1 = tm.addTag(tag1);
		//cout << "tm.addTag: " << tag1.tagId << ", " << tag1.tagName << endl;
		//cout << "tm.hasTag: " << tm.hasTag(tag1.tagId) << endl;


		cout << "----- FilterRule -----" << endl;

		FilterRule rule1;
		FilterRule rule2("rule name","filter rule");
		FilterRule rule3(rule2);
		FilterRule rule4(123, "other rule name"," other filter rule");
		cout << "rule1: " << rule1.toString() << endl;
		cout << "rule2: " << rule2.toString() << endl;
		cout << "rule3: " << rule3.toString() << endl;
		cout << "rule4: " << rule4.toString() << endl;
		
		cout << "----- FilterRulePersistence -----" << endl;

		FilterRulePersistence rulep(conn);
		
		// save()
		rulep.save(rule2);
		cout << "rule2 saved: " << rule2.toString() << endl;
		
		// load()
		FilterRule rule5;
		rulep.load(rule5, rule2.id);
		cout << "rule5 loaded from rule2: " << rule5.toString() << endl;
		rulep.load(rule5, 42); // load non-existent ruleId
		cout << "rule5 load non-existent: " << rule5.toString() << endl;

		// setName()
		rulep.setName(rule2, "modified rule name");
		cout << "rule2 setName: " << rule2.toString() << endl;

		// setRule()
		rulep.setRule(rule2, "modified filter rule");
		cout << "rule2 setRule: " << rule2.toString() << endl;

		// erase()
		rulep.erase(rule2.id);
		cout << "rule2 erased" << endl;
		rulep.load(rule5, rule2.id);
		cout << "rule5 loading erased rule: " << rule5.toString() << endl;

	} catch (database_error e) {
		cout << e.what() << endl;
	}
	return 0;
}
