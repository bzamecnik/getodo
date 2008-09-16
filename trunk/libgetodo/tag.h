// $Id$
//
// C++ Interface: Tag
//
// Description: 
//
//
// Author: Bohumir Zamecnik <bohumir@zamecnik.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef LIBGETODO_TAG_H
#define LIBGETODO_TAG_H

#include "common.h"
#include <sstream>

using namespace sqlite3x;

namespace getodo {

class Tag {
public:
	id_t id; // -1, if not already in database
	std::string name;

	Tag();
	Tag(const Tag& t);
	Tag(std::string name);
	Tag(id_t tagId, std::string tagName);
	virtual ~Tag();

	std::string toString() const;
	friend std::ostream& operator<< (std::ostream& o, const Tag& tag);
};

class TagPersistence {
	sqlite3_connection* conn;
public:
	// constructor for loading new Tags
	TagPersistence(sqlite3_connection* conn);
	virtual ~TagPersistence();

	// save Tag to database and assign an id
	// true, if really inserted
	bool insert(Tag& tag);
	// update existing Tag (tag.id must be > 0)
	void update(Tag& tag);
	// load Tag from database
	Tag& load(id_t id);
	// delete Tag from database
	void erase(id_t id);
};

} // namespace getodo

#endif // LIBGETODO_TAG_H
