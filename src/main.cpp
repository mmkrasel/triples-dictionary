//============================================================================
// Name        : main.cpp
// Author      : Rasel
// Version     :
// Copyright   : Your copyright notice
// Description : TripleDictionary in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include "Common.h"
#include "TripleTable.h"
#include "Dictionary.h"
#include <vector>
#include <regex>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <sys/stat.h>


#pragma pack(push, 1)

using namespace std;
using namespace rapidjson;

// Create a dictionary from a given triple dataset
// ./TD -d ./TT -d path_to_source_NT_dataset_file path_to_triple_tree_directory

ComponentId getSubObjId(Dictionary* dic, string searchFor){
	//searchFor.erase(remove(searchFor.begin(), searchFor.end(), '<'), searchFor.end());
	//searchFor.erase(remove(searchFor.begin(), searchFor.end(), '>'), searchFor.end());
	searchFor.erase(remove(searchFor.begin(), searchFor.end(), '"'), searchFor.end());
	//cout << searchFor << endl;
	return dic->getSubObjId(&searchFor[0]);
}
ComponentId getPredicateId(Dictionary* dic, string searchFor){
	//searchFor.erase(remove(searchFor.begin(), searchFor.end(), '<'), searchFor.end());
	//searchFor.erase(remove(searchFor.begin(), searchFor.end(), '>'), searchFor.end());
	searchFor.erase(remove(searchFor.begin(), searchFor.end(), '"'), searchFor.end());
	//cout << searchFor << endl;
	return dic->getPredicateId(&searchFor[0]);
}
bool createDirTree(const std::string full_path){
    size_t pos = 0;
    bool ret_val = true;
    while(ret_val == true && pos != std::string::npos){
        pos = full_path.find('/', pos + 1);
        if(mkdir(full_path.substr(0, pos).c_str(), 755) == -1){
        	ret_val = false;
        }
    }
    return ret_val;
}

int main(int argc, char** argv) {
	// cout << "#params: " << argc << endl;
	if(argc > 1){
		if(argv[1][1] == 'd' && argc == 4){
			Dictionary dic;
			createDirTree(argv[3]);
			dic.create(argv[2], argv[3]);
			dic.flushDictionary();
			dic.close();
		}
		else if(argv[1][1] == 't' && argc == 4){
			cout << "Loading Dictionary..." << endl;
			Dictionary dic;
			dic.open(argv[2]);
			cout << "Loading Tertiary Table..." << endl;
			cout << dic.getTriple(atoi(argv[3])) << endl;
			dic.close();
		}
		else if(argv[1][1] == 'q' && argc == 6){			// q: returns with triple ids / component ids
			cout << "Loading Dictionary..." << endl;
			Dictionary dic;
			dic.open(argv[2]);
			cout << "Loading Tertiary Table..." << endl;
			//exit(1);
			TripleItem* results = NULL;
			uint32 resultCount;
			// pattern as s p o
			char* s = argv[argc-3];
			char* p = argv[argc-2];
			char* o = argv[argc-1];
			if(p[0]=='x' && s[0] == 'x' && o[0]=='x'){  // ? ? ? => All triples
				results = dic.getTripleTable(&resultCount);			
			} else if(p[0]=='x' && o[0]=='x'){			// s ? ?
				results = dic.getTriplesForSubjectID(&resultCount, atoi(s));
			} else if(p[0]=='x' && s[0]=='x'){			// ? ? o
				results = dic.getTriplesForObjectID(&resultCount, atoi(o));
			} else if(p[0]=='x'){						// s ? o
				results = dic.getTriplesForSubObjIDs(&resultCount, atoi(s), atoi(o));
			}
			else if(s[0] == 'x' && o[0]=='x'){			// ? p ?
				results = dic.getTriplesForPredicateID(&resultCount, atoi(p));
			} else if(o[0]=='x'){						// s p ?
				results = dic.getTriplesForSubPredicateIDs(&resultCount, atoi(s), atoi(p));
			}
			else if(s[0]=='x'){							// ? p o
				results = dic.getTriplesForPredicateObjIDs(&resultCount, atoi(p), atoi(o));
			}
			else {										// s p o
				results = dic.doesExist(&resultCount, atoi(s), atoi(p), atoi(o));
			}
			for(uint32 i=0; i<resultCount; i++){
				cout << results[i].sub << "\t" << results[i].pred << "\t" << results[i].obj << endl;
			}
			cout << "#triples: " << resultCount << endl;
			dic.close();
		}
		else if(argv[1][1] == 's' && argc == 6){
			cout << "Loading Dictionary..." << endl;
			Dictionary dic;
			dic.open(argv[2]);
			cout << "Loading Tertiary Table..." << endl;
			TripleItem* results = NULL;
			uint32 resultCount;
			// pattern as s p o
			char* s = argv[argc-3];
			char* p = argv[argc-2];
			char* o = argv[argc-1];
			cout << "Start Searching..." << endl;
			if(p[0]=='x' && s[0] == 'x' && o[0]=='x'){  // ? ? ?
				results = dic.getTripleTable(&resultCount);
			} else if(p[0]=='x' && o[0]=='x'){			// s ? ?
				uint32 subId = getSubObjId(&dic, s);
				// printf("%d %s\n", subId, s);
				if(subId > 0){
					results = dic.getTriplesForSubjectID(&resultCount, subId);
				}
			} else if(p[0]=='x' && s[0]=='x'){			// ? ? o
				uint32 objId = getSubObjId(&dic, o);
				if(objId > 0){
					results = dic.getTriplesForObjectID(&resultCount, objId);
				}
			} else if(p[0]=='x'){						// s ? o
				uint32 subId = getSubObjId(&dic, s);
				if(subId > 0){
					uint32 objId = getSubObjId(&dic, o);
					if(objId > 0){
						results = dic.getTriplesForSubObjIDs(&resultCount, subId, objId);
					}
				}
			}
			else if(s[0] == 'x' && o[0]=='x'){			// ? p ?
				uint32 pid = getPredicateId(&dic, p);
				if(pid > 0){
					results = dic.getTriplesForPredicateID(&resultCount, pid);
				}
			} else if(o[0]=='x'){						// s p ?
				uint32 pid = getPredicateId(&dic, p);
				if(pid > 0){
					uint32 sid = getSubObjId(&dic, s);
					if(sid > 0){
						results = dic.getTriplesForSubPredicateIDs(&resultCount, sid, pid);
					}
				}
			}
			else if(s[0]=='x'){							// ? p o
				uint32 pid = getPredicateId(&dic, p);
				if(pid > 0){
					uint32 oid = getSubObjId(&dic, o);
					if(oid > 0){
						results = dic.getTriplesForPredicateObjIDs(&resultCount, pid, oid);
					}
				}
			}
			else {										// s p o
				uint32 pid = getPredicateId(&dic, p);
				if(pid > 0){
					uint32 oid = getSubObjId(&dic, o);
					if(oid > 0){
						uint32 sid = getSubObjId(&dic, s);
						if(sid > 0){
							results = dic.doesExist(&resultCount, sid, oid, pid);
						}
					}
				}
			}
			if(resultCount > 0){
				for(uint32 i=0; i<resultCount; i++){
					cout << dic.getSubObj(results[i].sub) << "\t" << dic.getPredicate(results[i].pred) << "\t" << dic.getSubObj(results[i].obj) << endl;
				}
			}
			cout << "#triples: " << resultCount << endl;
			dic.close();
		}
		else {
			cout << "Unknown action type" << endl;
		}
	} else {
		cout << "Please provide action type" << endl;
	}
	return 0;
}

