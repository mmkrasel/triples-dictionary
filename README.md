# triples-dictionary
Create a dictionary for RDF triples and access the triples through triples table

**# Dependencies**

	Packages: (Install boost and rapidjson library and keep it in the path of /usr/include/)
 		libboost-all-dev version 1.71.0
   		rapidjson-dev=1.1.0
	Compiler: g++ 
	Note: User **build.sh** to compile and build

**#Create dictionary from triple datatsets**

	./TD -d path_to_source_NT_dataset_file td_directory

**#Search for triple pattern (with uri-component ids)**

	./TD -q td_directory triple_pattern

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
 
    ./TD -q ../dbpedia/triple_tree x 1 2	


**#Search for triple pattern (with uri)**

	./TD -s td_directory triple_pattern

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



**#Example query: 	Find all triples having subject "http://www.Department6.University81.edu/GraduateStudent33".**
		
    ./TD -q ../lubm/triple_tree http://www.Department6.University81.edu/GraduateStudent33 x x


  
