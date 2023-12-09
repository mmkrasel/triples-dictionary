/*
 * Dictionary.cpp
 *
 *  Created on: 9 Dec 2023
 *      Author: rasel
 */

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Common.h"
#include "TripleTable.h"
#include "ComponentTree.h"
#include "Dictionary.h"
#include <regex>
#include <regex.h>

using namespace std;

Dictionary::Dictionary() {
	pathDir = NULL;
	inputPath = NULL;
	LOG = false;
}
Dictionary::~Dictionary() {}


void Dictionary::stats(){
	cout << tripleTable.NUM_TRIPLES << "\t" << so.getStats() << "\t" << p.getStats() << endl;
}


void Dictionary::create(char* inputPath, char* outputDirectory) {
	int pathLen = strlen(outputDirectory);
	pathDir = new char[pathLen+1];
	memcpy(&this->pathDir[0], &outputDirectory[0], pathLen);
	pathDir[pathLen] = '\0';
	pathLen = strlen(inputPath);
	this->inputPath = new char[pathLen+1];
	memcpy(&this->inputPath[0], &inputPath[0], pathLen);
	this->inputPath[pathLen] = '\0';

	so.init(outputDirectory, (char*)"so");
	p.init(outputDirectory, (char*)"p");
	tripleTable.init(outputDirectory, true);

    regex_t uri_regex, literal_regex, literal_regex_type;
    regcomp(&uri_regex, "^<.*>$", REG_EXTENDED);
    regcomp(&literal_regex, "^\".*\"(@[a-zA-Z]+)*$", REG_EXTENDED);
    regcomp(&literal_regex_type, "\"(.+)\"\\^\\^(.+)", REG_EXTENDED);
    regmatch_t matches[2];
	char** properties = (char**)malloc(sizeof(char*)*4);
	properties[0] = (char*) malloc(common.URI_MAX_LENGTH);
	properties[1] = (char*) malloc(common.URI_MAX_LENGTH);
	properties[2] = (char*) malloc(common.URI_MAX_LENGTH);
	properties[3] = (char*) malloc(common.URI_MAX_LENGTH);
	short uriCount = 0;
	TripleId tripleCount = 0;
	TripleId linesCount = 0;
	uint32 subId, objId;
	uint32 predId;
	char c, tmp;
	int index = 0;
	common.startClock();
	ifstream is(inputPath);
	int divisor = 100000;
	while(is.get(c)){
		if(uriCount==0){
			if(c==' '){
				tmp = properties[uriCount][index-2];
				properties[uriCount][index] = '\0';
			    int uri_matched = regexec(&uri_regex, properties[uriCount], 2, matches, 0);
			    if (uri_matched == 0) {
			    	subId = so.encodeUri(properties[uriCount]);
				    uriCount++;
					index = 0;
					continue;
			    } else {
			    	string str = properties[uriCount];
			    	if (str.find('"') != string::npos) {
			    	} else {
			    	    // shift existing elements to the right
			    	    for (int i = index-1; i >= 0; i--) {
			    	    	properties[uriCount][i+1] = properties[uriCount][i];
			    	    }
			    	    // insert new element at the beginning
			    	    properties[uriCount][0] = '"';
			    	    properties[uriCount][index+1] = '"';
			    	    properties[uriCount][index+2] = '\0';
			    	}
				    int literal_matched = regexec(&literal_regex, properties[uriCount], 2, matches, 0);
			    	if (literal_matched == 0) {
						subId = so.encodeLiteral(properties[uriCount]);
						uriCount++;
						index = 0;
						continue;
					} else {
						properties[uriCount][index-2] = tmp;
						properties[uriCount][index] = '\0';
						cout << index<<"\n\n\nNO S: " << properties[uriCount] << endl << endl << endl;
						exit(1);
					}
			    }
			}
		} else if (uriCount==1){
			if(c==' '){
				tmp = properties[uriCount][index-2];
				properties[uriCount][index] = '\0';
			    int uri_matched = regexec(&uri_regex, properties[uriCount], 2, matches, 0);
			    if (uri_matched == 0) {
			    	predId = p.encodeUri(properties[uriCount]);
				    uriCount++;
					index = 0;
					continue;
			    } else {
			    	string str = properties[uriCount];
			    	if (str.find('"') != string::npos) {
			    	} else {
			    	    // shift existing elements to the right
			    	    for (int i = index-1; i >= 0; i--) {
			    	    	properties[uriCount][i+1] = properties[uriCount][i];
			    	    }
			    	    // insert new element at the beginning
			    	    properties[uriCount][0] = '"';
			    	    properties[uriCount][index+1] = '"';
			    	    properties[uriCount][index+2] = '\0';
			    	}
				    int literal_matched = regexec(&literal_regex, properties[uriCount], 2, matches, 0);
			    	if (literal_matched == 0) {
			    		predId = p.encodeLiteral(properties[uriCount]);
						uriCount++;
						index = 0;
						continue;
					}else {
						properties[uriCount][index-2] = tmp;
						properties[uriCount][index] = '\0';
						cout << index<<"\n\n\nNO P: " << properties[uriCount] << endl << endl << endl;
						exit(1);
					}
			    }
			}
		} else {
			if(c=='\n'){
				linesCount++;
				tmp = properties[uriCount][index-2];
				properties[uriCount][index-2] = '\0';
			    int uri_matched = regexec(&uri_regex, properties[uriCount], 2, matches, 0);
			    if (uri_matched == 0) {
			    	objId = so.encodeUri(properties[uriCount]);
			    	tripleCount++;
			    	//printf("@Dictionary::create 1 *** %d %d %d\n", subId, predId, objId);
			    	tripleTable.add(subId, predId, objId);
					uriCount = 0;
					index = 0;
					if(tripleCount%divisor==0){
						this->stats();
						//cout << linesCount << " " << tripleCount  << "."  << properties[0] << " " << properties[1] << " " << properties[2] << endl;
					}
					continue;
			    } else {
				    int literal_matched = regexec(&literal_regex, properties[uriCount], 2, matches, 0);
			    	if (literal_matched == 0) {
						objId = so.encodeLiteral(properties[uriCount]);
						//printf("@Dictionary::create 2 *** %d %d %d\n", subId, predId, objId);
						tripleTable.add(subId, predId, objId);
			    		tripleCount++;
						uriCount = 0;
						index = 0;
						if(tripleCount%divisor==0){
							this->stats();
							//cout << linesCount << " 1 " << tripleCount <<  "."  << properties[0] << " " << properties[1] << " " << properties[2] << endl;
						}
						continue;
					}else {
						literal_matched = regexec(&literal_regex_type, properties[uriCount], 2, matches, 0);
						if (literal_matched == 0) {
							objId = so.encodeLiteral(properties[uriCount]);
							//printf("@Dictionary::create 3 *** %d %d %d\n", subId, predId, objId);
							tripleTable.add(subId, predId, objId);
							tripleCount++;
							uriCount = 0;
							index = 0;
							if(tripleCount%divisor==0){
								this->stats();
								//cout << linesCount << " 2 " << tripleCount  << "."  << properties[0] << " " << properties[1] << " " << properties[2] << endl;
							}
							continue;
						}
						else if (properties[uriCount][index-3] == '@') {
							objId = so.encodeLiteral(properties[uriCount]);
							//printf("@Dictionary::create 4 *** %d %d %d\n", subId, predId, objId);
							tripleTable.add(subId, predId, objId);
							tripleCount++;
							uriCount = 0;
							index = 0;
							if(tripleCount%divisor==0){
								this->stats();
								//cout << linesCount << " 3 " << tripleCount  << "."  << properties[0] << " " << properties[1] << " " << properties[2] << endl;
							}
							continue;
						}
						else if (index > 1024*5) {
					    	string str = properties[uriCount];
					    	if (str.find('"') != string::npos) {
					    	} else {
					    	    // shift existing elements to the right
					    	    for (int i = index-1; i >= 0; i--) {
					    	    	properties[uriCount][i+1] = properties[uriCount][i];
					    	    }
					    	    // insert new element at the beginning
					    	    properties[uriCount][0] = '"';
					    	    properties[uriCount][index+1] = '"';
					    	    properties[uriCount][index+2] = '\0';
					    	}
							objId = so.encodeLiteral(properties[uriCount]);
							//printf("@Dictionary::create 5 *** %d %d %d\n", subId, predId, objId);
							tripleTable.add(subId, predId, objId);
							tripleCount++;
							uriCount = 0;
							index = 0;
							if(tripleCount%divisor==0){
								this->stats();
								//cout << linesCount << " 4 " << tripleCount  << "."  << properties[0] << " " << properties[1] << " " << properties[2] << endl;
							}
							continue;
						}
						else if (properties[uriCount][index-3] != '@') {
					    	string str = properties[uriCount];
					    	if (str.find('"') != string::npos) {
					    	} else {
					    	    // shift existing elements to the right
					    	    for (int i = index-2; i >= 0; i--) {
					    	    	properties[uriCount][i+1] = properties[uriCount][i];
					    	    }
					    	    // insert new element at the beginning
					    	    properties[uriCount][0] = '"';
					    	    properties[uriCount][index-1] = '"';
					    	    properties[uriCount][index] = '\0';
					    	}
							objId = so.encodeLiteral(properties[uriCount]);
							//printf("@Dictionary::create 6 *** %d %d %d\n", subId, predId, objId);
							tripleTable.add(subId, predId, objId);
							tripleCount++;
							uriCount = 0;
							index = 0;
							if(tripleCount%divisor==0){
								this->stats();
								//cout << linesCount << " 5 " << tripleCount <<  "."  << properties[0] << " " << properties[1] << " " << properties[2] << endl;
							}
							continue;
						}
						else {
							properties[uriCount][index-2] = tmp;
							properties[uriCount][index] = '\0';
							cout << index<<"\n\n\nNO O: " << properties[uriCount] << endl << endl << endl;
							cout << linesCount << " " << tripleCount << "."  << properties[0] << " " << properties[1] << " " << properties[2] << endl;
							exit(1);
							//countDown = 2;
						}
					}
				}
			} else if(index>5 && properties[uriCount][index-1]=='.' && properties[uriCount][index-2]==' ' && ((properties[uriCount][index-3]=='@' && properties[uriCount][index-4]=='"')||(properties[uriCount][index-5]=='@' && properties[uriCount][index-6]=='"'))){
				properties[uriCount][index-2] = '\0';
				objId = so.encodeLiteral(properties[uriCount]);
				//printf("@Dictionary::create 7 *** %d %d %d\n", subId, predId, objId);
				tripleTable.add(subId, predId, objId);
				tripleCount++;
				uriCount = 0;
				index = 0;
				if(tripleCount%divisor==0){
					this->stats();
					//cout << linesCount << " " << tripleCount  << "."  << properties[0] << " " << properties[1] << " " << properties[2] << endl;
				}
				//continue;
			}
		}// else
		if(index < common.URI_MAX_LENGTH) {
			properties[uriCount][index++] = c;
		} else {
			cout << index << ": " << properties[uriCount] << endl;
			exit(1);
		}
	}
	is.close();
    regfree(&uri_regex);
    regfree(&literal_regex);
    regfree(&literal_regex_type);
    //exit(1);

	cout << "#Lines: " << linesCount;
	cout << "; #Triples: " << tripleCount;
	cout << "; Elapsed Time: " << common.getElapsedTime() << endl;
}

