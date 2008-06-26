// $Id:$
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

#include "getodo.h"

namespace getodo {

class Tag {
	id_t tagId;
	std::string tagName;
public:
	Tag(std::string tagName);
	~Tag();

	id_t getTagId();

	std::string getTagName();
	void setTagName(const std::string tagName);
};

class TagPersistence {
	sqlite3x::sqlite3_connection* db;
public:
	// Constructor for loading new Tags
	TagPersistence(sqlite3x::sqlite3_connection* db);

	// save Tag to database
	void save(const Tag& tag);
	// load Tag from database
	Tag& load(id_t tagId);
	
	void setTagName(const std::string tagName);
};

} // namespace getodo

#endif // LIBGETODO_TAG_H
