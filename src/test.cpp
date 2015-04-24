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
void test(){

	test_search();
	//test_hash();
	//test_insert_at();
	std::cout<<"Passed Tests!"<<std::endl;
}

/* Tests if the search for a string of any length in the hash map is working correctly */
/*void test_search(){

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
}*/

void check_insert_at(genome g,string ins,long abs_val,string &reference)
{
  //test if the the skip list has been updated..
	skip_list s = g.get_skip_list(); //*define this
	//get_prev_node(abs_val,genome_val,offset,&prev);
	check_skip_list_node(s,abs_val,ins); //*include skip list test
	
	//now check the hash
	auto m = g.get_hash();
	
	//update the reference and construct hash for the same
	reference.insert(abs_val,ins);
	genome g_temp;
	g_temp.set_reference(reference);
	g_temp.construct_hash();
	
	//the two hashes must be equal
	auto m_temp = g_temp.get_hash();
	assert(m_temp==m);
	
}

void check_search(genome g,string reference)
{
  
    long mylong[] = {1,3,10,14}; //careful.. index+K should be in bounds!
    std::vector<long> positions (mylong, mylong + sizeof(mylong) / sizeof(long) );
    
    std::vector<std::string> existing_reads;
    
    for(auto it = positions.begin();it!=positions.end();it++)
    {
      int kmer_len=0;
      if((*it)%2==0)
	kmer_len = K+5;
      else
	kmer_len = K+6;
	
      string temp(reference,*it,kmer_len);
      existing_reads.push_back(temp);
      
    }
    
    for(std::string read: existing_reads){
        assert(g.find(read)==find_substr(reference,read));
	cout<<"assert passed"<<endl;
    }
    
    //non_existent_reads
    for(auto it=existing_reads.begin();it!=existing_reads.end();it++)
    {
      *it = *it+"ZZ";
    }
    

    for(std::string read:existing_reads){
        assert(g.find(read).size()==0);
	//cout<<"assert passed!"<<endl;
    }

}
void test_search(){

    std::cout<<std::endl<<"Testing Search.. \t";

    genome g;
    std::string reference = "ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGAATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGA"; 
    g.set_reference(reference);
    g.construct_hash();
    
    check_search(g,reference);
    
    long abs_val = 7;string ins = "TT";
    long genome_val;unsigned long offset;node *prev;
	
    check_insert_at(g,ins,abs_val,reference);
    check_search(g,reference);
    
    abs_val=10;
    check_insert_at(g,ins,abs_val,reference);
    check_search(g,reference);
    
    abs_val=1;
    check_insert_at(g,ins,abs_val,reference);
    check_search(g,reference);
    
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

void test_insert_at()
{

 	cout<<endl<<"Testing insertions..\t";
	
	genome g;
	std::string reference = "ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGA"; 
	g.set_reference(reference);
	g.construct_hash();
	
	//edge cases
	//cout<<"Checking hash : "<<g.check_hash();
	long abs_val = 7;string ins = "TT";
	long genome_val;unsigned long offset;node *prev;
	g.insert_at(ins, abs_val);
	check_insert_at(g,ins,abs_val,reference);
	
	//should make this at random positions.. 
	abs_val=10;
	g.insert_at(ins, abs_val);
	check_insert_at(g,ins,abs_val,reference);
	
	abs_val=1;
	g.insert_at(ins, abs_val);
	check_insert_at(g,ins,abs_val,reference);
	
	abs_val=17;
	g.insert_at(ins, abs_val);
	check_insert_at(g,ins,abs_val,reference);
	
	g.snp_at(0,2,"NN");
	//g.check_hash();
	check_hash1(g);
	//cout<<"Checking hash : "<<g.check_hash();
	g.snp_at(g.get_length()-K,3,"NNN");
	//g.check_hash();
	check_hash1(g);
	g.snp_at(5,3,"NNN");  
	std::cout<<"Passed insert_at Tests! "<<std::endl;
}