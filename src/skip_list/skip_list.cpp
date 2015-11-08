#include "skip_list.h"

#include <algorithm>
#include <climits>
#include <cstdbool>
#include <cstdlib>
#include <iostream>

#include "../utils.h"

using namespace std;

//put this in utils
int coinToss() {
	return rand() % 2;
}

skip_list::skip_list() {
	node *bottom_head = new node(LONG_MIN, 0);
	node *bottom_tail = new node(LONG_MAX, 0);

	bottom_head->next = bottom_tail;
	bottom_tail->prev = bottom_head;

	head = bottom_head;
	tail = bottom_tail;
}

node* skip_list::find(long val) {
	node *temp = head;

	while (temp) {
		if (temp->next->val < val) {
			temp = temp->next;
		} else if (temp->next->val == val) {
			if (temp->down)
				temp = temp->down;
			else
				return temp->next;
		} else {
			temp = temp->down;
		}
	}
	return temp;
}

node* skip_list::naive_find(long val) {
	node *temp = head;
	while(temp->down){
		temp=temp->down;
	}
	while(temp){
		if(temp->val>val)
			return temp->prev;
		if(temp->val==val){
			return temp;
		}
		temp=temp->next;
	}
	return temp;
}

void skip_list::get_prev_node(long abs_val, long &val, unsigned long &pos,
		node **n) {

	node *temp = head;
	//node *prev;
	long cumulative_count = 0;
	pos = 0;
	val = 0;

	while (temp) {
		//cout<<"in here! cc = "<<cumulative_count<<" "<<abs_val<<" "<<temp->val<<endl;
		if (abs_val
				>= (temp->next->val + cumulative_count + temp->offset)&&temp->next->val!=LONG_MAX) {
			cumulative_count += temp->offset;
			temp = temp->next;
		}

		else	//current val is lesser than next val
		{
			if (temp->down) {
				//temp->offset+=offset;
				temp = temp->down;
			} else {
				//pair<node*,unsigned long> p;
				if (abs_val <= (temp->val + cumulative_count + temp->offset)) //existing node
						{
					//str = temp->str;
					*n = temp->prev;
					val = temp->val;
					pos = abs_val - cumulative_count - temp->val;
					//p.make_pair(temp->prev,val-cumulative_count);
					//return p;
				} else {
					*n = temp;
					val = abs_val - cumulative_count - temp->offset;
					pos = 0;
				}
				break;
			}
		}
	}

}
//TODO: remove this function after implementing generic deletions
bool skip_list::is_valid_delete(const long abs_val, const unsigned long len) {
	long val;
	unsigned long pos;
	node *prev;
	get_prev_node(abs_val, val, pos, &prev);

	if (prev->next->val == val) //if the value already exists
			{
		//update the offset pointers..
		if (((pos + len - 1) > (prev->next->str).length())
				|| (len > (prev->next->str).length())
				|| ((pos == 1) && (len >= (prev->next->str).length()))) {
			cout << ".";
			//  cout<<" len "<<len<<" (prev->next->str).length() "<<(prev->next->str).length()<<endl;
			// cout<<"Invalid value exists..pos = "<<pos<<" value "<<prev->next->val<<" Inconsistent value caught!"<<endl;
			return false;
		}

	}
	return true;
}

/*
 * inserts at initial position val, offset pos a string str.
 * If pos is LONG MAX, then the value is after the end of an insertion node
 */
