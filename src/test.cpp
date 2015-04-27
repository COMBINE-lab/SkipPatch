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
	skip_list s = g.get_skip_list();
	//check_skip_list_node(s, abs_val, ins); 
	
	//check if the reference has been updated
	reference.insert(abs_val+1,ins);
	assert(reference == g.get_updated_reference());

/*	std::cout << "Modified Hash: " << std::endl;
	g.display_hash();
	std::cout << std::endl; */
	
	/*---Something really fishy here---*/

	//now check the hash
	//update the reference and construct hash for the same
	/*
	genome g_temp;
	g_temp.set_reference(reference);
	g_temp.construct_hash();
	std::cout << "New Hash: " << std::endl;
	g_temp.display_hash();
	std::cout << std::endl;

	auto m_temp = g_temp.get_hash();
	auto m_genome = g.get_hash();      
	for(auto it=g_temp.get_hash().begin(); it!=g_temp.get_hash().end(); it++)
	{	
		std::cout << it->first << std::endl;
	  	for(auto row_it=(it->second).begin(); row_it!=(it->second).end(); row_it++)
	  	{
		    //translate from abs to genome pos:    
		    unsigned long pos;
		    long val;
		    node *n;
		    s.get_prev_node(*row_it,val,pos,&n);
		    *row_it=val;
		    std::cout << *row_it << " "; //Prints weird values

	  	} std::cout << std::endl;

		sort(it->second.begin(),it->second.end());
		sort(m_genome[it->first].begin(), m_genome[it->first].end());
		std::cout << "The hash should be: " << std::endl;
		g_temp.display_hash();
		std::cout << std::endl;
		//assert(it->second==m_genome[it->first]);
	}*/
}

/* Tests if the search for a string of any length in the hash map is working correctly */
void test_search_static_reference(){

	cout << "test_search_static_reference(): Start" << std::endl;

    genome g;
    std::string reference = "ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGA"; 
    g.set_reference(reference);
    g.construct_hash();

    std::vector<std::string> existing_reads {"AGCTAGCC","TGATCGATCGATCGCAGATCGA", "ATTAGCTAGCCTAGCTAGTA", "CCCCCTATCATCATCATCTACTACAT", 
    											"GTAGATGGATCTCCCCCTATCATCAT", "TGGATCTCCCCCTATCATCA", "ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACT",
			    								"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGA"};
    
    for(std::string read: existing_reads){
        assert(g.search(read)==find_substr(reference,read));
    }

    std::vector<std::string> non_existent_reads {"TGATCGATCGATCGCAGATCTT", "ATTAGCTAGTTTAGCTAGTA", "CCCCCTATCATTCATCTACTACATC", 
    												"GTAGATGGATCTCCCCTATCATCAT", "TGGATCTCCCCCTATCATCAAT", "ATTAGCTAGCTAGCTAGTAGATGGATCTCCCTATCATCTCATCTACT",
			    									"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCAC"};	

    for(std::string read: non_existent_reads){
        assert(g.search(read).size()==0);
    }

	cout << "test_search_static_reference(): Complete" << std::endl;
}


void check_search(genome g, string reference)
{	
	//Randomize
    std::vector<long> positions {1,3,14,21}; //careful.. index+K should be in bounds!
    std::vector<std::string> existing_reads;
    
    for(long pos: positions)
    {
    	int kmer_len=0;
      	if((pos)%2==0){
			kmer_len = K+5;
      	}
      	else{
			kmer_len = K+6;
		}
		
      	string temp(reference,pos,kmer_len);
      	existing_reads.push_back(temp);
    }

    for(std::string read: existing_reads){
    	assert(g.search(read)==find_substr(g.get_updated_reference(),read)); 
		 	
		 	/* auto temp1 = g.search(read);
			auto temp2 = find_substr(reference,read);
			cout<<"The reference is "<<reference<<endl;
			cout<<"The reference is "<<g.get_updated_reference()<<endl;
			cout<<"Current read is "<<read<<endl;
			cout<<"This should  be at ";
			for(auto it = temp2.begin();it!=temp2.end();it++)
			  cout<<*it<<"\t";
			cout<<endl;
			cout<<"This is at ";
			for(auto it = temp1.begin();it!=temp1.end();it++)
			  cout<<*it<<"\t";
			cout<<endl;
		    sort(temp1.begin(),temp1.end());
			sort(temp2.begin(),temp2.end());
			assert(temp1==temp2);*/
    }
    
    //non_existent_reads
    for(std::string read: existing_reads)
    {
    	read+=std::string("Z");
    	assert(g.search(read).size()==0);
    }
}

void test_search_dynamic_reference(){

	cout << "test_search_dynamic_reference(): Start" << std::endl;
	
    genome g;
    std::string reference = "ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCAT"; 
    g.set_reference(reference);
    g.construct_hash();
    
    check_search(g,reference);
    
    string ins = "AGATTA";
    long genome_val;unsigned long offset;node *prev;

    long abs_val = 0;
    g.insert_at(ins, abs_val);
    check_insert_at(g,ins,abs_val,reference);
    check_search(g,reference);
    
    abs_val=12;
    g.insert_at(ins, abs_val);
    check_insert_at(g,ins,abs_val,reference);
    check_search(g,reference);

    abs_val=30;
    g.insert_at(ins, abs_val);
    check_insert_at(g,ins,abs_val,reference);
    check_search(g,reference);

    cout << "test_search_dynamic_reference(): Complete" << std::endl;    
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

	std::cout <<"snp_at(): Complete!" << std::endl;
	
	//invaled cases
	//somewhere in the middle
	
}

void test_insert_at()
{
 	cout << std::endl << "insert_at(): Start" << std::endl;
	
	//length of insertion <K, >K, =K, =1
	std::vector<std::string> insertions {"ATG", "TTGTAC", "ATCG", "C"}; 

 	for(std::string ins: insertions){
		
		genome g;
		std::string reference = "ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGAT"; //ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGAT 
		g.set_reference(reference);
		g.construct_hash();
		
		//Randomize? 
		std::vector<long> positions {22,21,7,11,12};

		for(long p: positions){
			//std::cout << "B: " << g.get_updated_reference() << std::endl;
			g.insert_at(ins, p);
			//std::cout << "A: " << g.get_updated_reference() << std::endl;
			check_insert_at(g, ins, p, reference);
		}
	}

	cout << "insert_at(): Complete" << std::endl;
}

void test(){

	std::cout << std::endl <<  "Testing: Start" << std::endl;
	
	test_snp_at();
	test_insert_at();
	test_search();
	
	std::cout << "Testing: Complete!" << std::endl;
}