void Dictionary::flushDictionary(){
	unsigned long dicSize = so.flush(pathDir, (char*)"so");
	dicSize += p.flush(pathDir, (char*)"p");
	unsigned long ttSize = tripleTable.flushBuffer();
	unsigned long inSize = common.getFileSize(inputPath);
	printf("Dictionary: %lu\n", dicSize);
	printf("TriplTable: %lu\n", ttSize);
	printf("Dic+TTable: %lu\n", dicSize+ttSize);
	printf("Input Size: %lu\n", inSize);
	printf("Ratio IvsO: %f\n", (dicSize+ttSize)*100.0/inSize);
	delete[] this->pathDir;
	delete[] this->inputPath;
}
void Dictionary::open(char* datasetPath){
	tripleTable.init(datasetPath, false);
	tripleTable.load();
	p.open(datasetPath, (char*)"p");
	so.open(datasetPath, (char*)"so");
}
void Dictionary::close(){
	p.close();
	so.close();
	tripleTable.close();
	delete[] uri;
}
ComponentId Dictionary::getSubObjId(char* uri){
	//cout << uri << endl;
	return so.getComponentId(uri);
}
ComponentId Dictionary::getPredicateId(char* uri){
	return p.getComponentId(uri);
}
char* Dictionary::getPredicate(ComponentId id){
	return p.getComponent(id);
}
char* Dictionary::getSubObj(ComponentId id){
	return so.getComponent(id);
}

