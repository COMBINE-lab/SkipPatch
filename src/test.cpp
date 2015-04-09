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
/*
bool check_hash(const genome &g,long k)
{
	if(k==0)
		k=g.geg_length()-K+1;
	
	for(long i=0;i<k;i++)
	{
		string kmer(reference.begin()+i,reference.end()+i+K);
		list l=m.find(kmer);
		if(l.isempty())
		{
			cout<<"List is empty!!"<<endl;
			return false;
		}
		if(find(l.begin(),l.end(),i)==l.end())
		{
			cout<<"k mer at position "<<i<<" not found!!";
			return false;
		}
	}
	return true;

}
bool test_hash()
{
 	cout<<endl<<"Testing snips..\t";
	genome g;
	g.get_input();
	g.construct_hash();
	//edge cases
	check_hash(g);
	g.snp_at(0,2);
	check_hash(g,K+K);
	g.snp_at(g.get_length()-K,3);
	check_hash(g,g.get_length()-K-K,g.get_length());

	//invaled cases
	
	//somewhere in the middle
	
	
}*/
