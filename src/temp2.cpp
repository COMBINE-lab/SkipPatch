#include <bits/stdc++.h>

#define QF 4641
#define QC 1000
#define N 10
using namespace std;

void get_input(const string path_to_query_file,vector<tuple<char, long, long>> &edit,vector<tuple<char,string,char,long>> &query)
{
  string g;
  ifstream myfile (path_to_query_file);
  if (myfile.is_open())
  {
    for(int j=0;j<N;j++)
    {
      for (int i=0;i<QF;i++)
      {
	
	getline(myfile, g, ' ');
	char c = g[0];
	g.clear();
	
	getline( myfile,g, ' ');
	long l = stol(g,nullptr,10);
	g.clear();
	
	getline( myfile,g, '\n');
	long l_del = stol(g,nullptr,10);
	edit.push_back(make_tuple(c,l,l_del));
	g.clear();
	//cout<<g<<endl;
      }
      
      for (int i=0;i<QC;i++)
      {
	
	getline(myfile, g, ' ');
	char c = g[0];
	g.clear();
	
	string q;
	getline( myfile,q, ' ');
	
	
	getline(myfile, g, ' ');
	char c1 = g[0];
	g.clear();
	
	getline( myfile,g, '\n');
	long l_del=0;
	l_del = stol(g,nullptr,10);
	query.push_back(make_tuple(c,q,c1,l_del));
	//cout<<g<<endl;
      }
    }
  }
  else
  {
    cout<<"Cant open file :( "<<endl;
  }
}
void benchmark_search(const string path_to_query_file,const string output_path){
  std::cout<<"SEARCH BENCHMARKING START"<<std::endl;
  vector<tuple<char, long, long>> edit;
  vector<tuple<char,string,char,long>> query;
  get_input(path_to_query_file,edit,query);
  for(int j=0;j<N;j++)
  {
    for(int i=0;i<QF;i++)
    {
      cout<<get<0>(edit[i])<<" "<<get<1>(edit[i])<<" "<<get<2>(edit[i]);
      cout<<endl;
    }
    
    for(int i=0;i<QC;i++)
    {
      cout<<get<0>(query[i])<<" "<<get<1>(query[i])<<" "<<get<2>(query[i])<<" "<<get<3>(query[i]);
      cout<<endl;
    }
  }
}

int main()
{
  benchmark_search("/home/komal/dynamic-updateable-index/scripts/ecoli_q","./output.txt");
}