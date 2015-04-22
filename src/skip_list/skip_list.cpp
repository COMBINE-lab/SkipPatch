#include <bits/stdc++.h>
#include "skip_list.h"
#include "skip_list_test.h"

using namespace std;

//put this in utils
int coinToss()
{
    return rand()%2;
}

skip_list::skip_list()
{
  node *bottom_head = new node(LONG_MIN,0);
  node *bottom_tail = new node(LONG_MAX,0);
  
  bottom_head->next = bottom_tail;
  bottom_tail->prev=bottom_head;
  
  head=bottom_head;
  tail = bottom_tail;
}

node* skip_list::find(long val)
{
  node *temp=head;
  
  while(temp)
  {
    if(temp->next->val<val)
    {
      temp=temp->next;
    }
    else if(temp->next->val==val)
    {
      if(temp->down)
	temp=temp->down;
      else
	return temp->next;
    }
    else
    {
      temp=temp->down;
    } 
  }
  return temp;
}

node* skip_list::find_prev(long val)
{
  node *temp=head;
  
  while(temp)
  {
    if(temp->next->val<val)
    {
      temp=temp->next;
    }
    
    else	//current val is equal or lesser than next val
    {
      if(temp->down)
	temp=temp->down;
      else
	return temp;
    } 
  }
  return temp;
}

node* skip_list::find_and_update_prev(long val,string str)
{
  long offset = str.length();
  node *temp=head;
  
  while(temp)
  {
    if(temp->next->val<=val) // next val is >= next ; = since a node contains offset between that node and the next.
    {
      temp=temp->next;
    }
    
    else	//current val is lesser than next val
    {
      if(temp->down)
      {
	temp->offset+=offset;
	temp=temp->down;
      }
      else
      {
	if(temp->val==val)
	  return temp->prev;
	return temp;
      }
    } 
  }
  return temp;
}
/*pair<node*,unsigned long> skip_list::find_and_update_prev_abs(long val,unsigned long offset)
{
  //long offset = str.length();
  node *temp=head;
  long cumulative_count=0
  while(temp)
  {
    
    if(val>=(temp->next->val+cumulative_count+temp->offset))
    {
      cumulative_count+=temp->offset;
      temp=temp->next;
    }
    
    else	//current val is lesser than next val
    {
      if(temp->down)
      {
	temp->offset+=offset;
	temp=temp->down;
      }
      else
      {
	pair<node*,unsigned long> p;
	if(val<=(temp->val+cumulative_count+temp->offset))
	{
	  p.make_pair(temp->prev,val-cumulative_count);
	  return p;
	}
	p.make_pair(temp->prev,val-cumulative_count);
	return p;
      }
    } 
  }
  pair<node*,unsigned long> p;
  p.make_pair(temp,val-cumulative_count);
  return p;
}
*/
void skip_list::insert(long val,long offset)
{
  //find place to insert
  node *prev = find_prev(val);
  
  if(prev->next->val==val) //if the value already exists
  {
    //update the offset pointers.. 
  }
  else
  {
    node *new_node = new node(val,offset);
    new_node->next = prev->next;
    new_node->prev=prev;
    prev->next=new_node;
    new_node->next->prev=new_node;
    
    //promote the node up.
    node *node_to_copy = new_node;
    while(coinToss())
    {
      node *temp = node_to_copy;
      //find the closest upper level node.
      while(temp)
      {
	if(temp->up)
	{
	  //temp=temp->up;
	  break;
	}
	if(temp->val==LONG_MIN)
	    break;
	temp=temp->prev;
      }
      node *new_node_upper =  new node(val,offset);
      //if the upper level exists
      if(temp->up)
      {
	temp = temp->up;
	
	new_node_upper->next = temp->next;
	new_node_upper->prev=temp;
	new_node_upper->next->prev = new_node_upper;
	temp->next=new_node_upper;
	
      }
      else //create new level
      {
	//create 1 new nodes - 2 sentinel nodes.
	//node *new_node_upper =  new node(val,offset);
	node *new_node_upper_head =  new node(LONG_MIN,0);
	node *new_node_upper_tail =  new node(LONG_MAX,0);
	
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
	
      }
      new_node_upper->down=node_to_copy;
      node_to_copy->up =  new_node_upper;
      node_to_copy = new_node_upper;
    }
  }
    
}
//delete this:

