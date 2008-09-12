// $Id$
//
// C++ Implementation: tag
//
// Description: 
//
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "tag.h"

using namespace sqlite3x;

namespace getodo {

// ----- class Tag --------------------

Tag::Tag() : id(-1) {}
Tag::Tag(const Tag& t) : id(t.id), name(t.name) {}
Tag::Tag(std::string tagName) : id(-1), name(tagName) {}
Tag::Tag(id_t tagId, std::string tagName) : id(tagId), name(tagName) {}
Tag::~Tag() {}

std::string Tag::toString() const {
	std::ostringstream ss;
	ss << "Tag [" << id << "]: " << name;
	return ss.str();
}

// ----- class TagPersistence --------------------

TagPersistence::TagPersistence(sqlite3_connection* c) : conn(c) {}

TagPersistence::~TagPersistence() {}

bool TagPersistence::insert(Tag& tag) {
	// if(!conn) { TODO: throw ...}
	if (tag.name.empty()) { return false; } // exclude empty tags

	int count = 0;
	if (tag.id >= 0) {
		// find out if there is already a tag with such a tagId
		sqlite3_command cmd(*conn, "SELECT count(*) FROM Tag WHERE tagId = ?;");
		cmd.bind(1, tag.id);
		count = cmd.executeint();
	} else {
		sqlite3_command cmd(*conn, "SELECT tagId FROM Tag WHERE tagName = ?;");
		cmd.bind(1, tag.name);
		sqlite3_cursor cursor = cmd.executecursor();
		if (cursor.step()) {
			// if there is already a tag with such a name, set the
			// tag.id respectively
			tag.id = cursor.getint(0);
			cursor.close();
			return false;
		}
		cursor.close();
	}
	if (count > 0) {
		//update(tag);
		return false;
	} else {
		// it is not there -> insert
		// id is defined NOT NULL, inserting NULL by not specifying
		// the value sets it to the ROWID (ie. it's auto-incremented)
		sqlite3_command cmd(*conn, "INSERT INTO Tag (tagName) VALUES (?);");
		cmd.bind(1, tag.name);
		cmd.executenonquery();
		// get the id which database automatically created
		tag.id = sqlite3_last_insert_rowid(conn->db());
	}
	return true;
}

void TagPersistence::update(Tag& tag) {
	// if(!conn) { TODO: throw ...}
	if (tag.id >= 0) {
		// UPDATE doesn't throw any error when given row doesn't exist
		sqlite3_command cmd(*conn, "UPDATE Tag SET tagName = ? WHERE tagId = ?;");
		cmd.bind(1, tag.name);
		cmd.bind(2, tag.id);
		cmd.executenonquery();
	}
	// else { // throw }
}

Tag& TagPersistence::load(id_t id) {
	// if(!conn) { TODO: throw ...}
	
	sqlite3_command cmd(*conn, "SELECT tagName FROM Tag WHERE tagId = ?;");
	cmd.bind(1, id);
	//std::string name = cmd.executestring(); // throws if the tag doesn't exist
	sqlite3_cursor cursor = cmd.executecursor();
	std::string name;
	if (cursor.step()) {
		if (!cursor.isnull(0)) {
			name = cursor.getstring(0);
		}
	}
	cursor.close();
	// TODO: throw, if there is no record with such a tagId
	return *(new Tag(id, name));
}

void TagPersistence::erase(id_t id) {
	// if(!conn) { TODO: throw ...}
	
	sqlite3_command cmd(*conn, "DELETE FROM Tagged WHERE tagId = ?;");
	cmd.bind(1, id);
	cmd.executenonquery();
	
	cmd.prepare("DELETE FROM Tag WHERE tagId = ?;");
	cmd.bind(1, id);
	cmd.executenonquery();
}

} // namespace getodo