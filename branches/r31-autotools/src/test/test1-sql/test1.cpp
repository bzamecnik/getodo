#include "test1.h"

using namespace getodo;

int main(int argc, char argv[]) {
	using namespace std;
	TaskManager tm("test1.db");
	cout << "TaskManager created" << endl;
	
	Tag tag1;
	Tag tag2("photos");
	Tag tag3(tag2);
	Tag tag4(42, "photos");
	cout << "tag1: " << tag1.tagId << ", " << tag1.tagName << endl;
	cout << "tag2: " << tag2.tagId << ", " << tag2.tagName << endl;
	cout << "tag3: " << tag3.tagId << ", " << tag3.tagName << endl;
	cout << "tag4: " << tag4.tagId << ", " << tag4.tagName << endl;
	
	//tag1 = tm.addTag(tag1);
	//cout << "tm.addTag: " << tag1.tagId << ", " << tag1.tagName << endl;
	//cout << "tm.hasTag: " << tm.hasTag(tag1.tagId) << endl;
	return 0;
}