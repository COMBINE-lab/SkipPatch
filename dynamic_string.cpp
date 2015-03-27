#include <bits/stdc++.h>

using namespace std;
#define K 3 //user define parameter

string alphabet="AGCT"; //#used for generating random string


class utility
{
public:
  void ignore_first_line();
  bool is_valid_input(int,int,int); //#last arg should be uint; cant compare
  string generate_random_string(int);
  int get_snp_begin(int);
  int get_snp_end(int,int);
  //string generate_modified_substr(int,string);
};
utility u;
/*
string utility::generate_modified_substr(int snp_begin,string new_string)
{
  
}
*/
int utility::get_snp_begin(int pos)
{
  return (pos-K+1)<0?0:(pos-K+1);
}
int utility::get_snp_end(int pos,int len)
{
  return (pos+len+K-1)>len?len:(pos+len+K-1);
}
string utility::generate_random_string(int len)
{
  string random;
  for(int i=0;i<len;i++)
  {
    random+=alphabet[i%(alphabet.size())]; //*not really random :)
  }
  return random;
}
bool utility::is_valid_input(int pos,int len,int max_len)
{
  if(pos<0)
  {
    cout<<"Cant insert a neg pos"<<endl;
    return false;
  }
  if(len<=0)
  {
    cout<<"Min len =1"<<endl;
    return false;
  }
  if((pos+len-1)>=max_len) 
  {
    cout<<"insertion out of bounds!"<<endl;
    return false;
  }
  return true;
}
void utility::ignore_first_line()
{
  string input;
  getline(cin,input);
}


class genome
{
  string reference;
  unordered_map<string,list<int>> m;
public:
  int get_length();
  void get_input();
  void construct_hash();
  void display_hash();
  void display_genome();
  float get_load_factor();
  void display_load();
  bool snp_at(int,int,uint);
  void remove_kmer_from_hash_at(int,string);
  void add_kmer_from_hash_at(int,string);
  std::list<int> find(std::string); 
};

void genome::display_genome()
{
  cout<<reference<<endl;
}

int genome::get_length()
{
  return reference.length();
}

void genome::get_input()
{
  u.ignore_first_line();
  string input;
  while(getline(cin,input)){
    reference+=input;
    input.clear();
  }
}

void genome::construct_hash()
{
  for(auto it=reference.begin();it!=reference.end()-K+1;it++)
  {
    string temp(it,it+K);
    m[temp].push_back(it-reference.begin());
  }
}

/*
void genome::construct_hash()
{
  string input;
  int line_num=0;
  getline(cin,input); //first line is header
  line_num++;
  input.clear();
  string prev,temp; //#clearing the string instead.
  
  while(getline(cin,input))
  {
    reference+=input;
    temp=prev+input;
    int len = temp.length();
    int i;
    for(i=0;i<(len-K+1);i++) //#use iterators
    {
      string window(temp.begin()+i,temp.begin()+i+K); 
      m[window].push_back(i+line_num*(input.length()));
    }
    
    prev.clear();
    prev=temp.substr(i,K-1); //hash of last K-1 characters not constructed yet
    input.clear();	
    line_num++;
  }
  
}
*/

void genome::remove_kmer_from_hash_at(int i,string curr_kmer)
{
  m[curr_kmer].remove(i); //#need to define own erase function
  if(m[curr_kmer].size()==0)
    m.erase(curr_kmer);
  
}
void genome::add_kmer_from_hash_at(int i,string new_kmer)
{
  m[new_kmer].push_back(i);
}
void genome::display_hash()
{
  for(auto it=m.begin();it!=m.end();it++)
  {
    cout<<it->first<<"\t";
    for(auto list_it=(it->second).begin();list_it!=(it->second).end();list_it++)
    {
      cout<<*list_it<<" ";
    }
    cout<<endl;
  }
}

float genome::get_load_factor()
{
  return m.load_factor();
}

void genome::display_load()
{
  std::cout << "size = " << m.size() << std::endl;
  std::cout << "bucket_count = " << m.bucket_count() << std::endl;
  std::cout << "max_load_factor = " << m.max_load_factor() << std::endl;
  
}

