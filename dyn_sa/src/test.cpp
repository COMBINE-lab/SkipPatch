#include <iostream>
#include <cstdlib>

#include <fstream>

#include "dynSA.h"
#include "utils.h"
#include <string.h>
#include "types.h"
#include <sstream>
using namespace std;
using namespace dynsa;

extern "C" {
#include "time.h"
#include "sys/time.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
}

#include <vector>
#include <tuple>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>

#define QF 4641
#define QC 5000
#define N 10


uchar *get_reference_sequence(char *filename, size_t &n, int termin) {
  std::ifstream file(filename);
  if (!file)
    {
      cerr << "error reading file: " << filename << endl;
      exit(EXIT_FAILURE);
    }

  file.seekg(0, ios::end);
  n = file.tellg();
  if (termin)
    n++;
  file.seekg(0, ios::beg);

  uchar *text = new uchar[n+1];

  char c;
  size_t i=0;

  while (file.get(c))
    {
      text[i]=c;//(c=='\n'||c=='\r')?'X':c;
      i++;
    }
  file.close();

  if (termin)
    text[n-1]=0;
  text[n]=0;
  return text;
}

void usage(char *program) {
    cerr << "Usage (NOW): " << program << " <filename_input_reference> <input_test_data> <number_of_edits>" << endl;
    cerr << "Usage (EARLIER): " << program << " <filename> [lcp] [getSA <num_queries>] [insert (<file to ins.> <pos of ins.>)+] [delete (<pos. to delete> <length of deletion>)+] " << endl;
    exit(1);
}

//Print time elapsed in seconds
void print_time_elapsed(std::string desc, struct timeval* start, struct timeval* end) {
    
    struct timeval elapsed;
    
    if(start->tv_usec > end->tv_usec) {
        end->tv_usec += 1000000;
        end->tv_sec--;
    }
    elapsed.tv_usec = end->tv_usec - start->tv_usec;
    elapsed.tv_sec  = end->tv_sec  - start->tv_sec;
    float time_elapsed = (elapsed.tv_sec*1000000 + elapsed.tv_usec)/1000000.f;
    std::cout << desc << " Total Time Elapsed = " << time_elapsed << " seconds" <<std::endl;

    return;
}

void get_test_input(std::string filename, vector<tuple<char,long,string>> &vec)
{
  string g;
  ifstream myfile (filename);
  if (myfile.is_open())
  {
    while (getline(myfile, g, ' '))
    {
      char c = g[0];
      g.clear();
      getline( myfile,g, ' ');
      long l = stol(g,nullptr,10);
      g.clear();
      getline( myfile,g, '\n');
      vec.push_back(make_tuple(c,l,g));
    }      
  }
  else
  {
    cout<<"Cant open file :( "<<endl;
  }
  return;
}


