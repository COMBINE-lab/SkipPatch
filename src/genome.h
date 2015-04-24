#ifndef GENOME_H
#define GENOME_H

#include "./skip_list/skip_list.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#define K 4 //user defined parameter

class genome {

private:

    std::string reference;
    std::unordered_map<std::string, std::vector<long>> m;
    skip_list s;
    std::vector<bool> is_edit;

public:

    //Get the reference sequence from input
    void get_input();

    //Getters and setters
    void set_reference(std::string);
    std::string get_reference();
    std::string get_updated_reference();
    long get_length();
    std::unordered_map<std::string, std::vector<long>> get_hash();
    skip_list get_skip_list();

    //Hash table functions
    void construct_hash();
    float get_load_factor();
    
    //Display functions
    void display_genome();
    void display_hash();
    void display_load();
    
    //Modify the hash table to add/remove positions at which a k-mer occurs
    void remove_kmer_from_hash_at(long, std::string);
    void add_kmer_from_hash_at(long, std::string);
        
    //In-place update: Replace the segment in the reference at a particular point with the given variant
    void snp_at(long, std::string);
    
    //Make an insertion at a given location
    void insert_at(std::string, long);
    
    //Search for all the occurrences of a read
    std::vector<long> find(std::string);

    //void check_hash();
};

#endif
