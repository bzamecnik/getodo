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

std::string Tag::toString() {
	std::ostringstream ss;
	ss << "Tag [" << id << "]: " << name;
	return ss.str();
}

// ----- class TagPersistence --------------------

TagPersistence::TagPersistence(sqlite3_connection* c) : conn(c) {}

TagPersistence::~TagPersistence() {}

void TagPersistence::save(Tag& tag) {
	// if(!conn) { TODO: throw ...}
	int count = 0;
	if (tag.id >= 0) {
		// find out if there is already a tag with such a tagID
		sqlite3_command cmd(*conn, "SELECT count(*) FROM Tag WHERE tagId = ?;");
		cmd.bind(1, tag.id);
		count = cmd.executeint();
	} else {
		sqlite3_command cmd(*conn, "SELECT tagID FROM Tag WHERE tagName = ?;");
		cmd.bind(1, tag.name);
		sqlite3_cursor cursor = cmd.executecursor();
		if (cursor.step()) {
			// if there is already a tag with such a name, set the
			// tag.id respectively
			tag.id = cursor.getint(0);
			cursor.close();
			return;
		}
		cursor.close();
	}
	if (count > 0) {
		if (tag.id >= 0) {
			// it is already there -> update
			sqlite3_command cmd(*conn, "UPDATE Tag SET tagName = ? WHERE tagId = ?;");
			cmd.bind(1, tag.name);
			cmd.bind(2, tag.id);
			cmd.executenonquery();
		}
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
}

Tag& TagPersistence::load(id_t id) {
	// if(!conn) { TODO: throw ...}
	
	sqlite3_command cmd(*conn, "SELECT tagName FROM Tag WHERE tagId = ?;");
	cmd.bind(1, id);
	//std::string name = cmd.executestring(); // throws if the tag doesn't exist
	sqlite3_cursor cursor = cmd.executecursor();
	std::string name;
	if (cursor.step()) {
		name = cursor.getstring(0);
	}
	cursor.close();
	// TODO: throw, if there is no record with such a tagID
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
