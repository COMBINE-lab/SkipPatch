#ifndef SKIP_LIST_H
#define SKIP_LIST_H
#include <bits/stdc++.h>
struct node
{
    long val,offset;
    node *next,*down,*prev,*up;
    std::vector<int> invalid_pos;
    std::string str;
    node(long i,long o)
    {
        val = i;
        offset = o;
        next=down=prev=up=NULL;
    }
};

//must use short? instead of long for offset
class skip_list {

private:
 
   node *head,*tail;
   int val;
   int index;

public:

    skip_list();
    node* get_head();
    node* find(long); //obsolete, only for testing //if not found return null, else return the node itself.
    node *find_prev(long); //obsolete
    void insert(long,long); //obsolete
    void print_base_level();
    void print_list();
    void print_node(node*);
    node* find_and_update_prev(long,std::string);
    node* find_and_update_prev(unsigned long,unsigned long);
    void insert_and_update(long,unsigned long,std::string);
    long get_cumulative_count(long);
    void insert_and_update_abs(const long,std::string); //updates given absolute value
    void get_prev_node(long,long&,unsigned long&,node**);
    void delete_and_update(unsigned long,unsigned long,unsigned long);
    //long get_offset(node*);
};

#endif