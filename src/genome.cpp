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

    ins = std::vector<bool>(get_length(),false);
    del = std::vector<bool>(get_length(),false);
}

void genome::set_reference(std::string input)
{    
    reference = input;
    ins = std::vector<bool>(get_length(),false);
    del = std::vector<bool>(get_length(),false);
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

//  return read_reference_at(0,0,get_length());

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
    for(auto it=reference.begin();it<=reference.end()-K;it++){    
        std::string temp(it,it+K);
        if (!(temp.find('n')!=std::string::npos)  && !(temp.find('N')!=std::string::npos)) 
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

void genome::display_updated_genome()
{
    std::cout<<get_updated_reference()<<std::endl;
}

void genome::display_hash()
{   
    std::cout<<std::endl<<"Hash Table: "<<std::endl;
    for(auto it=m.begin();it!=m.end();it++){
        std::cout<<it->first<<"\t";
        for(auto vector_it=(it->second).begin();vector_it!=(it->second).end();vector_it++){
            std::cout<<*vector_it<<" ";
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}

void genome::display_load()
{
    std::cout << "size = " << m.size() << std::endl;
    std::cout << "bucket_count = " << m.bucket_count() << std::endl;
    std::cout << "max_load_factor = " << m.max_load_factor() << std::endl; 
}

void genome::remove_kmer_from_hash_at(long position_to_remove, std::string curr_kmer){

    if(!m[curr_kmer].empty()){
        m[curr_kmer].erase(std::find(m[curr_kmer].begin(), m[curr_kmer].end(), position_to_remove));
    }
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

long genome::get_virtual_position_from_genome_position(long genome_position, long offset) //arguement must be a tuple
{   
    return genome_position+s.get_cumulative_count(genome_position)+offset;
}

/*
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

    //std::string new_string(new_str.begin(),new_str.end());
    //if(new_string==""){
    //	new_string = generate_random_string(len);
    //}
    //len = new_string.length();
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
        
        //This is to temporarily handle the case when the k-mer is NNNNN..(k), or, contains an 'n' or 'N'
        //Since this k-mer exists at several (millions) of locations, "remove_kmer_from_hash" causes a bottleneck, slowing down the update process  
        //Temporary fix: Simple ignore the k-mer and return false. Permanent soultion?
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

}*/

void genome::snp_at(const long snp_pos_abs, const std::string snp) {

    const long snp_len = snp.length();

    auto kmer_pos_pair = get_kmers(snp_pos_abs-K+1,snp_len+K-1);
    int i=0;

    long genome_position;
    
    long after_start = std::min(snp_pos_abs+snp_len,get_length()-K); //Doesn't quite fix the edge cases yet
    long before_start = std::max(snp_pos_abs-K+1,(long)0); //Doesn't quite fix the edge cases yet
    const std::string after_snp = read_reference_abs_at(after_start,K-1,genome_position);
    const std::string before_snp = read_reference_abs_at(before_start,K-1,genome_position);

    std::string snp_update = before_snp + snp + after_snp;
    //Print to check why edge cases don't work
    //std::cout << before_snp << " + " << snp << " + " << after_snp << " = " << snp_update << endl ;

    for(auto it:kmer_pos_pair)
    {   
        remove_kmer_from_hash_at(it.second,it.first);
        string new_kmer = snp_update.substr(i,K);
        if(it.first.length()==K){
            add_kmer_from_hash_at(it.second,new_kmer);
        }
        //std::cout << "Replaced " << it.first << " with " << new_kmer << " at " << it.second << std::endl;
        
        i++;
    }

    //Update the genome itself
    //At every SNP, investigate if it is inside an inserted segment 
    //This should also take care of the cases where a series of SNPs could span several insertions
    unsigned long offset; node* n;
    int j=0;
    for(int j=0; j<snp_len; j++){
        get_genome_position_from_virtual_position(snp_pos_abs+j, genome_position, offset, &n);
        if(ins[genome_position]){
            n->str = (n->str).replace(offset-1,1,string(1,snp[j]));
        } else {
            reference[genome_position] = snp[j]; //No insertion at that point
        }
    }

}

std::vector<long> genome::search(std::string read){

	std::vector<long> positions;

	//Read the first k-mer of the read and find all the positions it occurrs at
	std::string read_kmer = read.substr(0,K); 
	
    auto search = m.find(read_kmer);
    if(search !=m.end()){
    	for(long pos: search->second){

    		long offset = 0;
    		//If the position doesn't start from within an insertion
    		if(!ins[pos]){
    			if(read==read_reference_at(pos,offset,read.length())){
    				positions.push_back(get_virtual_position_from_genome_position(pos,offset));
    			}
    		} else {
    			std::string read_ext = read_reference_at(pos,offset,read.length()+(s.find(pos)->str).length());
    			//A read can occurr multiple times within an insertion itself
    			//And hence might have different offsets for those locations 	
    			//Eureka! It works! ;) And that's how horrible some test cases can get :P
    			offset = read_ext.find(read); 
    			while(offset!=string::npos){
    				if(read==read_reference_at(pos,offset,read.length())){
    					positions.push_back(get_virtual_position_from_genome_position(pos,offset));
    				}
    				offset = read_ext.find(read,offset+1);
    			}
    		}
    	}
    }
	return positions;
}

//Get kmer_count number of k-mers starting at updated position start
vector<pair<string,long>> genome::get_kmers(const long start,const unsigned long kmer_count) //edge cases must be handled
{

	vector<pair<string,long> > vec;
	for(long i=start;i<start+kmer_count;i++) //could be faster
	{
		long genome_position;
		string kmer = read_reference_abs_at(i,K,genome_position);
		vec.push_back(make_pair(kmer,genome_position));
	}
	return vec;
}

string genome::read_reference_at(const long genome_position,const long offset,const long len)
{
    long rem_len=len,curr_genome_pos = genome_position,curr_offset = offset;string kmer;

    while(rem_len>0 && curr_genome_pos<=get_length()-1)
    {
        if(ins[curr_genome_pos])
        {
	
            if(!curr_offset)
	        {
                kmer+=reference[curr_genome_pos];
                rem_len--;
                curr_offset=1;
            }
            else
        	{
            	node *n = s.find(curr_genome_pos);
            	string temp(n->str,curr_offset-1,rem_len);
            	kmer+=temp;
            	rem_len-=temp.length();
	
            	if(!rem_len)
                    break;
            	curr_genome_pos++;
            	curr_offset=0;
        	}
        }
        else if(del[curr_genome_pos])
	{
	  curr_genome_pos++;
	}
        else
        {
            kmer+=reference[curr_genome_pos];
            rem_len--;
            curr_genome_pos++;
            curr_offset=0;
        }
    }
    return kmer;
}

string genome::read_reference_abs_at(const long abs_pos,const long len,long &genome_position)
{
	unsigned long offset;node* n;string kmer;
	get_genome_position_from_virtual_position(abs_pos, genome_position, offset, &n);
    kmer = read_reference_at(genome_position,offset,len);
	return kmer;
}

/*
Inserts a string at the 'insert_pos'. Updates the k-mers which lie in the "region of change" 
Splits the problem into two segments:
 - Updating the k-mers which begin at a point before the insertion
 - Adding the new k-mers which are a result of the insertion (begin at a point in the inserted segment)
*/
void genome::insert_at(const std::string insertion, const unsigned long insert_pos_abs){

	const long ins_len = insertion.length();

	auto kmer_pos_pair = get_kmers(insert_pos_abs-K+2,K-1);
	int i=0;

	long genome_position;
	const string end_kmer = read_reference_abs_at(insert_pos_abs+1,K-1,genome_position);
	string ins_copy = insertion+end_kmer;

    //std::cout << insert_pos_abs << " " << genome_position << std::endl;

	for(auto it:kmer_pos_pair)
	{   
		remove_kmer_from_hash_at(it.second,it.first);
		string temp = it.first.substr(0,K-i-1) + ins_copy.substr(0,i+1);
		if(it.first.length()==K){ //For handling edge cases: (insertion at the end of the genome)
            add_kmer_from_hash_at(it.second,temp);
        }
		i++;
	}
	
    long add_pos = genome_position;
    if(!kmer_pos_pair.empty()){ //For handling edge cases: (insertion at the beginning of the genome)
        add_pos = kmer_pos_pair.back().second;
    }
	
    for(i=0;i<insertion.length();i++)
	{
		string temp = insertion.substr(i,K);
		string temp2 = end_kmer.substr(0,K-temp.length());
		if((temp+temp2).length()==K){ //For handling edge cases: (insertion at the end of the genome)
            add_kmer_from_hash_at(add_pos,temp+temp2);
        }
	}

	//Set the ins bit to true
	ins[add_pos]=1;

	//Update the skip list
	s.insert_and_update_abs(insert_pos_abs,insertion);

}

void genome::delete_at(const unsigned long delete_pos_abs, const unsigned long del_len){

	auto kmers_to_replace = get_kmers(delete_pos_abs-K+1,K-1);
	int i=0;

	long genome_position;
	const string end_kmer = read_reference_abs_at(delete_pos_abs+del_len,K-1,genome_position);
    genome_position-=del_len;
    //std::cout << delete_pos_abs+del_len << " " << genome_position << std::endl;

	for(auto it:kmers_to_replace)
	{
		remove_kmer_from_hash_at(it.second,it.first);
		string temp = it.first.substr(0,K-i-1) + end_kmer.substr(0,i+1);
		if(it.first.length()==K){ //For handling edge cases: (insertion at the end of the genome)
            add_kmer_from_hash_at(it.second,temp);
        }
        //std::cout << "Replaced " << it.first << " with " << temp << " at " << it.second << std::endl;
		i++;
	}

    auto kmers_to_delete = get_kmers(delete_pos_abs,del_len);

    for(auto it: kmers_to_delete){
        remove_kmer_from_hash_at(it.second,it.first);    
        //std::cout << "Removed " << it.first << " from " << it.second << std::endl;
    }
	
	//Set the ins bit to true
	for(int j=0; j<del_len; j++){
    	del[genome_position+j]=1;
	}

	//Update the skip list
	s.delete_and_update_abs(delete_pos_abs,del_len);

}