string Dictionary::getTriple(uint32 id){
	if(id < 1 || id >= tripleTable.NUM_TRIPLES){
		cout << id << ": " << tripleTable.NUM_TRIPLES << " ";
		return "Invalid triple id";
	}
	id--;
	ComponentId subId = tripleTable.cache[id].sub;
	ComponentId objId = tripleTable.cache[id].obj;
	ComponentId preId = tripleTable.cache[id].pred;
	// cout << subId << " " << preId << " " << objId << endl;
	stringstream urlStr;
	urlStr << so.getComponent(subId) << " " << p.getComponent(preId) << " " << so.getComponent(objId) << endl;
	return urlStr.str();
}
TripleItem* Dictionary::getTripleTable(uint32* resultCount){
	//results = tripleTable.cache;
	*resultCount = tripleTable.NUM_TRIPLES;
	return tripleTable.cache;
}
TripleItem* Dictionary::getTriplesForSubjectID(uint32* resultCount, uint32 subId){
	*resultCount = 0;
	TripleItem* results = (TripleItem*)malloc(sizeof(TripleItem));
	for(uint32 i=0; i<tripleTable.NUM_TRIPLES; i++){
		if(tripleTable.cache[i].sub == subId){
			results = (TripleItem*)realloc(results, ((*resultCount)+1)*sizeof(TripleItem));
			results[*resultCount].sub = tripleTable.cache[i].sub;
			results[*resultCount].pred = tripleTable.cache[i].pred;
			results[*resultCount].obj = tripleTable.cache[i].obj;
			(*resultCount)++;
		}
	}
	return results;
}
TripleItem* Dictionary::getTriplesForObjectID(uint32* resultCount, uint32 objId){
	*resultCount = 0;
	TripleItem* results = (TripleItem*)malloc(sizeof(TripleItem));
	for(uint32 i=0; i<tripleTable.NUM_TRIPLES; i++){
		if(tripleTable.cache[i].obj == objId){
			results = (TripleItem*)realloc(results, ((*resultCount)+1)*sizeof(TripleItem));
			results[*resultCount].sub = tripleTable.cache[i].sub;
			results[*resultCount].pred = tripleTable.cache[i].pred;
			results[*resultCount].obj = tripleTable.cache[i].obj;
			(*resultCount)++;
		}
	}
	return results;
}
TripleItem* Dictionary::getTriplesForPredicateID(uint32* resultCount, uint32 predId){
	*resultCount = 0;
	TripleItem* results = (TripleItem*)malloc(sizeof(TripleItem));
	for(uint32 i=0; i<tripleTable.NUM_TRIPLES; i++){
		if(tripleTable.cache[i].pred == predId){
			results = (TripleItem*)realloc(results, ((*resultCount)+1)*sizeof(TripleItem));
			results[*resultCount].sub = tripleTable.cache[i].sub;
			results[*resultCount].pred = tripleTable.cache[i].pred;
			results[*resultCount].obj = tripleTable.cache[i].obj;
			(*resultCount)++;
		}
	}
	return results;
}
TripleItem* Dictionary::getTriplesForSubObjIDs(uint32* resultCount, uint32 subId, uint32 objId){
	*resultCount = 0;
	TripleItem* results = (TripleItem*)malloc(sizeof(TripleItem));
	for(uint32 i=0; i<tripleTable.NUM_TRIPLES; i++){
		if(tripleTable.cache[i].sub == subId && tripleTable.cache[i].obj == objId){
			results = (TripleItem*)realloc(results, ((*resultCount)+1)*sizeof(TripleItem));
			results[*resultCount].sub = tripleTable.cache[i].sub;
			results[*resultCount].pred = tripleTable.cache[i].pred;
			results[*resultCount].obj = tripleTable.cache[i].obj;
			(*resultCount)++;
		}
	}
	return results;
}
TripleItem* Dictionary::getTriplesForSubPredicateIDs(uint32* resultCount, uint32 subId, uint32 predId){
	*resultCount = 0;
	TripleItem* results = (TripleItem*)malloc(sizeof(TripleItem));
	for(uint32 i=0; i<tripleTable.NUM_TRIPLES; i++){
		if(tripleTable.cache[i].sub == subId && tripleTable.cache[i].pred == predId){
			results = (TripleItem*)realloc(results, ((*resultCount)+1)*sizeof(TripleItem));
			results[*resultCount].sub = tripleTable.cache[i].sub;
			results[*resultCount].pred = tripleTable.cache[i].pred;
			results[*resultCount].obj = tripleTable.cache[i].obj;
			(*resultCount)++;
		}
	}
	return results;
}
TripleItem* Dictionary::getTriplesForPredicateObjIDs(uint32* resultCount, uint32 predId, uint32 objId){
	*resultCount = 0;
	TripleItem* results = (TripleItem*)malloc(sizeof(TripleItem));
	for(uint32 i=0; i<tripleTable.NUM_TRIPLES; i++){
		if(tripleTable.cache[i].obj == objId && tripleTable.cache[i].pred == predId){
			results = (TripleItem*)realloc(results, ((*resultCount)+1)*sizeof(TripleItem));
			results[*resultCount].sub = tripleTable.cache[i].sub;
			results[*resultCount].pred = tripleTable.cache[i].pred;
			results[*resultCount].obj = tripleTable.cache[i].obj;
			(*resultCount)++;
		}
	}
	return results;
}
TripleItem* Dictionary::doesExist(uint32* resultCount, uint32 subId, uint32 predId, uint32 objId){
	*resultCount = 0;
	TripleItem* results = (TripleItem*)malloc(sizeof(TripleItem));
	for(uint32 i=0; i<tripleTable.NUM_TRIPLES; i++){
		if(tripleTable.cache[i].sub == subId && tripleTable.cache[i].obj == objId && tripleTable.cache[i].pred == predId){
			results[*resultCount].sub = tripleTable.cache[i].sub;
			results[*resultCount].pred = tripleTable.cache[i].pred;
			results[*resultCount].obj = tripleTable.cache[i].obj;
		}
	}
	(*resultCount)++;
	return results;
}
