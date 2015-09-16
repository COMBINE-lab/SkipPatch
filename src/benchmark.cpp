#include "benchmark.h"
#include "utils.h"

#define QF 302348
#define QC 5000
#define N 10
#define TESTS 100
using namespace std;

/**
 * Benchmarks the time taken to construct the hash from the reference sequence
 */
void benchmark_construction(genome &g) {

	struct timeval start, end;
	struct timezone tzp;

	gettimeofday(&start, &tzp);
	g.construct_hash();
	gettimeofday(&end, &tzp);

	print_time_elapsed("Constructing Hash: ", &start, &end);

	return;
}

void benchmark_search(genome &g, int num_patterns, int pattern_len) {

	struct timeval start, end;
	struct timezone tzp;

	gettimeofday(&start, &tzp);
	for (int i = 0; i < num_patterns; i++) {
		string temp = generate_random_string(pattern_len);
		auto vec = g.search(temp);
	}
	gettimeofday(&end, &tzp);

	std::string message = "Searching " + std::to_string(num_patterns)
			+ " patterns of length " + std::to_string(pattern_len) + "...\t";
	print_time_elapsed(message, &start, &end);

	return;
}

void benchmark_snp(genome &g) {
	struct timeval start, end;
	struct timezone tzp;

	std::vector<std::pair<long, char>> random = generate_random_inserts(TESTS,
			g.get_length());

	gettimeofday(&start, &tzp);

	for (std::vector<std::pair<long, char>>::iterator i = random.begin();
			i != random.end(); i++) {
		long position = i->first;
		char character = i->second;
		g.snp_at(position, std::string(1, character));
	}
	gettimeofday(&end, &tzp);

	std::string message = "Inserting " + std::to_string(TESTS)
			+ " SNPs at random locations" + "...\t";
	print_time_elapsed(message, &start, &end);

	return;
}

void benchmark_insert(genome &g) {
	struct timeval start, end;
	struct timezone tzp;

	std::vector<std::pair<long, char>> random = generate_random_inserts(TESTS,
			g.get_length());

	gettimeofday(&start, &tzp);

	for (std::vector<std::pair<long, char>>::iterator i = random.begin();
			i != random.end(); i++) {
		long position = i->first;
		char character = i->second;
		if (position > K && g.get_length() - K) { //To avoid corner cases in insert_at which haven't yet been handled
			g.insert_at(std::string(1, character), position);
		}
	}
	gettimeofday(&end, &tzp);

	std::string message = "Inserting " + std::to_string(TESTS)
			+ " strings at random locations" + "...\t";
	print_time_elapsed(message, &start, &end);

	return;
}

/**
 * Parses the input file which contains the edits to perform on a genome
 * Example input lines:
 * 	I 4439799 T
 * 	S 2261415 C
 * 	D 494753 494753
 */
