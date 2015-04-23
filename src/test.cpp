#include <iostream>
#include <vector>
#include <string>
#include <cassert>

#include "test.h"
#include "genome.h"
#include "utils.h"
using namespace std;
void test(){

	test_search();
	test_hash();
	std::cout<<"Passed Search Tests!"<<std::endl;
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

    std::cout<<"Passed Search Tests!"<<std::endl;

    return;
}

void check_hash1(genome g)
{
	string reference = g.get_reference();
	long k = reference.length()-K+1;
	//cout<<reference<<endl;
	auto m = g.get_hash();
	for(long i=0;i<k;i++)
	{
		string kmer(reference.begin()+i,reference.begin()+i+K);
		//cout<<"kmer "<<kmer<<endl;
		
		auto pos_it=m.find(kmer);
		if(pos_it==m.end())
		{
		  //cout<<"kmer not found!"<<endl;
		  assert(false);
		}  
		else
		{
		  vector<long> pos = pos_it->second;
		  for(auto it=pos.begin();it!=pos.end();it++)
		  {
		    string temp(reference.begin()+*it,reference.begin()+*it+K);
		  //  cout<<" temp "<<temp<<" pos = "<<*it<<endl;
		    assert(temp==kmer);
		  }
		}
		//cout<<"assertion true"<<endl;
	}
	//return true;

}
void test_hash()
{
 	cout<<endl<<"Testing snips..\t";
	
	genome g;
	std::string reference = "ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGA"; 
	g.set_reference(reference);
	g.construct_hash();
	//edge cases
	//cout<<"Checking hash : "<<g.check_hash();
	g.snp_at(0,2,"NN");
	//g.check_hash();
	check_hash1(g);
	//cout<<"Checking hash : "<<g.check_hash();
	g.snp_at(g.get_length()-K,3,"NNN");
	//g.check_hash();
	check_hash1(g);
	g.snp_at(5,3,"NNN");
	//g.check_hash();
	check_hash1(g);
	g.snp_at(17,3,"ASD");
	//g.check_hash();
	check_hash1(g);
	g.snp_at(100,3,"FSA");
	//g.check_hash();
	check_hash1(g);
	
	g.snp_at(10,3,"GRE");
	//g.check_hash();
	//cout<<"Checking hash : "<<g.check_hash();
	check_hash1(g);
	std::cout<<"Passed hash Tests! "<<std::endl;
	//invaled cases
	
	//somewhere in the middle
	
}
