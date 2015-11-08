#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include <string>
#include <vector>

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

struct mod_kmers{
	long initial_genome_pos;
	long initial_offset;
	long final_genome_pos;
	long final_offset;
	long num_kmers_moved;
	std::vector<long> insertions;
	std::vector<long> deletions;

	mod_kmers(long l1,long l2,long l3,long l4,long l5, std::vector<long> ins, std::vector<long> del){
		initial_genome_pos=l1;
		initial_offset=l2;
		final_genome_pos=l3;
		final_offset=l4;
		num_kmers_moved=l5;
		insertions = ins;
		deletions= del;
	}

	mod_kmers(){
			initial_genome_pos=-1;
			initial_offset=-1;
			final_genome_pos=-1;
			final_offset=-1;
			num_kmers_moved=-1;
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
    node* naive_find(long); //obsolete, only for testing //if not found return null, else return the node itself.
    void print_base_level();
    void print_list();
    void print_node(node*);
    void insert_and_update(long,unsigned long,std::string);
    long get_cumulative_count(long);
    void insert_and_update_abs(const long,std::string); //updates given absolute value
    void get_prev_node(long,long&,unsigned long&,node**);
    void get_stats(long &,long &);
    //void delete_and_update_abs(unsigned long,unsigned long,unsigned long);
    bool is_valid_delete(const long ,const unsigned long );
    mod_kmers delete_and_update_abs(const long ,const unsigned long);

    //long get_offset(node*);
};

#endif
