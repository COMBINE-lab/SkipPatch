#ifndef GENOME_H
#define GENOME_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#define K 4 //user defined parameter

class genome {

private:

    std::string reference;
    std::unordered_map<std::string, std::vector<long>> m;

public:

    void get_input();
    void set_reference(std::string);
    long get_length();
        
    void construct_hash();
    float get_load_factor();
    
    void display_genome();
    void display_hash();
    void display_load();
    
    void remove_kmer_from_hash_at(long, std::string);
    void add_kmer_from_hash_at(long, std::string);
    
    bool snp_at(long, long, std::string);
    
    void insert_at(std::string, long);

    std::vector<long> find(std::string); 
};

#endif
