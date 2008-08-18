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

Tag::Tag() : tagId(-1) {}
Tag::Tag(const Tag& t) : tagId(t.tagId), tagName(t.tagName) {}
Tag::Tag(std::string name) : tagId(-1), tagName(name) {}
Tag::Tag(id_t id, std::string name) : tagId(id), tagName(name) {}
Tag::~Tag() {}

// ----- class TagPersistence --------------------

TagPersistence::TagPersistence(sqlite3_connection* c) : conn(c) {}

TagPersistence::~TagPersistence() {}

void TagPersistence::save(Tag& tag) {
	// if(!conn) { TODO: throw ...}
	int count = 0;
	if (tag.tagId >= 0) {
		// find out if there is already a tag with such a tagID
		sqlite3_command cmd(*conn, "SELECT count(*) FROM Tag WHERE tagId = ?;");
		cmd.bind(1, tag.tagId);
		count = cmd.executeint();
	} else {
		sqlite3_command cmd(*conn, "SELECT tagID FROM Tag WHERE tagName = ?;");
		cmd.bind(1, tag.tagName);
		sqlite3_cursor cursor = cmd.executecursor();
		if (cursor.step()) {
			// if there is already a tag with such tagName, set the
			// tag.tagId respectively
			tag.tagId = cursor.getint(0);
			cursor.close();
			return;
		}
		cursor.close();
	}
	if (count > 0) {
		if (tag.tagId >= 0) {
			// it is already there -> update
			sqlite3_command cmd(*conn, "UPDATE Tag SET tagName = ? WHERE tagId = ?;");
			cmd.bind(1, tag.tagName);
			cmd.bind(2, tag.tagId);
			cmd.executenonquery();
		}
	} else {
		// it is not there -> insert
		// tagId is defined NOT NULL, inserting NULL by not specifying
		// the value sets it to the ROWID (ie. it's auto-incremented)
		sqlite3_command cmd(*conn, "INSERT INTO Tag (tagName) VALUES (?);");
		cmd.bind(1, tag.tagName);
		cmd.executenonquery();
		// get the tagId which database automatically created
		tag.tagId = sqlite3_last_insert_rowid(conn->db());
	}
}

Tag& TagPersistence::load(id_t tagId) {
	// if(!conn) { TODO: throw ...}
	
	sqlite3_command cmd(*conn, "SELECT tagName FROM Tag WHERE tagId = ?;");
	cmd.bind(1, tagId);
	//std::string tagName = cmd.executestring(); // throws if the tag doesn't exist
	sqlite3_cursor cursor = cmd.executecursor();
	std::string tagName;
	if (cursor.step()) {
		tagName = cursor.getstring(0);
	}
	cursor.close();
	// TODO: throw, if there is no record with such a tagID
	return *(new Tag(tagId, tagName));
}

void TagPersistence::erase(id_t tagId) {
	// if(!conn) { TODO: throw ...}
	
	sqlite3_command cmd(*conn, "DELETE FROM Tagged WHERE tagId = ?;");
	cmd.bind(1, tagId);
	cmd.executenonquery();
	
	cmd.prepare("DELETE FROM Tag WHERE tagId = ?;");
	cmd.bind(1, tagId);
	cmd.executenonquery();
}

} // namespace getodo
