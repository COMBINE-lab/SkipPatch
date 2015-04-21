#include <bits/stdc++.h>
#include <iostream>
#include <cassert>

#include "skip_list.h"
#include "skip_list_test.h"

void test_find_ordered(){

    skip_list s;

    s.insert_and_update(34,1);
    s.insert_and_update(78,1);
    s.insert_and_update(144,1);
    s.insert_and_update(909,1);
    s.insert_and_update(1209,1);

    assert(s.find(78));
    assert(s.find(1209));
    assert(s.find(909));
    assert(s.find(34));
    assert(s.find(144));

    assert(!s.find(232));
    assert(!s.find(-22));
    assert(!s.find(0));

}

void test_find_unordered(){

    skip_list s;

    s.insert_and_update(78,1);
    s.insert_and_update(1209,1);
    s.insert_and_update(909,1);
    s.insert_and_update(144,1);
    s.insert_and_update(34,1);

    assert(s.find(34));
    assert(s.find(78));
    assert(s.find(144));
    assert(s.find(909));
    assert(s.find(1209));

    assert(!s.find(232));
    assert(!s.find(-22));
    assert(!s.find(0));

}

void test_find(){

    test_find_ordered();
    test_find_unordered();

}

void test_find_prev_ordered(){

    skip_list s;

    s.insert_and_update(34,1);
    s.insert_and_update(78,1);
    s.insert_and_update(144,1);
    s.insert_and_update(909,1);
    s.insert_and_update(1209,1);

    assert(s.find_prev(34)  -> val == LONG_MIN);
    assert(s.find_prev(78)  -> val == 34);
    assert(s.find_prev(144) -> val == 78);
    assert(s.find_prev(909) -> val == 144);
    assert(s.find_prev(1209)-> val == 909);

    assert(s.find_prev(20)  -> val == LONG_MIN);
    assert(s.find_prev(50)  -> val == 34);
    assert(s.find_prev(100) -> val == 78);
    assert(s.find_prev(150) -> val == 144);
    assert(s.find_prev(1000)-> val == 909);
    assert(s.find_prev(1500)-> val == 1209);

}

void test_find_prev_unordered(){

    skip_list s;

    s.insert_and_update(78,1);
    s.insert_and_update(1209,1);
    s.insert_and_update(909,1);
    s.insert_and_update(144,1);
    s.insert_and_update(34,1);

    assert(s.find_prev(34)  -> val == LONG_MIN);
    assert(s.find_prev(78)  -> val == 34);
    assert(s.find_prev(144) -> val == 78);
    assert(s.find_prev(909) -> val == 144);
    assert(s.find_prev(1209)-> val == 909);

    assert(s.find_prev(20)  -> val == LONG_MIN);
    assert(s.find_prev(50)  -> val == 34);
    assert(s.find_prev(100) -> val == 78);
    assert(s.find_prev(150) -> val == 144);
    assert(s.find_prev(1000)-> val == 909);
    assert(s.find_prev(1500)-> val == 1209);

}

void test_find_prev(){

    test_find_prev_ordered();
    test_find_prev_unordered();
}

void test_get_cumulative_count(){

    skip_list s;

    s.insert_and_update(78,5);
    s.insert_and_update(1209,2);
    s.insert_and_update(909,3);
    s.insert_and_update(144,1);
    s.insert_and_update(34,6);

    assert(s.get_cumulative_count(34)==0);
    assert(s.get_cumulative_count(78)==6);
    assert(s.get_cumulative_count(144)==11);
    assert(s.get_cumulative_count(909)==12);
    assert(s.get_cumulative_count(1209)==15);

    assert(s.get_cumulative_count(45)==-1);
    assert(s.get_cumulative_count(238)==-1);
    assert(s.get_cumulative_count(900)==-1);

}

void test_find_and_update_prev_ordered(){

    skip_list s;

    s.insert_and_update(34,1);
    s.insert_and_update(78,1);
    s.insert_and_update(144,1);
    s.insert_and_update(909,1);
    s.insert_and_update(1209,1);

    assert(s.find_and_update_prev(34,1)  -> val == LONG_MIN);
    assert(s.find_and_update_prev(78,1)  -> val == 34);
    assert(s.find_and_update_prev(144,1) -> val == 78);
    assert(s.find_and_update_prev(909,1) -> val == 144);
    assert(s.find_and_update_prev(1209,1)-> val == 909);

    assert(s.find_and_update_prev(20,1)  -> val == LONG_MIN);
    assert(s.find_and_update_prev(50,1)  -> val == 34);
    assert(s.find_and_update_prev(100,1) -> val == 78);
    assert(s.find_and_update_prev(150,1) -> val == 144);
    assert(s.find_and_update_prev(1000,1)-> val == 909);
    assert(s.find_and_update_prev(1500,1)-> val == 1209);

}

void test_find_and_update_prev_unordered(){

    skip_list s;

    s.insert_and_update(78,1);
    s.insert_and_update(1209,1);
    s.insert_and_update(909,1);
    s.insert_and_update(144,1);
    s.insert_and_update(34,1);

    assert(s.find_and_update_prev(34,1)  -> val == LONG_MIN);
    assert(s.find_and_update_prev(78,1)  -> val == 34);
    assert(s.find_and_update_prev(144,1) -> val == 78);
    assert(s.find_and_update_prev(909,1) -> val == 144);
    assert(s.find_and_update_prev(1209,1)-> val == 909);

    assert(s.find_and_update_prev(20,1)  -> val == LONG_MIN);
    assert(s.find_and_update_prev(50,1)  -> val == 34);
    assert(s.find_and_update_prev(100,1) -> val == 78);
    assert(s.find_and_update_prev(150,1) -> val == 144);
    assert(s.find_and_update_prev(1000,1)-> val == 909);
    assert(s.find_and_update_prev(1500,1)-> val == 1209);

}

void test_find_and_update_prev(){
	
	test_find_and_update_prev_ordered();
	test_find_and_update_prev_unordered();
}

void test_skip_list(){

    std::cout <<  std::endl << "Testing Skip List.." << std::endl;

    test_find();
    std::cout << "find(): Passed All Test Cases!" << std::endl;

    test_find_prev();
    std::cout << "find_prev(): Passed All Test Cases!" << std::endl;

    test_get_cumulative_count();
    std::cout << "get_cumulative_count(): Passed All Test Cases!" << std::endl;

    test_find_and_update_prev();
    std::cout << "test_find_and_update_prev(): Passed All Test Cases!" << std::endl;

    std::cout << "Skip List: Passed All Test Cases!" << std::endl << std::endl ;

}