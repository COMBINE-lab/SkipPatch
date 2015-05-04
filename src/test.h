#ifndef TEST_H
#define TEST_H

#include <iostream>

void test_hash_fwd(std::unordered_map<std::string, std::vector<long>>,std::unordered_map<std::string, std::vector<long>>,skip_list);
void test_hash_rev(std::unordered_map<std::string, std::vector<long>> ,std::unordered_map<std::string, std::vector<long>> ,skip_list);
void test_hash(std::unordered_map<std::string, std::vector<long>>,std::unordered_map<std::string, std::vector<long>>,skip_list);

//void check_insert_at(genome, std::string, long, std::string&);
void test_insert_at();

void check_search();
void test_search();

void test_search_static_reference();
void test_search_dynamic_reference();

void test_snp_at();

//void check_delete_at(genome&, long, long, std::string&);
void test_delete_at();

void test();

#endif