void skip_list::insert_and_update(long val, unsigned long pos, string str) {
//find place to insertnode
//node *prev = find_and_update_prev(val,str);

	long offset = str.length();
	node *temp = head;
	node *prev;

	while (temp) {
		if (temp->next->val <= val) // next val is >= next ; = since a node contains offset between that node and the next.
				{
			temp = temp->next;
		} else  //current val is lesser than next val
		{
			if (temp->down) {
				temp->offset += offset;
				temp = temp->down;
			} else {
				if (temp->val == val) {
					prev = temp->prev;
					break;
				}
				prev = temp;
				break;
			}
		}
	}

//node *prev = find_and_update_prev(val,str);

	//offset = str.length();
	if (prev->next->val == val) //if the value already exists
			{
		//update the offset pointers..
		if (pos == LONG_MAX){
			pos = (prev->next->str).length();
		}
		if ((prev->next->str).length() < pos) { //sanity check
			LOGALERT(FILE_LOGGER,"Skip list inconsistent, pos> offset");
			return;
		}
		(prev->next->str).insert(pos, str);
		prev->next->offset += offset;
	} else {

		if (pos > 0 && pos!=LONG_MAX) { //sanity check
			LOGALERT(FILE_LOGGER,"Skip list inconsistent, pos > 0");
			return;
		}

		node *new_node = new node(val, offset);
		new_node->str = str;
		new_node->next = prev->next;
		new_node->prev = prev;
		prev->next = new_node;
		new_node->next->prev = new_node;

		//promote the node up.
		node *node_to_copy = new_node;
		while (coinToss()) {
			node *temp = node_to_copy;
			//find the closest upper level node.
			long offset_sum = 0; //keeps track of offset in that level
			while (temp) {
				if (temp->up) {
					//temp=temp->up;
					offset_sum += temp->offset;
					offset_sum -= node_to_copy->offset;
					break;
				}
				if (temp->val == LONG_MIN) {
					offset_sum += temp->offset;
					offset_sum -= node_to_copy->offset;
					break;
				}
				offset_sum += temp->offset;
				temp = temp->prev;

			}
			node *new_node_upper = new node(val, offset);
			//if the upper level exists
			if (temp->up) {

				temp = temp->up;
				long total_offset = temp->offset;
				new_node_upper->next = temp->next;
				new_node_upper->prev = temp;
				new_node_upper->next->prev = new_node_upper;
				temp->next = new_node_upper;

				temp->offset = offset_sum;
				new_node_upper->offset = total_offset - offset_sum;

			} else //create new level
			{
				//create 1 new nodes - 2 sentinel nodes.
				//node *new_node_upper =  new node(val,offset);

				node *new_node_upper_head = new node(LONG_MIN, 0);
				node *new_node_upper_tail = new node(LONG_MAX, 0);

				new_node_upper_head->next = new_node_upper;
				new_node_upper_head->down = head;

				new_node_upper->prev = new_node_upper_head;
				new_node_upper->next = new_node_upper_tail;

				new_node_upper_tail->prev = new_node_upper;
				new_node_upper_tail->down = tail;

				head->up = new_node_upper_head;
				tail->up = new_node_upper_tail;

				head = new_node_upper_head;
				tail = new_node_upper_tail;

				//get_total_offset
				long forward_offset = 0;
				node *temp_node_to_copy = node_to_copy;
				while (temp_node_to_copy) {
					forward_offset += temp_node_to_copy->offset;
					temp_node_to_copy = temp_node_to_copy->next;
				}
				long backward_offset = 0;
				temp_node_to_copy = node_to_copy;

				while (temp_node_to_copy->prev) {
					temp_node_to_copy = temp_node_to_copy->prev;
					backward_offset += temp_node_to_copy->offset;
				}
				new_node_upper_head->offset = backward_offset;
				new_node_upper->offset = forward_offset;

			}
			new_node_upper->down = node_to_copy;
			node_to_copy->up = new_node_upper;
			node_to_copy = new_node_upper;
		}
	}

}

/*
 * deletes len bases starting at current_pos (the position wrt to the updated genome) or until the end of the genome
 * TODO: should we bubble up the insertion?
 */
