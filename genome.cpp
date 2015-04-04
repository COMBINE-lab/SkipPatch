#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

#include "genome.h"
#include "utils.h"
//#define BENCHMARK 0
//#define DEBUG 0
using namespace std;

void genome::get_input()
{
  ignore_first_line();
  string input;
  while(getline(cin,input)){
    reference+=input;
    input.clear();
  }
#ifdef DEBUG
cout<<"Input taken! "<<endl;
#endif
}

void genome::set_reference(std::string input){
    reference = input;
}

long genome::get_length()
{
  return reference.length();
}

void genome::construct_hash()
{
  for(auto it=reference.begin();it<=reference.end()-K+1;it++)
  {
    string temp(it,it+K);
    m[temp].push_back(it-reference.begin());
  }
}

float genome::get_load_factor()
{
  return m.load_factor();
}

void genome::display_genome()
{
  cout<<reference<<endl;
}

void genome::display_hash()
{
  for(auto it=m.begin();it!=m.end();it++)
  {
    cout<<it->first<<"\t";
    for(auto list_it=(it->second).begin();list_it!=(it->second).end();list_it++)
    {
      cout<<*list_it<<" ";
    }
    cout<<endl;
  }
}

void genome::display_load()
{
  std::cout << "size = " << m.size() << std::endl;
  std::cout << "bucket_count = " << m.bucket_count() << std::endl;
  std::cout << "max_load_factor = " << m.max_load_factor() << std::endl;
  
}

void genome::remove_kmer_from_hash_at(long i,string curr_kmer)
{
  m[curr_kmer].remove(i); //#need to define own erase function
  if(m[curr_kmer].size()==0)
    m.erase(curr_kmer);
  
}

void genome::add_kmer_from_hash_at(long i,string new_kmer)
{
  m[new_kmer].push_back(i);
}

bool genome::snp_at(long pos,long len,string deflt /* ="" */)
{
  long max_len=reference.length();
  if(!is_valid_input(pos,len,reference.length())) //checks positions to be modified are valid
    {
	cout<<"Insertion at "<<pos<<" of len "<<len<<" failed"<<endl;
   	 return false;
	}
#ifdef DEBUG
cout<<"Input is valid"<<len<<endl;
#endif

  string new_string(deflt.begin(),deflt.end());
  if(new_string=="")
  	new_string = generate_random_string(len);
  len = new_string.length();
#ifdef DEBUG
cout<<"len  = "<<len<<endl;
#endif
  //handling edge cases:
  long snp_begin = (pos-K+1)<0?0:(pos-K+1); 
  long snp_end = (pos+len+K-1)>max_len?max_len:(pos+len+K-1);
  //cout<<snp_begin<<endl;
  
  //string modified_reference_substr=generate_modified_substr(snp_begin,pos,new_string);
  string modified_reference_substr(reference.begin()+snp_begin,reference.begin()+pos);
  modified_reference_substr+=new_string;
  string temp(reference.begin()+pos+len,reference.begin()+snp_end);
  modified_reference_substr+=temp;
/*
struct timeval start, end;
    struct timezone tzp;

  //  std::vector<std::pair<long,char> > random = generateRandomInserts(g.get_length());

    gettimeofday(&start, &tzp);*/
  for(long i=snp_begin;i<(snp_end-(K-1));i++)
  {
    string curr_kmer(reference.begin()+i,reference.begin()+i+K);
    string new_kmer(modified_reference_substr.begin()+(i-snp_begin),modified_reference_substr.begin()+(i-snp_begin+K));
    remove_kmer_from_hash_at(i,curr_kmer);
    add_kmer_from_hash_at(i,new_kmer);
  }
/*  gettimeofday(&end, &tzp);
  //  cout<<" for "<<TESTS<<" snips \t";
    print_time_elapsed("snips: ", &start, &end);
*/
  //updating the reference itself
  for(long i=pos;i<pos+len;i++)
  {
    reference[i] = new_string[i-pos];
  }
  #ifndef BENCHMARK
    cout<<"Generated string: "<<new_string<<"\t"<<"inserted at: "<<pos<<endl;
  #endif
  
  return true;
}

/*
Search the hash table to find the read.
If the length of the string is greater than k, 
k-mers of the read are generated and mapped to the genome.
*/
std::list<long> genome::find(string read){

	std::list<long> pos_prev;
	std::list<long> pos_curr;

	//Look for the first K-length substring of the read
	std::string read_kmer = read.substr(0,K); 
	auto search = m.find(read_kmer);
    if(search != m.end()) {
		pos_prev = search->second;
	} else {
		#ifndef BENCHMARK
		std::cout<< read << ": Not Found!"<<std::endl;
		#endif
		return pos_prev;
	}
	long pos = 1;
	//For every k-mer after that, check if it extends from the previous k-mer
	for(auto iter=read.begin()+1; iter!=read.end()-K+1; iter++){
    	read_kmer = read.substr(pos,K);
    	search = m.find(read_kmer); 

	    if(search != m.end()) { //If the k-mer is found, check if it can be extended from any of the previous k-mers		
			pos_curr = search->second;
			auto iter_p = pos_curr.begin();
			while(iter_p!=pos_curr.end()){
				auto curr = iter_p;
				++iter_p;
				//If you cannot extend from one of the earlier k-mers, remove that position
				if(std::find(pos_prev.begin(), pos_prev.end(),*curr-1)==pos_prev.end()){
                    pos_curr.erase(curr);
				}
			}
		} 		    
		pos_prev = pos_curr;
		pos_curr.clear();
		pos++;
  	}

 	long buf = read.length()-K;
  	for(auto iter=pos_prev.begin(); iter!=pos_prev.end(); iter++){
  		*iter -= buf;
  	}

  	return pos_prev;
}