bool genome::snp_at(int pos,int len,uint max_len)
{
  //utility u;
  if(!u.is_valid_input(pos,len,reference.length())) //checks positions to be modified are valid
    return false; 
  string new_string = u.generate_random_string(len);
  
  //handling edge cases:
  int snp_begin = u.get_snp_begin(pos); 
  int snp_end = u.get_snp_end(pos,reference.length());
  
  //cout<<snp_begin<<endl;
  
  //string modified_reference_substr=u.generate_modified_substr(snp_begin,pos,new_string);
  string modified_reference_substr(reference.begin()+snp_begin,reference.begin()+pos);
  modified_reference_substr+=new_string;
  string temp(reference.begin()+pos+len,reference.begin()+snp_end);
  modified_reference_substr+=temp;
  for(int i=snp_begin;i<(snp_end-(K-1));i++)
  {
    string curr_kmer(reference.begin()+i,reference.begin()+i+K);
    string new_kmer(modified_reference_substr.begin()+(i-snp_begin),modified_reference_substr.begin()+(i-snp_begin+K));
    remove_kmer_from_hash_at(i,curr_kmer);
    add_kmer_from_hash_at(i,new_kmer);
  }
  //updating the reference itself
  for(int i=pos;i<pos+len;i++)
  {
    reference[i] = new_string[i-pos];
  }
  cout<<"Generated string: "<<new_string<<"\t"<<"inserted at: "<<pos<<endl;
  return true;
}

/*
This function searches the hash table to find the read
If the length of the string is greater than k, 
k-mers of the read are generated and 
*/
std::list<int> genome::find(string read){

	std::list<int> pos_prev;
	std::list<int> pos_curr;

	//Look for the first K-length substring of the read
	std::string read_kmer = read.substr(0,K); 
	auto search = m.find(read_kmer);
    if(search != m.end()) {
		pos_prev = search->second;
	} else {
		std::cout<< read << ": Not Found!"<<std::endl;
		return pos_prev;
	}
	int pos = 1;
	//For every k-mer after that, check if it extends from the previous k-mer
	for(auto iter=read.begin()+1; iter!=read.end()-K+1; iter++){
    	read_kmer = read.substr(pos,pos+K-1);
    	search = m.find(read_kmer); 
	    if(search != m.end()) { //If the k-mer is found, check if it can be extended from any of the previous k-mers		
			pos_curr = search->second;

			auto iter_p = pos_curr.begin();
			while(iter_p!=pos_curr.end()){
				auto curr = iter_p;
				++iter_p;
				//If you cannot extend from one of the earlier k-mers, remove that position
				if(std::find(pos_prev.begin(), pos_prev.end(),*curr-1)==pos_prev.end()){
					pos_curr.erase(curr);
				}
			}
		} 		    
		pos_prev = pos_curr;
		pos_curr.clear();
		pos++;
  	}

 	int buf = read.length()-K;
  	for(auto iter=pos_prev.begin(); iter!=pos_prev.end(); iter++){
  		*iter -= buf;
  		std::cout<<"Found at: " << *iter << std::endl;
  	}

  	return pos_prev;
}



int main()
{
  genome g;
  
  g.get_input();
  g.construct_hash();
  
  g.display_genome();
  g.display_hash();

  cout<<"Finding ATT... \n";
  g.find("ATT");

  cout<<"Finding TAG... \n";
  g.find("TAG");
  
  cout<<"Finding TAGC... \n";
  g.find("TAGC");

  cout<<"Finding TAGCT... \n";
  g.find("TAGCT");

  if(!g.snp_at(1,4,g.get_length()))
    cout<<"insert failed"<<endl;
  
  g.display_genome();
  g.display_hash();
  
  if(!g.snp_at(5,1,g.get_length()))
    cout<<"insert failed"<<endl;
  
  g.display_genome();
  g.display_hash();
  
  if(!g.snp_at(5,3,g.get_length()))
    cout<<"insert failed"<<endl;
  
  g.display_genome();
  g.display_hash();
  
  if(!g.snp_at(2,5,g.get_length()))
    cout<<"insert failed"<<endl;
  
  g.display_genome();
  g.display_hash();



  /*if(!g.snp_at(1,7,g.get_length()))
    cout<<"insert failed"<<endl;
  if(!g.snp_at(1,0,g.get_length()))
    cout<<"insert failed"<<endl;
  if(!g.snp_at(-1,1,g.get_length()))
    cout<<"insert failed"<<endl;
  */
  /*g.construct_hash();
  g.display_load();
  cout << "load_factor = " << g.get_load_factor() << std::endl;
  g.display_hash();*/
  return 0;
}