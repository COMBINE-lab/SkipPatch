#include <utility>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

#include "genome.h"
#include "utils.h"

using namespace std;
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

    is_edit = std::vector<bool>(get_length(),false);
}

void genome::set_reference(std::string input)
{    
    reference = input;
    is_edit = std::vector<bool>(get_length(),false);
}

std::string genome::get_reference()
{
	return reference;
}

std::string genome::get_updated_reference(){

	std::string updated_reference;

	node *temp=s.get_head();
  	while(temp->down){
    	temp=temp->down;
  	}
  	temp=temp->next;
  	
  	long start=0, end=0;

	while(temp->next)
  	{
  		end = (temp->val)+1;
  		updated_reference+=std::string(reference.begin()+start, reference.begin()+end);
  		updated_reference+= temp->str;
   		start = end;
   		temp=temp->next;
  	}
  	updated_reference+=std::string(reference.begin()+start,reference.end());

  	return updated_reference;

}

/*Generated the updated reference sequence segment - including insertions and removing deletions
  from the positions 'start' to 'start+right_offset' positions of the genome (positions are genome positions)*/
std::string genome::get_updated_reference(long start, long left_offset, long right_offset){
    
    std::string updated_reference;
    node* curr_node;
    long curr_pos = start-left_offset;

    long ins_len = 0;
    if(s.find(start)){ //If there was an insertion in the first location 
         ins_len = s.find(start)->val;
    }

    while( updated_reference.length() < ins_len+right_offset+K ){ 
        updated_reference+=std::string(1,reference[curr_pos]); //**
        if(is_edit[curr_pos]){
            curr_node = s.find(curr_pos);
            updated_reference+=curr_node->str;
        }        
        curr_pos++;
    }

    return updated_reference;
}

/*Generated the updated reference sequence segment - including insertions and removing deletions
  from the positions 'begin' to 'end' of the genome (positions are genome positions)*/
std::string genome::get_updated_reference(long begin, long end){

    std::string updated_reference;
    node* curr_node;
    
    for(int i=begin; i<=end; i++){
        updated_reference+=reference[i];
        if(is_edit[i]){
            curr_node = s.find(i);
            updated_reference+=curr_node->str;
        }
    }

    return updated_reference;
}

long genome::get_length()
{
    return reference.length();
}

std::unordered_map<std::string, std::vector<long>> genome::get_hash()
{
	return m;
}

