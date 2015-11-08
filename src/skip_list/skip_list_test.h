#ifndef SKIP_LIST_TEST_H
#define SKIP_LIST_TEST_H

#include <string>

class skip_list;

using namespace std;
void test_find_ordered();
void test_find_unordered();
void test_find();
void test_get_cumulative_count();
void check_skip_list_node(skip_list s,const long &index,const string &ins);
void test_skip_list();

void test_delete_and_update_abs_standalone();
void test_delete_and_update_abs_insertion_standalone();
void test_delete_and_update_abs_insertion_overlap();
void test_delete_and_update_abs();

#endif

