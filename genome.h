#ifndef GENOME_H
#define GENOME_H

#include <iostream>
#include <list>
#include <string>
#include <unordered_map>

using namespace std;

#define K 3 //user define parameter

class genome {

private:
    string reference;
    unordered_map<string,list<long>> m;

public:
    void get_input();
    void set_reference(std::string);
    long get_length();
        
    void construct_hash();
    float get_load_factor();
    
    void display_genome();
    void display_hash();
    void display_load();
    
    void remove_kmer_from_hash_at(long,string);
    void add_kmer_from_hash_at(long,string);

    
    bool snp_at(long a,long b,string str="");
    
    std::list<long> find(std::string); 
};

#endif