mod_kmers skip_list::delete_and_update_abs(const long abs_val,
		const unsigned long len) {
	long offset = len * -1;
	node *temp = head, *prev;
	long cumulative_count = 0, val = 0;
	unsigned long pos = 0;
//TODO: coalesce the deletions if they are consecutive - eg: 18, -3, 21,-4

	//find the previous element, prev -  if the initial(genome) position is present in the skip list, return that node, else return the one previous to it.
	//If there is no node previous to it, the sentinel is returned
	node *node_to_promote = NULL;

	long initial_genome_pos=0;
	long initial_offset=0;
	long final_genome_pos=0;
	long final_offset=0;
	long num_kmers_moved=0;
	vector<long> insertions;
	vector<long> deletions;
	mod_kmers m_kmers = mod_kmers();

	while (temp) {

		if ((abs_val
				> (temp->next->val + cumulative_count + temp->offset)&&temp->next->val!=LONG_MAX) ||
				((abs_val ==(temp->next->val + cumulative_count + temp->offset)) && (temp->next->offset<0) && (!(temp->down)) ))
						{
			cumulative_count += temp->offset;
			temp = temp->next;
		}

		else	//the current position (abs_val) lies b/w current node and next
		{
			if (temp->down) {
				node *trav = temp;
				while ((abs_val + len)
						> (cumulative_count + trav->offset + trav->next->val)) { //if the deletion touches the next node, remove it.

					//update the offset
					trav->offset += trav->next->offset;

					//delete the node
					trav->next = trav->next->next;
					trav->next->prev = trav;

				}
				temp->offset += offset;
				temp = temp->down;
			} else { //bottom level of the skip list
				LOGDEBUG(FILE_LOGGER,"Reached bottom level of skip list");
				string collect_ins = "";
				long initial_pos = 0;
				long remaining_deletion = len;
				long offset_copy = LONG_MIN;
				if(abs_val==4292896){
					cout<<"Hre";
				}

				if (abs_val <= (cumulative_count + temp->offset + temp->val)) { //the deletion starts from within an insertion
					//TODO: what to do when all the bases in of the inserted segment get deleted?
					if(abs_val==4292896){
						cout<<"Here!";
					}
					LOGDEBUG(FILE_LOGGER,"the deletion starts from within an insertion");
					long deletion_start_offset = abs_val - temp->val- cumulative_count - 1;
					long deletion_len  = len;
					deletion_len = min(deletion_len,
							temp->offset - deletion_start_offset);
					LOGDEBUG(FILE_LOGGER,"trying to erase the string: "+to_string(temp->next->offset)+" "+to_string(temp->next->val));
					LOGDEBUG(FILE_LOGGER,"trying to erase the string: "+to_string(temp->prev->offset)+" "+to_string(temp->prev->val));
					LOGDEBUG(FILE_LOGGER,"trying to erase the string: "+to_string(abs_val)+ " "+to_string(cumulative_count)+" "+to_string(temp->offset)+" "+to_string(temp->val));
					LOGDEBUG(FILE_LOGGER,"trying to erase the string: "+to_string(deletion_start_offset)+ " "+to_string(len));
					temp->str.erase(deletion_start_offset, len);
					offset_copy=temp->offset;
					temp->offset-=deletion_len;
					remaining_deletion -= deletion_len;

					if(remaining_deletion<=0){
						break;
					}
					//find out where to create the new node
					//node *trav = temp;
					initial_pos = temp->val+1;
					while(temp->next->offset<0){
						long next_valid_pos =(temp->offset*(-1)) + temp->val;
						if( next_valid_pos > temp->next->val){
							temp=temp->next;
						}
						else{
							if(temp->offset>=0){
								initial_pos = temp->val+1;
							}
							else{
								initial_pos = next_valid_pos;
							}
							break;
						}
					}

				} //TODO: change the variable names - temp and abs_pos, len, collect_ins

				else { //deletion starts from a position where there has been no insertion (or deletion) EXCEPT the case where the deletion starts from the a genome position (of a node) that has an insertion
						//which essentially means that a new deletion node must be created.
					LOGDEBUG(FILE_LOGGER,"the deletion starts from a genome position");
					initial_pos = abs_val - (cumulative_count + temp->offset);
				}

					cumulative_count = cumulative_count + max(temp->offset,offset_copy); //max - if the deletion begins from within insertion, the offset gets subtracted by len of deletion within offset
					node *new_node=NULL;
					if((temp->next->val == initial_pos) && temp->next->offset>0){
						LOGDEBUG(FILE_LOGGER,"the deletion starts from a genome position which has an insertion");
						if((abs_val + len) > (temp->next->val + temp->next->offset+ cumulative_count)) //the deletion spans over the next node
						{
							LOGDEBUG(FILE_LOGGER,"the deletion spans over the next node");
							remaining_deletion-=temp->next->offset;
							//deletions.push_back(temp->next->val);
							temp->next = temp->next->next;
							temp->next->prev=temp;
						}
						else{
							LOGDEBUG(FILE_LOGGER,"the deletion DOES NOT spans over the next node");
							 new_node = new node(initial_pos,
														-1);

							collect_ins+=temp->next->str.substr(remaining_deletion-1,LONG_MAX);
							initial_genome_pos = temp->next->val;
							initial_offset=remaining_deletion-1;
							remaining_deletion=0;
							num_kmers_moved=(temp->next->offset - initial_offset);
							//delete temp->next and put new node instead

							new_node->prev = temp;
							new_node->next = temp->next->next;
							temp->next->next->prev = new_node;
							temp->next = new_node;

							node *trav = new_node;
							long new_node_val = initial_pos;
							LOGDEBUG(FILE_LOGGER,"find out which initial pos to put the insertion (to be moved)");
							while(trav->prev->offset<0){
								long valid_pos = trav->prev->val+ (-1 * trav->prev->offset);
								if(valid_pos>=trav->val){
									trav=trav->prev;
									new_node_val = trav->val;
								}
								else{
									break;
								}
							}
							/*while (trav->prev->val == (--new_node_val) //find out which initial pos to put the insertion (to be moved).
							&& (trav->prev->offset < 0)) {
								trav = trav->prev;
							}*/
							//bottom_up_update(new_node,collect_ins.length());
							node *temp = new_node;
							while (temp != head) {
								if (temp->up) {
									temp->up->offset -= collect_ins.length();
									temp = temp->up;
								} else {
									temp = temp->prev;
								}
							}
							LOGDEBUG(FILE_LOGGER,"if the initial position to be inserted already has an insertion");
							//insert_and_update(--new_node_val,LONG_MAX,collect_ins);
							new_node_val--;
							if (new_node_val == trav->prev->val) {
								LOGDEBUG(FILE_LOGGER,"if the initial position to be inserted already has an insertion");
								insert_and_update(new_node_val,LONG_MAX,collect_ins);
								final_offset=trav->prev->offset;
							} else { //create a new node.
								LOGDEBUG(FILE_LOGGER,"create a new node");
								insert_and_update(new_node_val,0,collect_ins);
								insertions.push_back(new_node_val);
								final_offset=0;
							}
							final_genome_pos=new_node_val;
							node_to_promote = new_node;
							collect_ins.clear();
						}
					}

					if(remaining_deletion>0){
					//create and insert a new node
					LOGDEBUG(FILE_LOGGER,"There is still some remaining deletion");
					new_node = new node(initial_pos,
							remaining_deletion * -1);

					new_node->next = temp->next;
					new_node->prev = temp;
					temp->next = new_node;
					new_node->next->prev = new_node;
					remaining_deletion--;


					node *trav = new_node;
					long prev = new_node->val;
					long prev_offset =0;
					while (((trav->next->val - trav->val  + trav->offset)<0) && (remaining_deletion>0)) {
						LOGDEBUG(FILE_LOGGER,"the deletion starts from within an insertion and there is some deletion remaining which touches the next node");
						if (trav->next->offset < 0) { //if the next node is a deletion, we coalesce it, the upper level nodes would have been deleted before reaching this node.
							LOGDEBUG(FILE_LOGGER,"the deletion starts from within an insertion and coalescing a deletion");
							long num_bases = trav->next->val - prev ;
							long num_invalid_bases = prev_offset>=0?0:(prev_offset*(-1)-1);
							remaining_deletion-= (num_bases - num_invalid_bases -1);
							prev = trav->next->val;
							prev_offset = trav->next->offset;
							trav->offset += trav->next->offset;
							trav->next = trav->next->next;
							trav->next->prev = trav;
						} else {
							LOGDEBUG(FILE_LOGGER,"the deletion starts from within an insertion and trying to remove a node with insertion");
							long num_bases = trav->next->val - prev ;
							long num_invalid_bases = prev_offset>=0?0:(prev_offset*(-1)-1);
							remaining_deletion-= (num_bases - num_invalid_bases);

							long deletion_len_within_ins = min(
									remaining_deletion, trav->next->offset);
							remaining_deletion -= (deletion_len_within_ins);
							if (deletion_len_within_ins != trav->next->offset) //the entire insertion in not deleted
									{
								LOGDEBUG(FILE_LOGGER,"the deletion starts from within an insertion and the entire insertion in not deleted");
								initial_genome_pos=trav->next->val;
								initial_offset=deletion_len_within_ins;
								num_kmers_moved = trav->next->offset - deletion_len_within_ins;
								collect_ins += trav->next->str.substr(
										deletion_len_within_ins,
										trav->next->offset - deletion_len_within_ins);
							}
							trav->offset+=deletion_len_within_ins;
							prev=trav->next->val;
							prev_offset=trav->next->offset;
							deletions.push_back(trav->next->val);
							trav->next = trav->next->next;
							trav->next->prev = trav;
						}
					}
					}
/*					remaining_deletion -= (trav->next->val - trav->val);
					while (remaining_deletion > 0) {
						LOGDEBUG(FILE_LOGGER,"the deletion starts from a genome position and there is some deletion remaining");
						if (trav->next->offset < 0) { //if the next node is a deletion, we coalesce it, the upper level nodes would have been deleted before reaching this node.
							LOGDEBUG(FILE_LOGGER,"the deletion starts from a genome position and coalescing a deletion");
							trav->offset -= trav->next->offset;
							trav->next = trav->next->next;
							trav->next->prev = trav;
						} else {
							LOGDEBUG(FILE_LOGGER,"the deletion starts from a genome position and trying to remove an insertion");
							long len_copy = len;
							long deletion_len_within_ins = min(
									(len_copy - (trav->next->val - trav->val)
											- 1), trav->next->offset);
							remaining_deletion -= deletion_len_within_ins;
							if (deletion_len_within_ins != trav->next->offset) //the entire insertion in not deleted
									{
								LOGDEBUG(FILE_LOGGER,"the deletion starts from a genome position and entire insertion in not deleted");
								collect_ins += trav->next->str.substr(
										deletion_len_within_ins,
										temp->offset - deletion_len_within_ins);
							}

							trav->next = trav->next->next;
							trav->next->prev = trav;
						}
						remaining_deletion -= (trav->next->val - trav->val);
					}*/
					//now find/create the node where the coalesced insertions must be inserted.
					//the node should be initial position just before the deletion node, unless that position has a deletion
					//also, we must update the upper levels of the skip list since we may be transferring the insertion across a skip list boundary,whici may effect the upper levels
					if (!collect_ins.empty()) {
						LOGDEBUG(FILE_LOGGER,"the deletion starts from a genome position and trying to move a node which has been deleted incompletely");
						node *trav = new_node;

						long new_node_val = initial_pos;
						LOGDEBUG(FILE_LOGGER,"find out which initial pos to put the insertion (to be moved)");
						while(trav->prev->offset<0){
							long valid_pos = trav->prev->val+ (-1 * trav->prev->offset);
							if(valid_pos>=trav->val){
								trav=trav->prev;
								new_node_val = trav->val;
							}
							else{
								break;
							}
						}
						/*long new_node_val = initial_pos;
						LOGDEBUG(FILE_LOGGER,"find out which initial pos to put the insertion (to be moved)");

						while (trav->prev->val == (--new_node_val) //find out which initial pos to put the insertion (to be moved).
						&& (trav->prev->offset < 0)) {
							trav = trav->prev;
						}*/

						//bottom_up_update(new_node,collect_ins.length());
						node *temp = new_node;
						while (temp != head) {
							if (temp->up) {
								temp->up->offset -= collect_ins.length();
								temp = temp->up;
							} else {
								temp = temp->prev;
							}
						}

						//now we should insert at trav->prev
						new_node_val--;
						if (new_node_val == trav->prev->val) {
							LOGDEBUG(FILE_LOGGER,"if the initial position to be inserted already has an insertion");
							insert_and_update(new_node_val,LONG_MAX,collect_ins);
							final_offset=trav->prev->offset;
						} else { //create a new node.
							LOGDEBUG(FILE_LOGGER,"create a new node");
							insert_and_update(new_node_val,0,collect_ins);
							insertions.push_back(new_node_val);
							final_offset=0;
						}
						final_genome_pos=new_node_val;
						node_to_promote = new_node;

						//if the initial position to be inserted already has an insertion
						/*if (new_node_val == trav->prev->val) {
							LOGDEBUG(FILE_LOGGER,"if the initial position to be inserted already has an insertion");
							trav->prev->str += collect_ins;
							trav->prev->offset += collect_ins.length();
						} else { //create a new node. TODO: bubble this up?
							LOGDEBUG(FILE_LOGGER,"create a new node");
							node *new_node = new node(new_node_val,
									collect_ins.length());
							new_node->next = trav;
							new_node->prev = trav->prev;
							trav->prev->next = new_node;
							trav->prev = new_node;
						}*/
						//if the insertion to be moved crosses the boundary, update the one before list before the boundary
						/*if (boundary_node) {
							LOGDEBUG(FILE_LOGGER,"if the insertion to be moved crosses the boundary, update the one before list before the boundary");
							//bottom_up_update(boundary_node->prev,collect_ins.length());
							node *temp = boundary_node->prev;
							while (temp != head) {
								if (temp->up) {
									temp->up->offset += collect_ins.length();
									temp = temp->up;
								} else {
									temp = temp->prev;
								}
							}
						} else {
							LOGDEBUG(FILE_LOGGER,"if the insertion to be moved crosses the boundary, update the one after the boundary");
							//bottom_up_update(trav->prev,collect_ins.length());
							node *temp = trav->prev;
							while (temp != head) {
								if (temp->up) {
									temp->up->offset += collect_ins.length();
									temp = temp->up;
								} else {
									temp = temp->prev;
								}
							}
						}
						*/
						//traverse the skip list to update the upper levels if the insertion moves across a boundary

					}
					node_to_promote = new_node;
					if(new_node){
						for(long i=0;i<((new_node->offset)*-(1));i++){
							deletions.push_back(i+new_node->val);

						}
					}
					break;
				}

				/*
				 //pair<node*,unsigned long> p;
				 if (abs_val <= (temp->val + cumulative_count + temp->offset)) { //deletion begins from within an insetion
				 prev = temp->prev;
				 val = temp->val;
				 pos = abs_val - cumulative_count - temp->val;

				 } else {
				 prev = temp;
				 val = abs_val - cumulative_count - temp->offset;
				 pos = 0;
				 }
				 break;*/
			}
		}

	/*node *new_node = new node(val, offset);
	 //new_node->str = str;
	 new_node->next = prev->next;
	 new_node->prev = prev;
	 prev->next = new_node;
	 new_node->next->prev = new_node;
	 */
	//promote the node up.

	m_kmers=mod_kmers(initial_genome_pos,initial_offset,final_genome_pos,final_offset,num_kmers_moved,insertions,deletions);
	node_to_promote=NULL;
	node *new_node = node_to_promote;
	node *node_to_copy = new_node;
	if (node_to_copy) {
		LOGDEBUG(FILE_LOGGER,"promote the node up");
		val = node_to_copy->val;
		offset = node_to_copy->offset;
		while (coinToss()) {
			LOGDEBUG(FILE_LOGGER,"promoting the node up");
			node *temp = node_to_copy;
			//find the closest upper level node.
			long offset_sum = 0; //keeps track of offset in that level
			while (temp) {
				if (temp->up) {
					//temp=temp->up;
					offset_sum += temp->offset;
					offset_sum -= node_to_copy->offset;
					break;
				}
				if (temp->val == LONG_MIN) {
					offset_sum += temp->offset;
					offset_sum -= node_to_copy->offset;
					break;
				}
				offset_sum += temp->offset;
				temp = temp->prev;

			}
			node *new_node_upper = new node(val, offset);
			//if the upper level exists
			if (temp->up) {

				temp = temp->up;
				long total_offset = temp->offset;
				new_node_upper->next = temp->next;
				new_node_upper->prev = temp;
				new_node_upper->next->prev = new_node_upper;
				temp->next = new_node_upper;

				temp->offset = offset_sum;
				new_node_upper->offset = total_offset - offset_sum;

			} else //create new level
			{
				LOGDEBUG(FILE_LOGGER,"creating a new level");
				//create 1 new nodes - 2 sentinel nodes.
				//node *new_node_upper =  new node(val,offset);

				node *new_node_upper_head = new node(LONG_MIN, 0);
				node *new_node_upper_tail = new node(LONG_MAX, 0);

				new_node_upper_head->next = new_node_upper;
				new_node_upper_head->down = head;

				new_node_upper->prev = new_node_upper_head;
				new_node_upper->next = new_node_upper_tail;

				new_node_upper_tail->prev = new_node_upper;
				new_node_upper_tail->down = tail;

				head->up = new_node_upper_head;
				tail->up = new_node_upper_tail;

				head = new_node_upper_head;
				tail = new_node_upper_tail;

				//get_total_offset
				long forward_offset = 0;
				node *temp_node_to_copy = node_to_copy;
				while (temp_node_to_copy) {
					forward_offset += temp_node_to_copy->offset;
					temp_node_to_copy = temp_node_to_copy->next;
				}
				long backward_offset = 0;
				temp_node_to_copy = node_to_copy;

				while (temp_node_to_copy->prev) {
					temp_node_to_copy = temp_node_to_copy->prev;
					backward_offset += temp_node_to_copy->offset;
				}
				new_node_upper_head->offset = backward_offset;
				new_node_upper->offset = forward_offset;

			}
			new_node_upper->down = node_to_copy;
			node_to_copy->up = new_node_upper;
			node_to_copy = new_node_upper;
		}
	}
	return m_kmers;
}
void skip_list::insert_and_update_abs(const long abs_val, string str) {

	node *temp = head, *prev;
	long cumulative_count = 0, val = 0;
	unsigned long offset = str.length();
	unsigned long pos = 0;
	while (temp) {
		//cout<<"in here! cc = "<<cumulative_count<<" "<<abs_val<<" "<<temp->val<<endl;
		if (abs_val
				>= (temp->next->val + cumulative_count + temp->offset)&&temp->next->val!=LONG_MAX) {
			cumulative_count += temp->offset;
			temp = temp->next;
		}

		else	//current val is lesser than next val
		{
			if (temp->down) {
				temp->offset += offset;
				temp = temp->down;
			} else {
				//pair<node*,unsigned long> p;
				if (abs_val <= (temp->val + cumulative_count + temp->offset)) {
					prev = temp->prev;
					val = temp->val;
					pos = abs_val - cumulative_count - temp->val;
					//p.make_pair(temp->prev,val-cumulative_count);
					//return p;
				} else {
					prev = temp;
					val = abs_val - cumulative_count - temp->offset;
					pos = 0;
				}
				break;
			}
		}
	}

//find place to insert
//cout<<"outta here!"<<endl;
//cout<<" prev val = "<<prev->val<<" val: "<<val<<" pos: "<<pos<<endl;
	if (prev->next->val == val) //if the value already exists
			{
		//update the offset pointers..
		if ((prev->next->str).length() < pos) {
			LOGALERT(FILE_LOGGER, "Invalid value..pos = "+ to_string(pos)+ " skip list inconsistent!");
			return;
		}
		(prev->next->str).insert(pos, str);
		prev->next->offset += offset;
	} else {

		if (pos > 0) {
			LOGALERT(FILE_LOGGER, "Invalid value..pos = "+ to_string(pos)+ " skip list inconsistent!");
			return;
		}

		node *new_node = new node(val, offset);
		new_node->str = str;
		new_node->next = prev->next;
		new_node->prev = prev;
		prev->next = new_node;
		new_node->next->prev = new_node;

		//promote the node up.
		node *node_to_copy = new_node;
		while (coinToss()) {
			node *temp = node_to_copy;
			//find the closest upper level node.
			long offset_sum = 0; //keeps track of offset in that level
			while (temp) {
				if (temp->up) {
					//temp=temp->up;
					offset_sum += temp->offset;
					offset_sum -= node_to_copy->offset;
					break;
				}
				if (temp->val == LONG_MIN) {
					offset_sum += temp->offset;
					offset_sum -= node_to_copy->offset;
					break;
				}
				offset_sum += temp->offset;
				temp = temp->prev;

			}
			node *new_node_upper = new node(val, offset);
			//if the upper level exists
			if (temp->up) {

				temp = temp->up;
				long total_offset = temp->offset;
				new_node_upper->next = temp->next;
				new_node_upper->prev = temp;
				new_node_upper->next->prev = new_node_upper;
				temp->next = new_node_upper;

				temp->offset = offset_sum;
				new_node_upper->offset = total_offset - offset_sum;

			} else //create new level
			{
				//create 1 new nodes - 2 sentinel nodes.
				//node *new_node_upper =  new node(val,offset);

				node *new_node_upper_head = new node(LONG_MIN, 0);
				node *new_node_upper_tail = new node(LONG_MAX, 0);

				new_node_upper_head->next = new_node_upper;
				new_node_upper_head->down = head;

				new_node_upper->prev = new_node_upper_head;
				new_node_upper->next = new_node_upper_tail;

				new_node_upper_tail->prev = new_node_upper;
				new_node_upper_tail->down = tail;

				head->up = new_node_upper_head;
				tail->up = new_node_upper_tail;

				head = new_node_upper_head;
				tail = new_node_upper_tail;

				//get_total_offset
				long forward_offset = 0;
				node *temp_node_to_copy = node_to_copy;
				while (temp_node_to_copy) {
					forward_offset += temp_node_to_copy->offset;
					temp_node_to_copy = temp_node_to_copy->next;
				}
				long backward_offset = 0;
				temp_node_to_copy = node_to_copy;

				while (temp_node_to_copy->prev) {
					temp_node_to_copy = temp_node_to_copy->prev;
					backward_offset += temp_node_to_copy->offset;
				}
				new_node_upper_head->offset = backward_offset;
				new_node_upper->offset = forward_offset;

			}
			new_node_upper->down = node_to_copy;
			node_to_copy->up = new_node_upper;
			node_to_copy = new_node_upper;
		}
	}

}
long skip_list::get_cumulative_count(long val) //returns the absolute offset for a val in the genome.
		{
	long sum = 0;
	node *temp = head;

	while (temp) {
		if (temp->next->val <= val) // next val is >= next ; = since a node contains offset between that node and the next.
				{
			sum += temp->offset;
			temp = temp->next;
		}

		else	//current val is lesser than next val
		{
			if (temp->down) {
				//temp->offset+=offset;
				temp = temp->down;
			} else {
				if (temp->val == val)
					return sum;
				else
					return sum + temp->offset;
			}
		}
	}
	return -1;

}
node* skip_list::get_head() {
	return head;
}
void skip_list::print_node(node *temp) {
	cout << "(" << temp->val << "," << temp->offset << ")" << "\t";
	LOGDEBUG(FILE_LOGGER, "(" + std::to_string(temp->val) + "," + ")" + "\t");

}
void skip_list::print_base_level() {
	cout << endl << endl << "Base of the skip_list:" << endl;
//find base level head

	node *temp = head;
	while (temp->down)
		temp = temp->down;
//temp=temp->next; //ignoring the sentinel
	while (temp) {
		print_node(temp);
		cout << temp->str << "\t";
		temp = temp->next;
	}
	cout << endl;
}