void get_input(const string path_to_query_file,vector<tuple<char, long, long,string>> &edit,vector<tuple<char,string,char,long>> &query)
{
  ifstream myfile (path_to_query_file);
  if (myfile.is_open())
  {
    for(int j=0;j<N;j++)
    {
//	cout<<j<<endl;
  string g;
      for (int i=0;i<QF;i++)
      {
//	cout<<i<<endl;	
	getline(myfile, g, ' ');
	char c = g[0];
//	cout<<c<<"\t";
	g.clear();
	
	getline( myfile,g, ' ');
	long l = stol(g,nullptr,10);
//	cout<<l<<"\t";
	g.clear();
	
	getline( myfile,g, '\n');
	long l_del=0;
	if(c=='D')
	{
		l_del = stol(g,nullptr,10);
	}
//	cout<<g<<endl;
	edit.push_back(make_tuple(c,l,l_del,g));
	g.clear();
	//cout<<g<<endl;
      }
      
	//cout<<j<<endl;
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



void benchmark_edits(int argc, char *argv[]){

	size_t n;
	uchar *text;
  	DynSA *wt;
  	float *f;
	size_t total_length_ins = 0, total_length_del = 0;
    	struct timeval start, end;
    	struct timezone tzp;

  	if (argc < 3) {
    		usage(argv[0]);
  	}

  	text = get_reference_sequence(argv[1],n,1);

	// Creates the structure for dynamic suffix array
	wt = new DynSA(false);
	f = DynRankS::createCharDistribution(text, (n > 10000000) ? n+1 : 10000000, 1);
	wt->initEmptyDynSA(f);
	gettimeofday(&start, &tzp);
	wt->addText(text,n);
	gettimeofday(&end, &tzp);
	print_time_elapsed("DynSA: Building Index: ", &start, &end);

	int test_count=0;

	//Read data
  	vector<tuple<char, long, string>> input_data;
  	get_test_input(argv[2],input_data);

	gettimeofday(&start, &tzp);

	for(auto it: input_data){

		if(test_count>=stoi(argv[3],nullptr,10))
			break;

  		if(get<0>(it)=='I') {
			//cout<<"Inserting "<<get<2>(it)<<" at "<<get<1>(it) << endl;
			uchar *ins = new uchar[ (get<2>(it)).length()+1 ];
			strcpy( (char*)ins, (get<2>(it)).c_str() );
			wt->addChars(ins, get<2>(it).length(), get<1>(it)+2); //wt->addChars(patterns[i], length_patterns[i], ins_indexes[i]+1);
   			total_length_ins += get<2>(it).length();   	
   			test_count++;
		}
		if(get<0>(it)=='D') {
			//cout<<"Deleting from "<< get<1>(it)+1 <<" to "<< stoi(get<2>(it))+1 << endl;
			wt->deleteChars(stoi(get<2>(it),nullptr,10)-get<1>(it)+1, get<1>(it)+1); //wt->deleteChars(length_del[i], del_indexes[i]+1);
    		total_length_del += (stoi(get<2>(it),nullptr,10)-get<1>(it));
    		test_count++;
		}
		if(get<0>(it)=='S') {
			//cout<<"SNP "<<get<2>(it)<<" at "<<get<1>(it) << endl;
			uchar *ins = new uchar[ (get<2>(it)).length()+1 ];
			strcpy( (char*)ins, (get<2>(it)).c_str() );
			
			//cout<<"Deleting from "<< get<1>(it) <<" to "<< (get<2>(it)).length()+get<1>(it)-1 << endl;
			wt->deleteChars((get<2>(it)).length(), get<1>(it)+1);
			//cout<<"Inserting "<<get<2>(it)<<" at "<<get<1>(it) << endl;
			wt->addChars(ins, get<2>(it).length(), get<1>(it)+1);
			test_count++;
		}
  	}
	gettimeofday(&end, &tzp);
	std::string message = std::string("DynSA: Updates: ");
	print_time_elapsed(message, &start, &end);

	/*
	//Regenerating the updated reference sequence from the BWT F&L
	n = wt->getSize();
	size_t i = 1, length = 0;
  	char c = (*wt)[i];
  	uchar *newtext = new uchar[n];
  	newtext[n-1]='\0';
  	//Retrieve the text
  	while (c!=0) {
    	c = (*wt)[i];
    	newtext[n-length-2]=c;
    	i = wt->LFmapping(i);
    	length++;
    	if (length > n)
      		break;
  	}
	*/

	//Write updated genome to file 
  	//std::string output_file (argv[3] + std::string("_") + std::string(argv[4]) + std::string(".dynsa"));
	//std::ofstream outfile (output_file);
	//outfile << newtext;
	//outfile.close();

	//std::cout << text << endl;
	//std::cout << newtext << endl;

	/*
  	if (n!=length) { // Clearly, if length is different from the expected length, we have a big problem!
    	cerr << "Houston, we have a problem...   " << " n = " << n << ", length = " << length << endl;
    	exit(2);
  	} else {
  		//Verify here if the generated string is correct 
	}
	*/

	delete [] text;
  	delete [] f;
  	cout << "Done..." << endl;

}


void benchmark_search(int argc, char *argv[]){

	size_t n;
	uchar *text;
  	DynSA *wt;
  	float *f;
	size_t total_length_ins = 0, total_length_del = 0;
    	struct timeval start, end;
   	struct timezone tzp;

  	if (argc < 2) {
    		usage(argv[0]);
  	}

  	text = get_reference_sequence(argv[1],n,1);

	// Creates the structure for dynamic suffix array
	wt = new DynSA(false);
	f = DynRankS::createCharDistribution(text, (n > 10000000) ? n+1 : 10000000, 1);
	wt->initEmptyDynSA(f);
	gettimeofday(&start, &tzp);
	wt->addText(text,n);
	gettimeofday(&end, &tzp);
	print_time_elapsed("DynSA: Building Index: ", &start, &end);

	//Read data
  	vector<tuple<char, long, long, string>> edits;
  	vector<tuple<char, string, char, long>> queries;
  	get_input(argv[2],edits,queries);
	
	auto it = edits.begin();
	auto q = queries.begin();

	int queryfreq = QF;
	int querycount = QC;
	int iterations = N;

	for(int i=0; i<iterations; i++){
		
		for(int j=0; j<queryfreq; j++){

			if(get<0>(*it)=='I') {
				//cout<<"Inserting "<<get<3>(*it)<<" at "<<get<1>(*it) << endl;
				uchar *ins = new uchar[ (get<3>(*it)).length()+1 ];
				strcpy( (char*)ins, (get<3>(*it)).c_str() );
				wt->addChars(ins, get<3>(*it).length(), get<1>(*it)+2); //wt->addChars(patterns[i], length_patterns[i], ins_indexes[i]+1);
	   			total_length_ins += get<3>(*it).length();
	   			it++;
			}
			else if(get<0>(*it)=='D') {
				//cout<<"Deleting from "<< get<1>(*it) <<" to "<< get<2>(*it) << endl;
				wt->deleteChars(get<2>(*it)-get<1>(*it)+1, get<1>(*it)); //wt->deleteChars(length_del[i], del_indexes[i]+1);
	    			total_length_del += (get<2>(*it)-get<1>(*it));
	    			it++;
			}
		} 
		
		gettimeofday(&start, &tzp);
		for(int j=0; j<querycount; j++){
			uchar *query = new uchar[ (get<1>(*q)).length()+1 ];
			strcpy((char*)query, (get<1>(*q)).c_str());
			//std::cerr << i*querycount+j << "   Locating: " << query << endl;	
			wt->locate(query);
			q++;
		}
		gettimeofday(&end, &tzp);
		std::string message = std::string("DynSA: Search: ");
		print_time_elapsed(message, &start, &end);
		
	}

	//Regenerating the updated reference sequence from the BWT F&L
	/*
	n = wt->getSize();
	size_t i = 1, length = 0;
  	char c = (*wt)[i];
  	uchar *newtext = new uchar[n];
  	newtext[n-1]='\0';
  	//Retrieve the text
  	while (c!=0) {
    	c = (*wt)[i];
    	newtext[n-length-2]=c;
    	i = wt->LFmapping(i);
    	length++;
    	if (length > n)
      		break;
  	}

  	std::string output_file (argv[3] + std::string(".") + std::string(argv[4]) + std::string(".dynsa"));
	std::ofstream outfile (output_file);
	outfile << newtext;
	outfile.close();

  	if (n!=length) { // Clearly, if length is different from the expected length, we have a big problem!
    	cerr << "Houston, we have a problem...   " << " n = " << n << ", length = " << length << endl;
    	exit(2);
  	} else {
  		//Verify here if the generated string is correct 
	}
	*/

	delete [] text;
  	delete [] f;
  	cout << "Done..." << endl;

}

void benchmark_substr(int argc, char *argv[]){

	size_t n;
	uchar *text;
  	DynSA *wt;
  	float *f;
	size_t total_length_ins = 0, total_length_del = 0;
    	struct timeval start, end;
    	struct timezone tzp;

  	if (argc < 3) {
    		usage(argv[0]);
  	}

  	text = get_reference_sequence(argv[1],n,1);

	// Creates the structure for dynamic suffix array
	wt = new DynSA(false);
	f = DynRankS::createCharDistribution(text, (n > 10000000) ? n+1 : 10000000, 1);
	wt->initEmptyDynSA(f);
	gettimeofday(&start, &tzp);
	wt->addText(text,n);
	gettimeofday(&end, &tzp);
	print_time_elapsed("DynSA: Building Index: ", &start, &end);

	int test_count=0;

	//Read data
  	vector<tuple<char, long, string>> input_data;
  	get_test_input(argv[2],input_data);

	gettimeofday(&start, &tzp);

	for(auto it: input_data){

		if(test_count>=stoi(argv[3],nullptr,10))
			break;

  		if(get<0>(it)=='I') {
			//cout<<"Inserting "<<get<2>(it)<<" at "<<get<1>(it) << endl;
			uchar *ins = new uchar[ (get<2>(it)).length()+1 ];
			strcpy( (char*)ins, (get<2>(it)).c_str() );
			wt->addChars(ins, get<2>(it).length(), get<1>(it)+2); //wt->addChars(patterns[i], length_patterns[i], ins_indexes[i]+1);
   			total_length_ins += get<2>(it).length();   	
   			test_count++;
		}
		if(get<0>(it)=='D') {
			//cout<<"Deleting from "<< get<1>(it)+1 <<" to "<< stoi(get<2>(it))+1 << endl;
			wt->deleteChars(stoi(get<2>(it),nullptr,10)-get<1>(it)+1, get<1>(it)+1); //wt->deleteChars(length_del[i], del_indexes[i]+1);
    		total_length_del += (stoi(get<2>(it),nullptr,10)-get<1>(it));
    		test_count++;
		}
		if(get<0>(it)=='S') {
			//cout<<"SNP "<<get<2>(it)<<" at "<<get<1>(it) << endl;
			uchar *ins = new uchar[ (get<2>(it)).length()+1 ];
			strcpy( (char*)ins, (get<2>(it)).c_str() );
			
			//cout<<"Deleting from "<< get<1>(it) <<" to "<< (get<2>(it)).length()+get<1>(it)-1 << endl;
			wt->deleteChars((get<2>(it)).length(), get<1>(it)+1);
			//cout<<"Inserting "<<get<2>(it)<<" at "<<get<1>(it) << endl;
			wt->addChars(ins, get<2>(it).length(), get<1>(it)+1);
			test_count++;
		}
  	}
	gettimeofday(&end, &tzp);
	std::string message = std::string("DynSA: Updates: ");
	print_time_elapsed(message, &start, &end);

	//Now extract substrings
    std::vector<std::pair<long,long>> substrings;
    std::ifstream inputfile (argv[4]);
    std::string line, pos, len;
    if (inputfile.is_open())
    {
        while(getline(inputfile, line))
        {
            std::stringstream linestream(line);
            getline(linestream, pos, ',');
            getline(linestream, len, ',');
            substrings.push_back(std::make_pair(stol(pos,nullptr,10),stol(len,nullptr,10)));
            line.clear(); pos.clear(); len.clear();
        }
    }
    else
    {
        std::cerr << "Houston, we have a problem opening the substring file" << std::endl;
        exit(-1);
    }

    gettimeofday(&start, &tzp);
    for(auto substr: substrings){
        //std::cout << substr.first << " " << substr.second << std::endl;
	wt->retrieveTextFactor(substr.first, substr.second);
    }
    gettimeofday(&end, &tzp);
    message.clear();
    message = "Extracted " + std::to_string(substrings.size()) + " substrings: ";
    print_time_elapsed(message, &start, &end);


	delete [] text;
  	delete [] f;
  	cout << "Done..." << endl;

}

//Edited for benchmarking with DynHash
int main(int argc, char *argv[]) {

	//benchmark_edits(argc, argv);
	//benchmark_search(argc, argv);
	benchmark_substr(argc,argv);
}