void get_input(std::vector<std::tuple<char, long, string, long>> &edits, std::string edits_file_path) {

	std::string g;
	std::ifstream edits_file(edits_file_path);
	
	std::string edit;
	if (edits_file.is_open()) {
		while (!edits_file.eof())
    	{
    		std::getline(edits_file,edit);
    		std::stringstream edit_stream(edit);
    		std::string e;
    		std::vector<std::string> edit_details;
    		while (std::getline(edit_stream, e, ' ')) {
        		edit_details.push_back(e);
        	}

        	char edit_type = edit[0];
        	if(edit_type=='S' || edit_type=='I') {
        		edits.push_back(make_tuple(edit_type, stol(edit_details[1], nullptr, 10), edit_details[2] , 0));
        	} else if(edit_type=='D') {
        		edits.push_back(make_tuple(edit_type, stol(edit_details[1], nullptr, 10), "" , stol(edit_details[2], nullptr, 10)));	
        	}
    	}
    } else {
    	//TODO: Change to logging
		std::cerr << "[benchmark.cpp][get_input()] Failed to open file " << edits_file_path << std::endl;
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

void benchmark_search(genome &g,const string path_to_query_file){
  //std::cout<<"SEARCH BENCHMARKING START"<<std::endl;

  std::cout<<"BENCHMARKING START"<<std::endl;
  benchmark_construction(g);

  vector<tuple<char, long, long,string>> edit;
  vector<tuple<char,string,char,long>> query;
  get_input(path_to_query_file,edit,query);


  for(int j=0;j<N;j++)
  {
	long c = j*QF;
    for(int i=0;i<QF;i++)
    {
	if(get<0>(edit[i+c])=='I')
	      g.insert_at(get<3>(edit[i+c]),get<1>(edit[i+c]));
	else
	      g.delete_at(get<1>(edit[i+c]),get<2>(edit[i+c])-get<1>(edit[i+c])+1);

    }

  struct timeval start, end;
  struct timezone tzp;

  gettimeofday(&start, &tzp);
    for(int i=0;i<QC;i++)
    {
      long c= j*QC;
      g.search(get<1>(query[i+c]));
    }
    gettimeofday(&end, &tzp);
    string message = "Search Iteration "+ j;
    print_time_elapsed(message, &start, &end);

  }
    std::cout<<"BENCHMARKING END"<<std::endl;
}



/**
 * Benchmarks the time taken to perform all the edits specified in the edits_file
 * to the genome.
 */
void benchmark_edits(genome &g, std::string edits_file, const long number_of_edits) {

	std::cout << "BEGIN: BENCHMARKING EDITS" << std::endl;

	benchmark_construction(g);

	vector<tuple<char, long, std::string, long>> edit;
	get_input(edit, edits_file);
	long ins_count = 0, del_count = 0, snp_count = 0;

	struct timeval start, end;
	struct timezone tzp;

	gettimeofday(&start, &tzp);
	long total_edits = number_of_edits;
	std::vector<long> invalid_deletes;
	long edit_index = 0;

	for (auto it : edit) {

		if (total_edits > 0) {

			if (get<0>(it) == 'I') {
				g.insert_at(get<2>(it), get<1>(it));
				ins_count++;
			}

			if (get<0>(it) == 'D') {
				//TODO: Fix corner cases and remove
				if (!g.delete_at(get<1>(it), get<3>(it) - get<1>(it) + 1)) {
					invalid_deletes.push_back(edit_index);
					total_edits++;
				}
				del_count++;
			}

			if (get<0>(it) == 'S') {
				g.snp_at(get<1>(it), get<2>(it));
				snp_count++;
			}

			total_edits--;

		} else {
			std::cout << "Total edits: " << number_of_edits << std::endl;
			break;
		}
		edit_index++;
	}

	gettimeofday(&end, &tzp);

	print_time_elapsed("Edits: ", &start, &end);

	std::cout << "Total Insertions: " << ins_count << std::endl;
	std::cout << "Total Deletions: " << del_count << std::endl;
	std::cout << "Total SNPs: " << snp_count << std::endl;

	//TODO: Remove after deletion corner case is fixed
	//Store every invalid delete in a file
	std::ofstream invalid_deletes_file("/home/komal/data/invalid_deletes");
	for (auto invalid_delete : invalid_deletes)
		invalid_deletes_file << invalid_delete << "\n";
	invalid_deletes_file.close();

	std::cout << "END: BENCHMARKING EDITS" << std::endl;

}


void benchmark_substring(genome &g, std::string filepath){

	std::vector<std::pair<long,long>> substrings;
	std::ifstream inputfile (filepath);
	std::string line, pos, len;
	if (inputfile.is_open())
  	{
		while(getline(inputfile, line)){
	
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

	
        struct timeval start, end;
        struct timezone tzp;
	long temp;
        gettimeofday(&start, &tzp);

	for(auto substr: substrings){
		//std::cout << substr.first << " " << substr.second << std::endl;
		g.read_reference_abs_at(substr.first, substr.second, temp);
	}

	gettimeofday(&end, &tzp);

	std::string message = 	"Extracted " + std::to_string(substrings.size()) + " substrings: ";
   	print_time_elapsed(message, &start, &end);

}