skip_list genome::get_skip_list()
{
	return s;
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

long genome::get_genome_position_from_virtual_position(long virtual_position)
{
    node *n;
    long genome_position; //genome_position is position in the 'original genome' corresponding to the virtual_position in the 'virtual genome' 
    unsigned long offset = 0;
    s.get_prev_node(virtual_position, genome_position, offset, &n); 

    return genome_position;
}

void genome::get_genome_position_from_virtual_position(long virtual_position, long &genome_position, unsigned long &offset, node **n)
{
    s.get_prev_node(virtual_position, genome_position, offset, n); 
}

long genome::get_virtual_position_from_genome_position(long genome_position) //arguement must be a tuple
{   
    return genome_position+s.get_cumulative_count(genome_position);
}

//Is it required to pass both the length and the string? Can't we derive the length from the string itself? - its supposed to be optional - did you change it? 
void genome::snp_at(long pos, std::string new_string) 
{
	long len = new_string.length();

    long max_len=reference.length();
    if(!is_valid_input(pos, len, reference.length())) //checks positions to be modified are valid
    {
        std::cerr<<"Insertion at "<<pos<<" of len "<<len<<" failed"<<std::endl;
    	std::exit(-1);
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
            return;
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

}

std::vector<std::string> genome::generate_kmers(std::string str)
{
    std::vector<std::string> kmers;
    for(int i=0; i<str.length()-K+1; i++){
        kmers.push_back(str.substr(i,K));    
    }

    return kmers;
}

/*
Search the hash table to find the read.
If the length of the string is greater than k, 
k-mers of the read are generated and mapped to the genome.
*/
std::vector<long> genome::search(std::string read){

    std::vector<long> positions;

    //An unordered_map containing positions and their "offsets"(distance from the start of the read)
    std::unordered_map<long, long> pos_prev;
    std::unordered_map<long, long> pos_curr;

    //Look for the first K-length substring of the read
    std::string read_kmer = read.substr(0,K); 
    auto search = m.find(read_kmer);
    if(search == m.end()){
        return positions;
    } 
    else {
        for(long p: search->second){
            pos_prev[p]=0;
        }
    }

    std::vector<std::string> read_kmers = generate_kmers(read);
    read_kmers.erase(read_kmers.begin());

    for(std::string kmer: read_kmers){
        search = m.find(kmer);

        if(search!=m.end()){ //If the k-mer is found, check if it can be extended from any of the previous k-mers
            for(long p: search->second){
                if(pos_prev.find(p-1)!=pos_prev.end()){ //If it can extend from the previous k-mer (and is not a part of an insertion)
                    pos_curr[p]=pos_prev[p-1]+1; //Insert the new pos with an offset++     
                }
                else if(pos_prev.find(p)!=pos_prev.end()){ //If it can extend, but is a part of an insertion,
                    pos_curr[p]=pos_prev[p]; //Insert the new position with the same offset
                }
            }
        }
        pos_prev = pos_curr;
        pos_curr.clear(); 
    }

    //For all the positions, subtract the offset (which is different depending on whether or not there was an insertion at one of the positions) 
    for(auto it=pos_prev.begin(); it!=pos_prev.end(); it++){
        positions.push_back(it->first-it->second); 
    }

    //Get virtual positions from genome positions
    //If the read started from a point within an insertion, reconstruct the required segment of the reference and find where it exactly started from 
    for(auto it=positions.begin(); it!=positions.end(); it++){            
        long position_offset = 0; //Used to locate the exact start point if the read started from a point within an insert
        if(is_edit[*it]){
            position_offset = get_updated_reference(*it,0,read.length()).find(read, 0);
        }
        *it=get_virtual_position_from_genome_position(*it);
        *it+=position_offset;
    }

    return positions;
}

/*
Inserts a string at the 'insert_pos'. Updates the k-mers which lie in the "region of change" 
Splits the problem into two segments:
 - Updating the k-mers which begin at a point before the insertion
 - Adding the new k-mers which are a result of the insertion (begin at a point in the inserted segment)
*/

void genome::insert_at(std::string ins, long insert_pos_abs){
  
    std::string insertion = ins; 

    node *n;
    long insert_pos; //position in the 'original genome' corresponding to the virtual_position in the 'virtual genome' 
    unsigned long offset = 0;
    
    get_genome_position_from_virtual_position(insert_pos_abs, insert_pos, offset, &n);
    if(n->next->val==insert_pos) //if the value already exists
      n=n->next; //**if the insertion is nested, this is not true    
    std::cout << "Inserting " << insertion << " at " << insert_pos_abs << ":" << insert_pos << "," << offset << std::endl;    

    if(insert_pos<0 || insert_pos>get_length()-1){
        std::cerr << "Position of insertion out of bounds of length of genome" << std::endl;
        std::exit(-1);
    }

    std::string nested_insertion; 
    if(is_edit[insert_pos]){
        nested_insertion=n->str;
        if(offset > nested_insertion.length()){
        	std::cerr << "Nested Insertions: Offset greater than size of insertion: " << n->str << " " << offset << " " << insertion << std::endl;
        	return;
        }
        nested_insertion.insert(offset,insertion);
        insertion = nested_insertion;
    }

    long edit_start = std::max(insert_pos-K+2,(long)0); //**may not be true - insertion just before
    long edit_end = std::min(insert_pos+K, (long)reference.length()-1); 

    //Segment which contains the k-mers which no longer exist 
    std::string edit_segment = get_updated_reference(edit_start, edit_end);
    std::cout << "edit_segment: " << edit_segment << std::endl;
    
    //Make a new string of edit_segment + "insertion" to replace the removed k-mers with new k-mers
    std::string new_segment = get_updated_reference(edit_start, insert_pos-1) + reference[insert_pos] //**why go to reference again?
                            + insertion 
                            + get_updated_reference(insert_pos+1, insert_pos+K);
    std::cout << "new_segment: " << new_segment << std::endl;
    
        //Replace all the modified k-mers
    for(int p=0; p<=insert_pos-edit_start; p++){
        std::string curr_kmer = edit_segment.substr(p,K);
        std::string new_kmer = new_segment.substr(p,K);
	std::cout << "Replacing: " << curr_kmer << " with " << new_kmer << " at " << edit_start+p << std::endl;
        if(curr_kmer!=new_kmer){
	    
            if(curr_kmer.length()==K){
                remove_kmer_from_hash_at(p+edit_start,curr_kmer);
            }    
            if(new_kmer.length()==K){
                add_kmer_from_hash_at(p+edit_start,new_kmer);
            }    
        }
    }

//    std::string new_kmer_segment = std::string(insertion+reference.substr(insert_pos+1,K-1));
      std::string new_kmer_segment = insertion + get_updated_reference(insert_pos+1, insert_pos+K-1);
      if(new_kmer_segment.length()>=K){
	  for(int p=0; p<ins.length(); p++) {
	      std::string new_kmer = new_kmer_segment.substr(p,K);
		  if(new_kmer.length()==K){
		    std::cout << "Adding: " << new_kmer << " at " << insert_pos << std::endl;  
		    add_kmer_from_hash_at(insert_pos,new_kmer);
		  }
	    }
      }
/*	
	for(int p=insert_pos-edit_start+1; p<=insert_pos+ins.length()-edit_start; p++){
		std::string new_kmer = new_segment.substr(p,K);
		  if(new_kmer.length()==K){
		    std::cout << "Adding: " << new_kmer << " at " << insert_pos << std::endl;  
		    add_kmer_from_hash_at(insert_pos,new_kmer);
		  }
	      }
	  }
*/


    /*
    //Replace all the modified k-mers contained in edit_segment from the hashmap
    for(int i=0; i<edit_segment.length()-K; i++){
        if(i<=insert_pos){ //Check to handle insertions at the beginning of the genome
            std::string curr_kmer = edit_segment.substr(i,K);
            std::string new_kmer = new_segment.substr(i,K);
            if(curr_kmer!=new_kmer){
                std::cout << "Replacing: " << curr_kmer << " with " << new_kmer << " at " << edit_start+i << std::endl;
                if(curr_kmer.length()==K){
                    remove_kmer_from_hash_at(edit_start+i, curr_kmer);
                }
                if(new_kmer.length()==K && std::count(m[new_kmer].begin(), m[new_kmer].end(), edit_start+i)==0){
                    add_kmer_from_hash_at(edit_start+i, new_kmer);
                }
            }
        }
    }

    //Add the new k-mers generated due to insertion into the hashmap
    std::string new_kmer_segment = std::string(insertion+reference.substr(insert_pos+1,K-1)); //**why go to reference again?
    if(new_kmer_segment.length()>=K){
        for(int i=0; i<new_kmer_segment.length()-K+1; i++){
            std::string new_kmer = new_kmer_segment.substr(i,K);
            if(new_kmer.length()==K && std::count(m[new_kmer].begin(), m[new_kmer].end(), insert_pos)==0){
                std::cout << "Adding: " << new_kmer << " at " << insert_pos << std::endl;
                add_kmer_from_hash_at(insert_pos, new_kmer);
            }
        }
    }
*/
    s.print_list();
    if(is_edit[insert_pos]){
	s.insert_and_update(insert_pos, offset, ins);
        //n->str = nested_insertion;
        //n->offset +=  insertion.length();
    } else {
        s.insert_and_update(insert_pos, offset, insertion); //Insert an entry into the skip list
        is_edit[insert_pos]=true; //Set the bit to true if the location consists of an edit
    }
    s.print_list();

}
