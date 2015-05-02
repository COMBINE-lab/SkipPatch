#ifndef SKIP_LIST_TEST_H
#define SKIP_LIST_TEST_H

using namespace std;
void test_find_ordered();
void test_find_unordered();
void test_find();
void test_get_cumulative_count();
void test_find_and_update_prev_ordered();
void test_find_and_update_prev_unordered();
void test_find_and_update_prev();
void check_skip_list_node(skip_list s,const long &index,const string &ins);
void test_skip_list();

#endif
