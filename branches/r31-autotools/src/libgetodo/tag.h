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

using namespace sqlite3x;

namespace getodo {

class Tag {
public:
	id_t tagId; // -1, if not already in database
	std::string tagName;

	Tag();
	Tag(const Tag& t);
	Tag(std::string tagName);
	Tag(id_t tagId, std::string tagName);
	virtual ~Tag();
};

class TagPersistence {
	sqlite3_connection* conn;
public:
	// constructor for loading new Tags
	TagPersistence(sqlite3_connection* conn);
	virtual ~TagPersistence();

	// save Tag to database
	// - if it has no tagId (eg. it was newly created), assign some
	Tag& save(const Tag& tag);
	// load Tag from database
	Tag& load(id_t tagId);
	// delete Tag from database
	void erase(id_t tagId);
};

} // namespace getodo

#endif // LIBGETODO_TAG_H
