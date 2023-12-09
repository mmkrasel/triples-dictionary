#!/bin/bash

rm -r src


echo "===================================================================================="
echo "Compiling and Buidling ...."

for cpp_file in Common ComponentTree Dictionary HashSegmented TripleTable main; do
	g++ -I/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -std=gnu++11 -MMD -MP -MF"src/$cpp_file.d" -MT"src/$cpp_file.o" -o "src/$cpp_file.o" "src/$cpp_file.cpp"
done

g++ -o TD  ./src/Common.o ./src/ComponentTree.o ./src/Dictionary.o ./src/HashSegmented.o src/main.o ./src/TripleTable.o -lboost_regex

# echo "Executable has been build with name 'TT' ...."
# echo "===================================================================================="
# echo 
# echo "===================================================================================="
# echo "# Create the dictionary for an input RDF triples dataset."
# echo "# 'td_dictionary' is a directory to store all kinds of data produced from triple-tree"
# #./TD -d RDF_Triples_datatset_file td_dictionary
# echo "===================================================================================="
# echo
# echo "DONE"
# echo "===================================================================================="
# echo "# Search for a given pattern."
# echo "# Last 3 arguments represent the triple pattern S P O"
# echo "# 'x' indicates an unbound triple componet"
# echo 
# echo "# Following command finds all triples having subject id 1"
# echo "./TD -q tt_directory 1 x x"
# ./TD -q /media/rasel/hdd/RDF/datasets/lubm/1000 x x 7
# echo
# echo "# Following commands retrieve all triples having subject as http://www.Department0.University0.edu/UndergraduateStudent59"
# echo "./TD -s tt_directory page_size http://www.Department0.University0.edu/UndergraduateStudent59 x x"
# ./TD -s /media/rasel/hdd/RDF/datasets/lubm/1000 http://www.Department0.University0.edu/UndergraduateStudent59 x x
# echo 
# echo "# Following commands retrieve all triples having object as http://www.Department0.University0.edu/UndergraduateStudent59"
# echo "./TD -s tt_directory page_size x x http://www.Department0.University0.edu/UndergraduateStudent59"
# echo 
# echo "# Following command retrieves the triple having ID 1"
# echo "./TD -q tt_directory 1"
# ./TD -t /media/rasel/hdd/RDF/datasets/lubm/1000 1
# echo "===================================================================================="
# echo 


