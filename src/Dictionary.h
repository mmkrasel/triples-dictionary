/*
 * Dictionary.h
 *
 *  Created on: 9 Dec 2023
 *      Author: rasel
 */

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include <boost/regex.hpp>
#include "TripleTable.h"
#include "ComponentTree.h"

using namespace std;


class Dictionary {
public:
	char* inputPath;
	char* pathDir;
	bool LOG;
	ComponentTree so, p;
	TripleTable tripleTable;
	Common common;
	char* uri = NULL;
	//
	Dictionary();
	virtual ~Dictionary();
	void stats();
	void create(char* inputPath, char* outputDirectory);
	ComponentId getSubObjId(char* uri);
	ComponentId getPredicateId(char* uri);
	char* getSubObj(ComponentId id);
	char* getPredicate(ComponentId id);
	string getTriple(uint32 id);
	TripleItem* getTripleTable(uint32* resultCount);

	TripleItem* getTriplesForSubjectID(uint32* resultCount, uint32 subId);
	TripleItem* getTriplesForObjectID(uint32* resultCount, uint32 objId);
	TripleItem* getTriplesForSubObjIDs(uint32* resultCount, uint32 subId, uint32 objId);
	TripleItem* getTriplesForPredicateID(uint32* resultCount, uint32 subId);
	TripleItem* getTriplesForSubPredicateIDs(uint32* resultCount, uint32 subId, uint32 predId);
	TripleItem* getTriplesForPredicateObjIDs(uint32* resultCount, uint32 predId, uint32 objId);
	TripleItem* doesExist(uint32* resultCount, uint32 subId, uint32 predId, uint32 objId);

	void flushDictionary();
	void open(char* datasetPath);
	void close();
};

#endif /* DICTIONARY_H_ */
