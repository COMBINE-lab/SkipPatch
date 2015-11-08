#ifndef GENOME_H
#define GENOME_H

#include <utility>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

#include "./skip_list/skip_list.h"

#define K 20 //user defined parameter
#define S 1

class genome {

private:

    std::string reference;
    std::unordered_map<uint64_t, std::vector<long>> m;
    //std::unordered_map<std::string, std::vector<long>, std::function<unsigned long(std::string)>> m;
    skip_list s;
    std::vector<bool> ins;
    std::vector<bool> del;
    //long length;

public:

    genome();

    //Get the reference sequence from input
    void get_input(std::string);
    void check_node_correctness(long,long);
    //Getters and setters
    void set_reference(std::string);
    std::string get_reference();
    long get_length();
    std::unordered_map<uint64_t, std::vector<long>>& get_hash();
    //std::unordered_map<std::string, std::vector<long>, std::function<unsigned long(std::string)>> get_hash();
    skip_list get_skip_list();

    //Generate the updated reference, or a segment of the updated reference
    std::string read_reference_at(const long ,const long,const long);
    std::string read_reference_abs_at(const long,const long,long &);
    void read_reference_abs_at(const long ,const long ,long&,unsigned long&,std::string&);

    //Hash table functions
    void load_hash(const std::string& fname);
    void save_hash(const std::string& fname);
    void construct_hash();
    float get_load_factor();

    //Display functions
    void display_genome();
    void display_updated_genome();
    void display_hash();
    void display_load();

    //Modify the hash table to add/remove positions at which a k-mer occurs
    void remove_kmer_from_hash_at(long, std::string);
    void add_kmer_to_hash_at(long, std::string);

    //Translating positions between the genome and virtual coordinate systems
    long get_genome_position_from_virtual_position(long);
    void get_genome_position_from_virtual_position(long, long &, unsigned long &, node**);
    long get_virtual_position_from_genome_position(long,long);

    //In-place update: Replace the segment in the reference at a particular point with the given variant
    void snp_at(long, std::string);

    //Make an insertion at a given location
    void insert_at(const std::string, const unsigned long);

    //Make a deletion at a given location
    bool delete_at(const unsigned long, const unsigned long);

    //Search for all the occurrences of a read
    std::vector<long> search(std::string);

    //Get kmers
    std::vector<std::string> generate_kmers(std::string);
    std::vector<std::pair<std::string,long>> get_kmers(const long, const unsigned long);
    std::vector<std::tuple<std::string,long,unsigned long>> get_kmers_with_offset(const long,const unsigned long );

    bool is_kmer_hashed(std::string kmer, long position);
};

#endif
