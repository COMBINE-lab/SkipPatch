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