void skip_list::get_stats(long &num_updates, long &num_levels) {
	num_updates = 0;
	num_levels = 0;
	cout << endl << endl << "Base of the skip_list:" << endl;

	node *temp = head;
	while (temp->down) {
		num_levels++;
		//temp=temp->down;
	}
//temp=temp->next; //ignoring the sentinel
	while (temp) {
		num_updates++;
		//print_node(temp);
		//cout<<temp->str<<"\t";
		temp = temp->next;
	}
	cout << endl;
}
void skip_list::print_list() {
	cout << endl << "Skip_list:" << endl;

	LOGDEBUG(FILE_LOGGER, "Skip_list:");
	node *temp = head;
	while (temp) {
		node *temp_row = temp;
		while (temp_row) {
			print_node(temp_row);
			temp_row = temp_row->next;
		}
		temp = temp->down;
		cout << endl;
		LOGDEBUG(FILE_LOGGER, "\n");
	}
}
/*
 int main()
 {

 test_skip_list();


 skip_list s;


 s.insert_and_update(2,0,"A");
 //s.print_list();
 s.insert_and_update(5,0,"A");
 //s.print_list();
 s.insert_and_update(5,1,"T");
 //s.print_list();
 s.insert_and_update(6,0,"A");
 //s.print_list();
 s.insert_and_update(7,0,"G");
 //s.print_list();
 s.insert_and_update(7,0,"T");
 //s.print_list();
 s.insert_and_update(9,0,"A");
 s.print_list();
 s.insert_and_update_abs(100,"T");
 s.insert_and_update_abs(10,"T");

 s.insert_and_update_abs(3,"T");
 s.insert_and_update_abs(4,"AC");
 s.insert_and_update_abs(6,"CC");
 //string str;
 //long offset;
 unsigned long pos;
 long val;
 node *n;
 s.get_prev_node(1000,val,pos,&n);
 if(n->next->val==val)//node exists
 {
 cout<<"Node exists! "<<"val "<<val<<" string "<<n->next->str<<" pos "<<pos<<" "<<n->next->val<<endl;
 }
 else
 {
 cout<<" New node must be created! "<<"val "<<val<<" string "<<n->next->str<<" pos "<<pos<<" "<<n->next->val<<endl;
 }
 //cout<<"Got string "<<n->str<<" pos "<<pos<<" "<<n->val<<endl;
 //s.insert_and_update_abs(12,"GG");
 cout<<s.get_cumulative_count(100)<<endl; // -1 means it does not exist
 cout<<s.get_cumulative_count(1)<<endl;
 cout<<s.get_cumulative_count(7)<<endl;
 cout<<s.get_cumulative_count(5)<<endl;

 node* temp1 = s.find(2);
 if(temp1)
 {
 s.print_node(temp1);
 }
 else
 cout<<"not found!"<<endl;

 temp1 = s.find(9);
 if(temp1)
 {
 s.print_node(temp1);
 }
 else
 cout<<"not found!"<<endl;
 //cout<<s.index;
 s.print_base_level();
 s.print_list();

 
 
 return 0;
 }*/