void skip_list::insert_and_update(long val,unsigned long pos,string str)
{
  //find place to insert
  node *prev = find_and_update_prev(val,str);
  long offset = str.length();
  if(prev->next->val==val) //if the value already exists
  {
    //update the offset pointers.. 
    if((prev->next->str).length()<pos)
    {
      cout<<"Invalid value.. skip list inconsistent!"<<endl;
      return;
    }
    (prev->next->str).insert(pos,str);
    prev->next->offset+=offset;
  }
  else
  {
    
    if(pos>0)
    {
      cout<<"Invalid value.. skip list inconsistent!"<<endl;
      return;
    }
    
    
    node *new_node = new node(val,offset);
    new_node->str = str;
    new_node->next = prev->next;
    new_node->prev=prev;
    prev->next=new_node;
    new_node->next->prev=new_node;
    
    //promote the node up.
    node *node_to_copy = new_node;
    while(coinToss())
    {
      node *temp = node_to_copy;
      //find the closest upper level node.
      long offset_sum = 0; //keeps track of offset in that level
      while(temp)
      {
	if(temp->up)
	{
	  //temp=temp->up;
	  offset_sum+=temp->offset;
	  offset_sum-=node_to_copy->offset;
	  break;
	}
	if(temp->val==LONG_MIN)
	{
	    offset_sum+=temp->offset;
	    offset_sum-=node_to_copy->offset;
	    break;
	}
	offset_sum+=temp->offset;
	temp=temp->prev;
	
      }
      node *new_node_upper =  new node(val,offset);
      //if the upper level exists
      if(temp->up)
      {
	
	temp = temp->up;
	long total_offset = temp->offset;
	new_node_upper->next = temp->next;
	new_node_upper->prev=temp;
	new_node_upper->next->prev = new_node_upper;
	temp->next=new_node_upper;
	
	temp->offset=offset_sum;
	new_node_upper->offset = total_offset - offset_sum;
	
      }
      else //create new level
      {
	//create 1 new nodes - 2 sentinel nodes.
	//node *new_node_upper =  new node(val,offset);
	
	node *new_node_upper_head =  new node(LONG_MIN,0);
	node *new_node_upper_tail =  new node(LONG_MAX,0);
	
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
	while(temp_node_to_copy)
	{
	  forward_offset+=temp_node_to_copy->offset;
	  temp_node_to_copy = temp_node_to_copy->next;
	}
	long backward_offset = 0;
	temp_node_to_copy = node_to_copy;
	
	while(temp_node_to_copy->prev)
	{
	  temp_node_to_copy = temp_node_to_copy->prev;
	  backward_offset+=temp_node_to_copy->offset;
	}
	new_node_upper_head->offset=backward_offset;
	new_node_upper->offset = forward_offset;
	
      }
      new_node_upper->down=node_to_copy;
      node_to_copy->up =  new_node_upper;
      node_to_copy = new_node_upper;
    }
  }
    
}
void skip_list::insert_and_update_abs(const long abs_val,string str)
{
  
  node *temp=head,*prev;
  long cumulative_count=0,val=0;
  unsigned long offset = str.length();
  unsigned long pos = 0;
  while(temp)
  {
  //cout<<"in here! cc = "<<cumulative_count<<" "<<abs_val<<" "<<temp->val<<endl;  
    if(abs_val>=(temp->next->val+cumulative_count+temp->offset) &&temp->next->val!=LONG_MAX)
    {
      cumulative_count+=temp->offset;
      temp=temp->next;
    }
    
    else	//current val is lesser than next val
    {
      if(temp->down)
      {
	temp->offset+=offset;
	temp=temp->down;
      }
      else
      {
	//pair<node*,unsigned long> p;
	if(abs_val<=(temp->val+cumulative_count+temp->offset))
	{
	  prev = temp->prev;
	  val = temp->val;
	  pos = abs_val-cumulative_count-temp->val;
	  //p.make_pair(temp->prev,val-cumulative_count);
	  //return p;
	}
	else
	{
	  prev = temp;
	  val = abs_val- cumulative_count - temp->offset;
	  pos = 0;
	}
	break;
      }
    } 
  }
  
  
  
  //find place to insert
  //cout<<"outta here!"<<endl;
  //cout<<" prev val = "<<prev->val<<" val: "<<val<<" pos: "<<pos<<endl;
  if(prev->next->val==val) //if the value already exists
  {
    //update the offset pointers.. 
    if((prev->next->str).length()<pos)
    {
      cout<<"Invalid value..pos = "<<pos<<" skip list inconsistent!"<<endl;
      return;
    }
    (prev->next->str).insert(pos,str);
    prev->next->offset+=offset;
  }
  else
  {
    
    if(pos>0)
    {
      cout<<"Invalid value.. pos = "<<pos<<" skip list inconsistent!"<<endl;
      return;
    }
    
    
    node *new_node = new node(val,offset);
    new_node->str = str;
    new_node->next = prev->next;
    new_node->prev=prev;
    prev->next=new_node;
    new_node->next->prev=new_node;
    
    //promote the node up.
    node *node_to_copy = new_node;
    while(coinToss())
    {
      node *temp = node_to_copy;
      //find the closest upper level node.
      long offset_sum = 0; //keeps track of offset in that level
      while(temp)
      {
	if(temp->up)
	{
	  //temp=temp->up;
	  offset_sum+=temp->offset;
	  offset_sum-=node_to_copy->offset;
	  break;
	}
	if(temp->val==LONG_MIN)
	{
	    offset_sum+=temp->offset;
	    offset_sum-=node_to_copy->offset;
	    break;
	}
	offset_sum+=temp->offset;
	temp=temp->prev;
	
      }
      node *new_node_upper =  new node(val,offset);
      //if the upper level exists
      if(temp->up)
      {
	
	temp = temp->up;
	long total_offset = temp->offset;
	new_node_upper->next = temp->next;
	new_node_upper->prev=temp;
	new_node_upper->next->prev = new_node_upper;
	temp->next=new_node_upper;
	
	temp->offset=offset_sum;
	new_node_upper->offset = total_offset - offset_sum;
	
      }
      else //create new level
      {
	//create 1 new nodes - 2 sentinel nodes.
	//node *new_node_upper =  new node(val,offset);
	
	node *new_node_upper_head =  new node(LONG_MIN,0);
	node *new_node_upper_tail =  new node(LONG_MAX,0);
	
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
	while(temp_node_to_copy)
	{
	  forward_offset+=temp_node_to_copy->offset;
	  temp_node_to_copy = temp_node_to_copy->next;
	}
	long backward_offset = 0;
	temp_node_to_copy = node_to_copy;
	
	while(temp_node_to_copy->prev)
	{
	  temp_node_to_copy = temp_node_to_copy->prev;
	  backward_offset+=temp_node_to_copy->offset;
	}
	new_node_upper_head->offset=backward_offset;
	new_node_upper->offset = forward_offset;
	
      }
      new_node_upper->down=node_to_copy;
      node_to_copy->up =  new_node_upper;
      node_to_copy = new_node_upper;
    }
  }
    
}

