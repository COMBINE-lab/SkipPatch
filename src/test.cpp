#include <iostream>
#include <vector>
#include <string>
#include <cassert>

#include "test.h"
#include "genome.h"
#include "utils.h"

void test(){

	test_search();

}

/* Tests if the search for a string of any length in the hash map is working correctly */
void test_search(){

    std::cout<<std::endl<<"Testing Search.. \t";

    genome g;
    std::string reference = "ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGA"; 
    g.set_reference(reference);
    g.construct_hash();

    std::vector<std::string> existing_reads {"TGATCGATCGATCGCAGATCGA", "ATTAGCTAGCCTAGCTAGTA", "CCCCCTATCATCATCATCTACTACAT", 
    											"GTAGATGGATCTCCCCCTATCATCAT", "TGGATCTCCCCCTATCATCA", "ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACT",
			    								"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGA"};
    
    for(std::string read: existing_reads){
        assert(g.find(read)==find_substr(reference,read));
    }

    std::vector<std::string> non_existent_reads {"TGATCGATCGATCGCAGATCGAA", "ATTAGCTAGTTTAGCTAGTA", "CCCCCTATCATTCATCTACTACATC", 
    												"GTAGATGGATCTCCCCTATCATCAT", "TGGATCTCCCCCTATCATCAAT", "ATTAGCTAGCTAGCTAGTAGATGGATCTCCCTATCATCTCATCTACT",
			    									"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGC"};	

    for(std::string read: non_existent_reads){
        assert(g.find(read).size()==0);
    }

    std::cout<<"Passed All Tests!"<<std::endl;

    return;
}

/*
bool check_hash(const genome &g,long k)
{
	if(k==0)
		k=g.geg_length()-K+1;
	
	for(long i=0;i<k;i++)
	{
		string kmer(reference.begin()+i,reference.end()+i+K);
		vector l=m.find(kmer);
		if(l.isempty())
		{
			cout<<"vector is empty!!"<<endl;
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
