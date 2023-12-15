# triples-dictionary
Create a dictionary for RDF triples and access the triples through triples table

**# Dependencies**

	Packages: (Install boost and rapidjson library and keep it in the path of /usr/include/)
 		libboost-all-dev version 1.71.0
   		rapidjson-dev=1.1.0
	Compiler: g++ 
	Note: User **build.sh** to compile and build

## Use of binaries as an application

**#Create dictionary from triple datatsets**

	./TD -d path_to_source_NT_dataset_file td_dictionary

**#Search for triple pattern (with uri-component ids) in Triples Dictionary**

	./TD -q td_dictionary triple_pattern

		- triple_pattern: s p o
			- unbounded subject/predicate/object will be represented with x;   x x x
			- bounded subject/predicate/object is represented by their ID; such as 1 1 2, where 1, 1, and 2 represent the subject, predicate, and object ID, respectively.
			- Some patters: 
				x x x: Extracts all triples
				x 1 x: Extracts all triples with predicate id 1
				1 x x: Extracts all triples with subject id 1
				x x 2: Extracts all triples with object id 2
				1 1 x: Extracts all triples with predicate id 1 and subject id 1
				x 1 2: Extracts all triples with predicate id 1 and object id 2
				1 x 2: Extracts all triples with subject id 1 and object id 2
				1 1 2: Find the triple in triple tree

**#Example query: Retrieve all triples having object_id "2" and predicate_id "1".
 
    ./TD -q ../dbpedia/dictionary x 1 2	


**#Search for triple pattern (with uri) in Triples Dictionary**

	./TD -s td_dictionary triple_pattern

    - triple_pattern: s p o
      - unbounded subject/predicate/object will be represented with x;   x x x
      
      - Some patters: 
      
      - Extracts all triples 
        x x x
  
      - Extracts all triples with predicate "http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#telephone" 
          x http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#telephone x
          
      - Extracts all triples with subject "http://www.Department6.University81.edu/GraduateStudent33"
        http://www.Department6.University81.edu/GraduateStudent33 x x
        
      - Extracts all triples with predicate "http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#telephone" and subject "http://www.Department6.University81.edu/GraduateStudent33"
        http://www.Department6.University81.edu/GraduateStudent33 http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#telephone x



**#Example query: Retrieve all triples having subject "http://www.Department6.University81.edu/GraduateStudent33".**
		
    ./TD -s ../lubm/dictionary http://www.Department6.University81.edu/GraduateStudent33 x x


## Use source codes as a C++ library

**Create a dictionary from an input RDF triples dataset**
 
	Dictionary dic;
	dic.create("path_to_source_NT_dataset_file", "td_dictionary");
	dic.flushDictionary();
	dic.close();
	
**Queries on dictionay**
 
	/*Open the dictionary first*/
	Dictionary dic;
	dic.open("td_dictionary");
	unsigned int count;
	TripleItem* results = NULL;

	/* Get all triples, triple pattern: ? ? ? */
	results = dic.getTripleTable(&count);

	/* Get triples for pattern s ? ? */
	unsigned int sid = dic.getSubObjId("any uri here");
	if(sid > 0) results = dic.getTriplesForSubjectID(&count, sid);

	/* Get triples for pattern ? ? o */
	unsigned int oid = dic.getSubObjId("any uri here");
	if(oid > 0) results = dic.getTriplesForObjectID(&count, oid);

	/* Get triples for pattern ? p ? */
	unsigned int pid = dic.getPredicateId("any uri here");
	if(pid > 0) results = dic.getTriplesForPredicateID(&count, pid);

	/* Get triples for pattern s ? o */
 	....
	if(sid>0 && oid>0) results = dic.getTriplesForSubObjIDs(&count, sid, oid);

	/* Get triples for pattern s p ? */
 	....
	if(sid>0 && pid>0) results = dic.getTriplesForSubPredicateIDs(&count, sid, pid);

	/* Get triples for pattern ? p o */
 	....
	if(oid>0 && pid>0) results = dic.getTriplesForPredicateObjIDs(&count, pid, oid);

	/* Get triples for pattern s p o */
 	....
	if(sid>0 && oid>0 && pid>0) results = dic.doesExist(&count, sid, oid, pid);

	/* Print all triples */
	if(count > 0)
		for(int i=0; i<count; i++){
			cout << dic.getSubObj(results[i].sub) << "\t";
   			cout << dic.getPredicate(results[i].pred) << "\t";
	  		cout << dic.getSubObj(results[i].obj) << endl;
		}
	dic.close();

