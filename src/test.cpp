#include <iostream>
#include <vector>
#include <string>
#include <cassert>

#include "./skip_list/skip_list.h"
#include "./skip_list/skip_list_test.h"

#include "test.h"
#include "genome.h"
#include "utils.h"

using namespace std;

void test_hash(genome g)
{
	string reference = g.get_reference();
	long k = reference.length()-K+1;
	
	auto m = g.get_hash();
	for(long i=0;i<k;i++)
	{
		string kmer(reference.begin()+i,reference.begin()+i+K);
		auto pos_it=m.find(kmer);
		if(pos_it==m.end())
		{
		  	assert(false);
		}
		else
		{
		  	vector<long> positions = pos_it->second;
		  	for(long pos: positions)
		  	{
		    	string temp(reference.begin()+pos,reference.begin()+pos+K);
		    	assert(temp==kmer);
		  	}
		}
	}
}

void check_insert_at(genome g, string ins, long abs_val, string &reference)
{
  	//test if the the skip list has been updated..
	skip_list s = g.get_skip_list(); //*define this
	check_skip_list_node(s, abs_val, ins); //*include skip list test

	//check if the reference has been updated
	reference.insert(abs_val+1,ins);
	assert(reference == g.get_updated_reference());

	/* 
	*****The hash of both the strings won't be the same because 
	when you construct the hash based on the updated reference, 
	the positions in the new hash are "virtual positions", and won't be the same as the "genome positions"
	
	//now check the hash
	//update the reference and construct hash for the same
	genome g_temp;
	g_temp.set_reference(updated_ref);
	g_temp.construct_hash();	

	//the two hashes must be equal
	assert(g_temp.get_hash() == g.get_hash());
	*/
}

/* Tests if the search for a string of any length in the hash map is working correctly */
void test_search_static_reference(){

	cout << std::endl << "test_search_static_reference(): Start" << std::endl;

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

	cout << "test_search_static_reference(): Complete" << std::endl << std::endl;
}


void check_search(genome g, string reference)
{
    std::vector<long> positions {1,3,14,21}; //careful.. index+K should be in bounds!
    std::vector<std::string> existing_reads;
    
    for(auto it = positions.begin(); it!=positions.end(); it++)
    {
    	int kmer_len=0;
      	if((*it)%2==0){
			kmer_len = K+5;
      	}
      	else{
			kmer_len = K+6;
		}
		
      	string temp(reference,*it,kmer_len);
      	existing_reads.push_back(temp);
      
    }
    
    for(std::string read: existing_reads){
    	std::cout << "Searching for: " << read << std::endl;
        assert(g.find(read)==find_substr(reference,read));
    }
    
    //non_existent_reads
    for(auto it=existing_reads.begin();it!=existing_reads.end();it++)
    {
      *it = *it+"ZZ";
    }
    
    for(std::string read:existing_reads){
        assert(g.find(read).size()==0);
    }
}

void test_search_dynamic_reference(){

	cout << std::endl << "test_search_dynamic_reference(): Start" << std::endl;
	
    genome g;
    std::string reference = "ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGAATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGA"; 
    g.set_reference(reference);
    g.construct_hash();
    
    check_search(g,reference);
    
    string ins = "TT";
    long genome_val;unsigned long offset;node *prev;

	long abs_val = 7;
    check_insert_at(g,ins,abs_val,reference);
    check_search(g,reference);
    
    abs_val=10;
    check_insert_at(g,ins,abs_val,reference);
    check_search(g,reference);
    
    abs_val=21;
    check_insert_at(g,ins,abs_val,reference);
    check_search(g,reference);

    cout << "test_search_dynamic_reference(): Complete" << std::endl << std::endl;    
}

void test_search(){

	cout << std::endl << "test_search(): Start" << std::endl;

	test_search_static_reference();
	test_search_dynamic_reference();

	cout << "test_search(): Complete" << std::endl << std::endl;

}

void test_snp_at()
{
 	cout << std::endl << "snp_at(): Start" << std::endl;
	
	genome g;
	std::string reference = "ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGAATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGA	"; 
	g.set_reference(reference);
	g.construct_hash();

	g.snp_at(0,"AC");
	test_hash(g);

	g.snp_at(g.get_length()-K,"TTA");
	test_hash(g);

	g.snp_at(5,"ACA");
	test_hash(g);

	g.snp_at(17,"C");
	test_hash(g);

	g.snp_at(100,"ATCCG");
	test_hash(g);
	
	g.snp_at(10,"GTTAA");
	test_hash(g);

	std::cout <<"snp_at(): Complete!" << std::endl << std::endl;
	
	//invaled cases
	//somewhere in the middle
	
}

void test_insert_at()
{
 	cout << std::endl << "insert_at(): Start" << std::endl;
	
	//length of insertion <K, >K, =K, =1
	std::vector<std::string> insertions {"ATG", "TTGTAC", "AAAA", "C"}; 

 	for(std::string ins: insertions){
		
		genome g;
		std::string reference = "ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGAT"; 
		g.set_reference(reference);
		g.construct_hash();
		
		//Randomize? 
		//For now, they're spaced in such a way that the edit areas of two insertions don't overlap
		//No nested insertions supported yet
		std::vector<long> positions {g.get_length()-1,0,15,31,40};

		for(long p: positions){
			g.insert_at(ins, p);
			check_insert_at(g, ins, p, reference);
		}
	}

	cout << "insert_at(): Complete" << std::endl << std::endl;
}

void test(){

	std::cout << std::endl <<  "Testing: Start" << std::endl;
	
	test_snp_at();
	test_insert_at();
	//test_search();
	
	std::cout << "Testing: Complete!" << std::endl << std::endl;
}