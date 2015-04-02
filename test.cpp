#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <cassert>

#include <ctime>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "test.h"
#include "genome.h"
#include "utils.h"

using namespace std;

/* Tests if the search for a string of any length in the hash map is working correctly */
void test_search(){

    std::cout<<std::endl<<"Testing Search.. \t";

    genome g;
    std::string reference = "ATTAGCTAGCCTAGCT"; 
    g.set_reference(reference);
    g.construct_hash();

    std::vector<std::string> reads {"ATT", "TAG", "TAGC", "TAGCT","TAGCTAGC","CCTAGCT", "ATTAGCTAGCCTAGCT","TTAGCTAGCCTAGC"};
    for(std::string read: reads){
        assert(g.find(read)==find_substr(reference,read));
    }
   
    std::cout<<"Passed All Tests!"<<std::endl;

}