long skip_list::get_cumulative_count(long val)
{
  long sum=0;
  node *temp=head;
  
  while(temp)
  {
    if(temp->next->val<=val) // next val is >= next ; = since a node contains offset between that node and the next.
    {
      sum+=temp->offset;
      temp=temp->next;
    }
    
    else	//current val is lesser than next val
    {
      if(temp->down)
      {
	//temp->offset+=offset;
	temp=temp->down;
      }
      else
      {
	if(temp->val==val)
	  return sum;
	else
	  return -1;
      }
    } 
  }
  return -1;
  
}
node* skip_list::get_head()
{
  return head;
}
void skip_list::print_node(node *temp)
{
  cout<<"("<<temp->val<<","<<temp->offset<<")"<<"\t";
}
void skip_list::print_base_level()
{
  cout<<endl<<endl<<"Base of the skip_list:"<<endl;
  //find base level head
  
  node *temp=head;
  while(temp->down)
    temp=temp->down;
  //temp=temp->next; //ignoring the sentinel
  while(temp)
  {
    print_node(temp);
    cout<<temp->str<<"\t";
    temp=temp->next;
  }
  cout<<endl;
}

void skip_list:: print_list()
{
  cout<<endl<<"Skip_list:"<<endl;
  node *temp = head;
  while(temp)
  {
    node *temp_row = temp;
    while(temp_row)
    {
      print_node(temp_row);
      temp_row=temp_row->next;
    }
    temp=temp->down;
    cout<<endl;
  }
}

int main()
{

  test_skip_list();


  //skip_list s;
  
  /*
  s.insert(2,1);
  s.insert(1,1);
  s.insert(1,1);
  s.insert(5,1);
  s.insert(5,1);
  s.insert(6,1);
  s.insert(7,1);
  s.insert(9,1);
  s.insert(0,1);
  */
/*
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

  */
 
  return 0;
}