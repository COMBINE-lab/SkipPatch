#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

#include "genome.h"
#include "utils.h"
//#define BENCHMARK 0
//#define DEBUG 0

void genome::get_input()
{
    ignore_first_line();
    std::string input;
    while(getline(std::cin, input)){
        reference+=input;
        input.clear();
    }
    #ifdef DEBUG
    std::cout<<"Input taken! "<<std::endl;
    #endif
}

void genome::set_reference(std::string input)
{    
    reference = input;
}

long genome::get_length()
{
    return reference.length();
}

void genome::construct_hash()
{
    for(auto it=reference.begin();it<=reference.end()-K+1;it++){    
        std::string temp(it,it+K);
        m[temp].push_back(it-reference.begin());
    }
}

float genome::get_load_factor()
{    
    return m.load_factor();
}

void genome::display_genome()
{
    std::cout<<reference<<std::endl;
}

void genome::display_hash()
{
    for(auto it=m.begin();it!=m.end();it++){
        std::cout<<it->first<<"\t";
        for(auto vector_it=(it->second).begin();vector_it!=(it->second).end();vector_it++){
            std::cout<<*vector_it<<" ";
        }
        std::cout<<std::endl;
    }
}

void genome::display_load()
{
    std::cout << "size = " << m.size() << std::endl;
    std::cout << "bucket_count = " << m.bucket_count() << std::endl;
    std::cout << "max_load_factor = " << m.max_load_factor() << std::endl; 
}

void genome::remove_kmer_from_hash_at(long position_to_remove, std::string curr_kmer){

    std::vector<long> positions = m[curr_kmer];
    positions.erase(std::remove(positions.begin(), positions.end(), position_to_remove), positions.end());
    
    std::cout<<"genome::remove_kmer_from_hash_at\t"<<curr_kmer<<"\t"<<positions.size()<<std::endl;

    //Having a very long "vector/list" of positions (~1 million) causes a bottleneck here
    //Approximately takes 0.5 seconds to execute this line "once" if the length of "positions" is 1 million
    
    if(m[curr_kmer].size()==0){
        m.erase(curr_kmer);
    } 
}

void genome::add_kmer_from_hash_at(long position, std::string new_kmer)
{   
    m[new_kmer].push_back(position);
}

//Is it required to pass both the length and the string? Can't we derive the length from the string itself?
bool genome::snp_at(long pos, long len, std::string new_string) 
{
    if(len!=new_string.length()){
        std::cerr << "genome::snp_at \t The length of the new string is not equal to the length of the update specified in the arguments.\n";
    }

    long max_len=reference.length();
    if(!is_valid_input(pos, len, reference.length())) //checks positions to be modified are valid
    {
        std::cout<<"Insertion at "<<pos<<" of len "<<len<<" failed"<<std::endl;
    	return false;
    }
    #ifdef DEBUG
    cout<<"Input is valid"<<len<<std::endl;
    #endif

    /*std::string new_string(new_str.begin(),new_str.end());
    if(new_string==""){
    	new_string = generate_random_string(len);
    }
    len = new_string.length();*/
    #ifdef DEBUG
    cout<<"len  = "<<len<<std::endl;
    #endif
    //handling edge cases:
    long snp_begin = (pos-K+1)<0?0:(pos-K+1); 
    long snp_end = (pos+len+K-1)>max_len?max_len:(pos+len+K-1);
    //cout<<snp_begin<<endl;

    //string modified_reference_substr=generate_modified_substr(snp_begin,pos,new_string);
    std::string modified_reference_substr(reference.begin()+snp_begin,reference.begin()+pos);
    modified_reference_substr+=new_string;
    std::string temp(reference.begin()+pos+len,reference.begin()+snp_end);
    modified_reference_substr+=temp;

    for(long i=snp_begin;i<(snp_end-(K-1));i++)
    {
        std::string curr_kmer(reference.begin()+i,reference.begin()+i+K);
        /*
        This is to temporarily handle the case when the k-mer is NNNNN..(k), or, contains an 'n' or 'N'
        Since this k-mer exists at several (millions) of locations, "remove_kmer_from_hash" causes a bottleneck, slowing down the update process  
        Temporary fix: Simple ignore the k-mer and return false. Permanent soultion?
        */
        if( (curr_kmer.find('n')!=std::string::npos)  || (curr_kmer.find('N')!=std::string::npos) ){
            return false;
        }
        std::string new_kmer(modified_reference_substr.begin()+(i-snp_begin),modified_reference_substr.begin()+(i-snp_begin+K));
        remove_kmer_from_hash_at(i,curr_kmer);
        add_kmer_from_hash_at(i,new_kmer);
    }

    //updating the reference itself
    for(long i=pos;i<pos+len;i++)
    {
        reference[i] = new_string[i-pos];
    }
    #ifdef DEBUG
    std::cout<<"Generated string: "<<new_string<<"\t"<<"Inserted at: "<<pos<<std::endl;
    #endif

    return true;
}

/*
Search the hash table to find the read.
If the length of the string is greater than k, 
k-mers of the read are generated and mapped to the genome.
*/
std::vector<long> genome::find(std::string read){

	std::vector<long> pos_prev;
	std::vector<long> pos_curr;

	//Look for the first K-length substring of the read
	std::string read_kmer = read.substr(0,K); 
	auto search = m.find(read_kmer);
    if(search != m.end()) {
		pos_prev = search->second;
	} else {
		return pos_prev;
	}
	long pos = 1;
	//For every k-mer after that, check if it extends from the previous k-mer
	for(auto iter=read.begin()+1; iter!=read.end()-K+1; iter++){
    	read_kmer = read.substr(pos,K);
    	search = m.find(read_kmer); 

	    if(search != m.end()) { //If the k-mer is found, check if it can be extended from any of the previous k-mers
			pos_curr = search->second;

            //Retain only those positions which you can extend from one of the earlier k-mers 
            std::vector<long> temp; //Created this temp because deleting from the vector pos_curr was generating a segmentation fault.
            for(auto iter_p=pos_curr.begin(); iter_p!=pos_curr.end(); iter_p++){
                auto curr = iter_p;
                if(std::find(pos_prev.begin(), pos_prev.end(), *curr-1) != pos_prev.end()){
                    temp.push_back(*curr);
                }
            }
            pos_curr = temp;
            temp.clear();
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
