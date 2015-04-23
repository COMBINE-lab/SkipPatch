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
    //ignore_first_line();
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

std::string genome::get_reference()
{
  return reference;
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
    m[curr_kmer].erase(std::remove(m[curr_kmer].begin(), m[curr_kmer].end(), position_to_remove), m[curr_kmer].end());
    assert( std::find( m[curr_kmer].begin(), m[curr_kmer].end(), position_to_remove) == m[curr_kmer].end() ); //Verifying if the position was actually deleted
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

//Is it required to pass both the length and the string? Can't we derive the length from the string itself? - its supposed to be optional - did you change it?
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

std::unordered_map<std::string, std::vector<long>> genome :: get_hash()
{
  return m;
}
/*

Inserts a string at the 'insert_pos'. Updates the k-mers which lie in the "region of change" 
Splits the problem into two segments:
 - Updating the k-mers which appear before the insertion
 - Adding the new k-mers which are a result of the insertion 

Working test cases:
 - Insertions of lengths <K, ==K, >K
 - Insertions in locations: (begin,begin+K), (end-K,end), other 

Current limitations (due to unavailability of helper functions)
1. Assumes that there has been no prior insertion (positions are original genome positions) 
2. Does not support nested insertions
3. Supports only single insertion - no handling of overlapping k-mers which might have arisen due to adjacent insertions

*Needs rigorous testing - several test cases to be written*

*/

void genome::insert_at(std::string insertion, long insert_pos){

    std::cout << "Inserting " << insertion << " at " << insert_pos << std::endl; 

    if(insert_pos<0 || insert_pos>reference.length()-1){
        std::cerr << "Position of insertion out of bounds of length of genome" << std::endl;
        return;
    }

    long edit_start = std::max(insert_pos-K+2,(long)0);
    long edit_end = std::min(insert_pos+K, (long)reference.length()-1);

    //Segment which contains the k-mers which no longer exist 
    std::string edit_segment = std::string(reference.begin()+edit_start, reference.begin()+edit_end+1);

    //Make a new string of edit_segment + "insertion" to replace the removed k-mers with new k-mers
    std::string new_segment = std::string(reference.begin()+edit_start, reference.begin()+insert_pos+1)
                            + insertion 
                            + std::string(reference.begin()+insert_pos+1, reference.begin()+insert_pos+K);
    
    
    //Replace all the modified k-mers contained in edit_segment from the hashmap
    for(int i=0; i<K-1; i++){
        if(i<=insert_pos){ //Check to handle insertions at the beginning of the genome
            std::string curr_kmer = edit_segment.substr(i,K);
            std::string new_kmer = new_segment.substr(i,K);
            //std::cout << "Replacing: " << curr_kmer << " at " << edit_start+i << " with " << new_kmer << std::endl;
            if(curr_kmer!=new_kmer){
                if(curr_kmer.length()==K){
                    remove_kmer_from_hash_at(edit_start+i, curr_kmer);
                }
                if(new_kmer.length()==K){
                    add_kmer_from_hash_at(edit_start+i, new_kmer);
                }
            }
        }
    }

    //Add the new k-mers generated due to insertion into the hashmap
    std::string new_kmer_segment = std::string(insertion+reference.substr(insert_pos+1,K-1));
    if(new_kmer_segment.length()>=K){
        for(int i=0; i<new_kmer_segment.length()-K+1; i++){
            std::string new_kmer = new_kmer_segment.substr(i,K);
            if(new_kmer.length()==K){
                //std::cout << "Adding: " << new_kmer << " at " << insert_pos << std::endl;
                add_kmer_from_hash_at(insert_pos, new_kmer);
            }
        }
    }
}