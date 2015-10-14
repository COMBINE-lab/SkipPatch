#include <bits/stdc++.h>
#include <iostream>
#include <cassert>

#include "skip_list.h"
#include "skip_list_test.h"
#include "utils.h"

void test_find_ordered(){

    skip_list s;

    s.insert_and_update(34,0,"AT");
    s.insert_and_update(78,0,"C");
    s.insert_and_update(144,0,"GG");
    s.insert_and_update(909,0,"ATCG");
    s.insert_and_update(1209,0,"CAT");

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

    s.insert_and_update(78,0,"C");
    s.insert_and_update(1209,0,"CAT");
    s.insert_and_update(909,0,"ATCG");
    s.insert_and_update(144,0,"GG");
    s.insert_and_update(34,0,"AT");

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

void test_get_cumulative_count(){

    skip_list s;

    s.insert_and_update(78,0,"C");
    s.insert_and_update(1209,0,"CAT");
    s.insert_and_update(909,0,"ATCG");
    s.insert_and_update(144,0,"GG");
    s.insert_and_update(34,0,"AT");

    assert(s.get_cumulative_count(34)==0);
    assert(s.get_cumulative_count(78)==2);
    assert(s.get_cumulative_count(144)==3);
    assert(s.get_cumulative_count(909)==5);
    assert(s.get_cumulative_count(1209)==9);
    
    assert(s.get_cumulative_count(12)==0);
    assert(s.get_cumulative_count(238)==5);
    assert(s.get_cumulative_count(900)==5);
    assert(s.get_cumulative_count(909)==5);
    assert(s.get_cumulative_count(910)==9);
    assert(s.get_cumulative_count(2000)==12);

}


void test_insert_and_update_abs()
{
    skip_list s;

    s.insert_and_update(2,0,"A");
    s.insert_and_update(5,0,"GC");
    s.insert_and_update(11,0,"T");
    s.insert_and_update(13,0,"AG");

    s.insert_and_update_abs(2,"TT");  //insertion at first node; at the beginning
    assert(s.find(2)->str=="TTA");
    s.insert_and_update_abs(5,"G");   //insertion at first node; at the end
    assert(s.find(2)->str=="TTAG"); 
    s.insert_and_update_abs(12,"G");  //insertion at middle node; new node
    assert(s.find(6)->str=="G"); 
    s.insert_and_update_abs(13,"G");  //insertion at middle node; at the end
    assert(s.find(6)->str=="GG"); 
    s.insert_and_update_abs(26,"GG");  //insertion at middle node; at the end
    assert(s.find(15)->str=="GG"); 

}


void check_skip_list_node(skip_list s,const long &index,const string &ins) //checks wether the string at some absolute index(which has an insertion) is same as ins
{
  
    unsigned long pos;
    long val;
    node *n;
    string str;
    //s.insert_and_update_abs(2,ins); //insertion at first node; at the beginning
    //assert(s.find(index)->str=="TTA");
    //check_skip_list_node(s,2,"TT");
    s.get_prev_node(index,val,pos,&n);
    str = n->next->str;
    assert(str.substr(pos,pos+ins.length())==ins);
}

void test_get_prev_node()
{
    skip_list s;

    s.insert_and_update(2,0,"A");
    s.insert_and_update(5,0,"GC");
    s.insert_and_update(11,0,"T");
    s.insert_and_update(13,0,"AG");

    s.insert_and_update_abs(2,"TT");  //insertion at first node; at the beginning
    assert(s.find(2)->str=="TTA");
    check_skip_list_node(s,2,"TT");

    s.insert_and_update_abs(5,"G");
    assert(s.find(2)->str=="TTAG");   //insertion at first node; at the end
    check_skip_list_node(s,5,"G");

    s.insert_and_update_abs(12,"G");
    assert(s.find(6)->str=="G");      //insertion at middle node; new node
    check_skip_list_node(s,12,"G");

    s.insert_and_update_abs(13,"G");
    assert(s.find(6)->str=="GG");     //insertion at middle node; at the end
    check_skip_list_node(s,13,"G");

    s.insert_and_update_abs(26,"GG");
    assert(s.find(15)->str=="GG");    //insertion at middle node; at the end
    check_skip_list_node(s,26,"GG");

}

void test_delete_and_update_abs_standalone(){

    skip_list s;

    s.delete_and_update_abs(2,1);
    s.print_list();
    assert(s.find(2)->str=="");
    assert(s.find(2)->offset==-1); 
    //LOGDEBUG(FILE_LOGGER, "test_delete_and_update_abs_standalone(): Passed All Test Cases!");
    
    s.delete_and_update_abs(8,2);
    s.print_list();
    assert(s.find(9)->str==""); 
    assert(s.find(9)->offset==-2); 
    
    s.delete_and_update_abs(20,2);
    s.print_list();
    assert(s.find(23)->str==""); 
    assert(s.find(23)->offset==-2);

    s.delete_and_update_abs(15,2);
    s.print_list();
    assert(s.find(18)->str==""); 
    assert(s.find(18)->offset==-2); 

}

void test_delete_and_update_abs_insertion_standalone(){

    skip_list s;

    s.insert_and_update_abs(4,"TTA");
    s.insert_and_update_abs(12,"AG");
    s.insert_and_update_abs(18,"CGAT");
    s.insert_and_update_abs(26,"A");

    s.delete_and_update_abs(2,1); //Deletion before all insertions (no overlap)
    assert(s.find(2)->str=="");
    assert(s.find(2)->offset==-1);
    assert(s.find(4)->str=="TTA"); 

    s.delete_and_update_abs(8,2); //Deletion in between insertions (no overlap)
    assert(s.find(6)->str==""); 
    assert(s.find(6)->offset==-2); 
    assert(s.find(9)->str=="AG"); 
    
    s.delete_and_update_abs(13,1); //Deletion in between insertions (no overlap)
    assert(s.find(11)->str=="");
    assert(s.find(11)->offset==-1); 
    assert(s.find(13)->str=="CGAT"); 

    s.delete_and_update_abs(24,1); //Deletion after all insertions (no overlap)
    assert(s.find(18)->str==""); 
    assert(s.find(17)->str=="A"); 

}

void test_delete_and_update_abs_insertion_overlap(){

    skip_list s;
    LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap()");
    s.insert_and_update_abs(4,"TTA");
    s.insert_and_update_abs(12,"AG");
    s.insert_and_update_abs(18,"CGAT");
    s.insert_and_update_abs(26,"A");

    LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap(),after insertions");
    s.delete_and_update_abs(5,1); //Deletion within insertions (overlap)

    LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap(),after first deletion");
    assert(s.find(4)->str=="TA"); 
    assert(s.find(4)->offset==2); 
    s.delete_and_update_abs(5,1); //Deletion within insertions (overlap)

    LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap(),after s.delete_and_update_abs(5,1)");
    assert(s.find(4)->str=="A");
    assert(s.find(4)->offset==1); 

    //s.print_list();
    s.delete_and_update_abs(10,2);
    LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap(),after s.delete_and_update_abs(10,2)");
    assert(s.find(9)->str==""); 
    assert(s.find(9)->offset==-1);

    s.print_list();
    assert(s.find(8)->str=="G");
    assert(s.find(8)->offset==1);

    s.delete_and_update_abs(14,6); //Deletion within insertions (overlap extending into genome) 
    LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap(),after s.delete_and_update_abs(14,6)");
    s.print_list();
    assert(s.find(13)->str=="");
    assert(s.find(13)->offset==-2);

    s.insert_and_update_abs(14,"AGC");
    LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap(),after s.delete_and_update_abs(14,AGC)");
    s.print_list();
    assert(s.find(15)->str=="AGC");
    assert(s.find(15)->offset==3);

    s.delete_and_update_abs(15,2);
    LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap(),after s.delete_and_update_abs(15,-2)");
    s.print_list();
    assert(s.find(15)->str=="C");
    assert(s.find(15)->offset==1);

    s.delete_and_update_abs(15,1);
    LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap(),after s.delete_and_update_abs(15,-1)");
    s.print_list();
    assert(s.find(15)->str=="");
    assert(s.find(15)->offset==0);

    s.insert_and_update_abs(14,"T");
	s.print_list();

	s.delete_and_update_abs(14,1);
    LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap(),after s.delete_and_update_abs(15,-1)");
    s.print_list();
    assert(s.find(12)->str=="T");
    assert(s.find(12)->offset==1);

    s.insert_and_update_abs(11,"AGCT");
	LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap(),after s.delete_and_update_abs(15,-1)");
	s.print_list();
	assert(s.find(10)->str=="AGCT");
	assert(s.find(10)->offset==4);

	s.delete_and_update_abs(13,5);
	LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap(),after s.delete_and_update_abs(15,-1)");
	s.print_list();
	assert(s.find(10)->str=="AT");
	assert(s.find(10)->offset==2);

	s.insert_and_update_abs(14,"AGC");
	s.print_list();

	s.insert_and_update_abs(17,"AGC");
	s.print_list();

	s.insert_and_update_abs(7,"AGC");
	s.delete_and_update_abs(7,2);
	s.print_list();
	assert(s.find(6)->str=="");
	assert(s.find(6)->offset==-1);
	assert(s.find(5)->str=="GC");
	assert(s.find(5)->offset==2);

	s.delete_and_update_abs(7,5);
	LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap(),after s.delete_and_update_abs(15,-1)");
	s.print_list();

	s.delete_and_update_abs(4,3);
	LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap(),after s.delete_and_update_abs(15,-1)");
	s.print_list();


	s.delete_and_update_abs(4,10);
	LOGDEBUG(FILE_LOGGER, "In test_delete_and_update_abs_insertion_overlap(),after s.delete_and_update_abs(15,-1)");
	s.print_list();



}

void test_delete_and_update_abs(){

    test_delete_and_update_abs_standalone();
    test_delete_and_update_abs_insertion_standalone();
    test_delete_and_update_abs_insertion_overlap();

}

void test_skip_list(){

    std::cout <<  std::endl << "Testing Skip List.." << std::endl;

    test_find();
    LOGINFO(FILE_LOGGER, "find(): Passed All Test Cases!");

    test_get_cumulative_count();
    LOGINFO(FILE_LOGGER, "get_cumulative_count(): Passed All Test Cases!");

    test_insert_and_update_abs();
    LOGINFO(FILE_LOGGER, "test_insert_and_update_abs(): Passed All Test Cases!");

    test_get_prev_node();
    LOGINFO(FILE_LOGGER, "test_get_prev_node(): Passed All Test Cases!");

    test_delete_and_update_abs();
    LOGINFO(FILE_LOGGER, "test_delete_and_update_abs(): Passed All Test Cases!");

    LOGINFO(FILE_LOGGER, "Skip List: Passed All Test Cases!");

}
