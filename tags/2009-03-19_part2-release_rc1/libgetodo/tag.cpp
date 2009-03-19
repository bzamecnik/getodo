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

Tag::Tag() : id(INVALID_ID) {}
Tag::Tag(const Tag& t) : id(t.id), name(t.name) {}
Tag::Tag(std::string tagName) : id(INVALID_ID), name(tagName) {}
Tag::Tag(id_t tagId, std::string tagName) : id(tagId), name(tagName) {}
Tag::~Tag() {}

std::string Tag::toString() const {
	return boost::lexical_cast<std::string, Tag>(*this);
}

std::ostream& operator<< (std::ostream& o, const Tag& tag) {
	// eg.: Tag [id]: "tag name"
	o << "Tag [" << tag.id << "]: \"" << tag.name << '"';
	return o;
}

bool Tag::isValidId(id_t id) {
	return id >= 0;
}

bool Tag::hasValidId() const {
	return Tag::isValidId(id);
}

// ----- class TagPersistence --------------------

TagPersistence::TagPersistence(sqlite3_connection* c) : conn(c) {}

TagPersistence::~TagPersistence() {}

id_t TagPersistence::insert(Tag& tag) {
	if (!conn) { 
		throw GetodoError("No database connection in the persistence.");
	}

	if (tag.name.empty()) { // exclude empty tags
		throw std::invalid_argument("Tag name empty");
	}

	if (!tag.hasValidId()) {
		sqlite3_command cmd(*conn, "SELECT tagId FROM Tag WHERE tagName = ?;");
		cmd.bind(1, tag.name);
		sqlite3_cursor cursor = cmd.executecursor();
		if (cursor.step()) {
			// if there is already a tag with such a name, set the
			// tag.id respectively
			tag.id = cursor.getint(0);
			cursor.close();
			return tag.id;
		}
		cursor.close();
	}
	// it is not there -> insert
	// id is defined NOT NULL, inserting NULL by not specifying
	// the value sets it to the ROWID (ie. it's auto-incremented)
	sqlite3_command cmd(*conn, "INSERT INTO Tag (tagName) VALUES (?);");
	cmd.bind(1, tag.name);
	cmd.executenonquery();
	// get the id which database automatically created
	tag.id = sqlite3_last_insert_rowid(conn->db());
	return tag.id;
}

void TagPersistence::update(const Tag& tag) {
	if (!conn) {
		throw GetodoError("No database connection in the persistence.");
	}
	if (!tag.hasValidId()) {
		throw std::invalid_argument("Invalid tag id: " + tag.id);
	}

	// UPDATE doesn't throw any error when given row doesn't exist
	sqlite3_command cmd(*conn, "UPDATE Tag SET tagName = ? WHERE tagId = ?;");
	cmd.bind(1, tag.name);
	cmd.bind(2, tag.id);
	cmd.executenonquery();
}

Tag TagPersistence::load(id_t id) {
	if (!conn) {
		throw GetodoError("No database connection in the persistence.");
	}
	
	sqlite3_command cmd(*conn, "SELECT tagName FROM Tag WHERE tagId = ?;");
	cmd.bind(1, id);
	std::string name;
	try {
		name = cmd.executestring(); // this throws if the tag doesn't exist
	} catch(sqlite3x::database_error ex) {
		throw GetodoError("No such a tag to load.");
	}
	return Tag(id, name);
}

void TagPersistence::erase(id_t id) {
	if (!conn) {
		throw GetodoError("No database connection in the persistence.");
	}
	
	sqlite3_command cmd(*conn, "DELETE FROM Tagged WHERE tagId = ?;");
	cmd.bind(1, id);
	cmd.executenonquery();
	
	cmd.prepare("DELETE FROM Tag WHERE tagId = ?;");
	cmd.bind(1, id);
	cmd.executenonquery();
}

} // namespace getodo
