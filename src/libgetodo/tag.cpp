// $Id:$
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
Tag::Tag(std::string tagName) : tagId(-1) {
	this->tagName = tagName;
}
Tag::Tag(id_t tagId, std::string tagName) {
	this->tagId = tagId;
	this->tagName = tagName;
}

// ----- class TagPersistence --------------------

TagPersistence::TagPersistence(sqlite3_connection* conn) {
	this->conn = conn;
}

Tag& TagPersistence::save(Tag& tag) {
	// if(!conn) { TODO: throw ...}
	int count = 0;
	if (tag.tagId >= 0) {
		// find out, if there is already a tag with such tagID
		sqlite3_command cmd(*conn, "SELECT count(*) FROM Tag WHERE tagId = (?);");
		cmd.bind(1, tag.tagId);
		count = cmd.executeint();
	}
	if (count > 0) {
		// it is already there -> update
		sqlite3_command cmd(*conn, "UPDATE Tag SET tagName = ? WHERE tagId = (?);");
		cmd.bind(1, tag.tagName);
		cmd.bind(2, tag.tagId);
		cmd.executenonquery();
	} else {
		// it is not there -> insert
		sqlite3_command cmd(*conn, "INSERT INTO Tag (tagName) VALUES (?);");
		cmd.bind(1, tag.tagName);
		cmd.executenonquery();
		// get tagId which database automatically created
		tag.tagId = sqlite3_last_insert_rowid(conn->db());
	}
	return tag;
}

Tag& TagPersistence::load(id_t tagId) {
	// if(!conn) { TODO: throw ...}
	
	sqlite3_command cmd(*conn, "SELECT tagName FROM Tag WHERE tagId = (?);");
	cmd.bind(1, tagId);
	std::string tagName = cmd.executestring();
	// TODO: throw, if there is not record  with this tagID
	return *(new Tag(tagId, tagName));
}

} // namespace getodo
