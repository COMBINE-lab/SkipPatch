#include <bits/stdc++.h>

using namespace std;
#define K 3 //user define parameter

class genome
{
  string reference;
  unordered_map<string,list<int>> m;
public:
  
  void get_input();
  void construct_hash();
  void display_hash();
  void display_genome();
  float get_load_factor();
  void display_load();
};

class utility
{
public:
  void ignore_first_line();
};

void genome::display_genome()
{
  cout<<reference<<endl;
}
void utility::ignore_first_line()
{
  string input;
  getline(cin,input);
}

utility u;

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
int main()
{
  genome g;
  
  g.get_input();
  g.display_genome();
  g.construct_hash();
  g.display_load();
  cout << "load_factor = " << g.get_load_factor() << std::endl;
  g.display_hash();
  return 0;
}