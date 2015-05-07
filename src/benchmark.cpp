#include "benchmark.h"
#include "utils.h"

#define TESTS 100
using namespace std;
void benchmark_construction(genome &g){

    //Calculating time elapsed
    struct timeval start, end;
    struct timezone tzp;

    gettimeofday(&start, &tzp);
    g.construct_hash();
    gettimeofday(&end, &tzp);

    print_time_elapsed("Constructing Hash: ", &start, &end);

//    gettimeofday(&start, &tzp);
//    std::string fname("hashdump.bin");
//    g.save_hash(fname);
//    gettimeofday(&end, &tzp);
//
//    print_time_elapsed("Saving hash to file: ", &start, &end);

    return;
}

void benchmark_search(genome &g, int num_patterns, int pattern_len){

	struct timeval start, end;
	struct timezone tzp;

	gettimeofday(&start, &tzp);
	for(int i=0;i<num_patterns;i++){
		string temp = generate_random_string(pattern_len);
//			cout<<temp<<" "<<i<<endl;
	auto vec = g.search(temp);
	}
	gettimeofday(&end, &tzp);

	std::string message = "Searching " + std::to_string(num_patterns) + " patterns of length "+ std::to_string(pattern_len) + "...\t";
	print_time_elapsed(message, &start, &end);

	return;
}

void benchmark_snp(genome &g)
{
    struct timeval start, end;
    struct timezone tzp;

    std::vector<std::pair<long,char>> random = generate_random_inserts(TESTS,g.get_length());

    gettimeofday(&start, &tzp);

    for(std::vector<std::pair<long,char>>::iterator i=random.begin(); i!=random.end(); i++){
        long position = i->first;
        char character = i->second;
		g.snp_at(position, std::string(1,character));
    }
    gettimeofday(&end, &tzp);

	std::string message = "Inserting " + std::to_string(TESTS) + " SNPs at random locations"+ "...\t";
    print_time_elapsed(message, &start, &end);

    return;
}
void benchmark_insert(genome &g)
{
    struct timeval start, end;
    struct timezone tzp;

    std::vector<std::pair<long,char>> random = generate_random_inserts(TESTS,g.get_length());

    gettimeofday(&start, &tzp);

    for(std::vector<std::pair<long,char>>::iterator i=random.begin(); i!=random.end(); i++){
        long position = i->first;
        char character = i->second;
        if(position>K && g.get_length()-K){ //To avoid corner cases in insert_at which haven't yet been handled
			g.insert_at(std::string(1,character), position);
		}
    }
    gettimeofday(&end, &tzp);

	std::string message = "Inserting " + std::to_string(TESTS) + " strings at random locations"+ "...\t";
    print_time_elapsed(message, &start, &end);

    return;
}
void get_input(vector<tuple<string,long,char,long>> &vec,string path)
{


//  vector<tuple<string,long,char>> vec;
  string g;
  ifstream myfile (path);
  if (myfile.is_open())
  {
    while (getline(myfile, g, ' ') )
    {
      //cout<<g<<"\t";
      char c = g[0];
      g.clear();
      getline( myfile,g, ' ');
      long l = stol(g,nullptr,10);
      //cout<<g<<" conv val "<<l<<"\t";
      g.clear();
      getline( myfile,g, '\n');
	long l_del=0;
	if(c=='D')
	{
		l_del = stol(g,nullptr,10);
	}
      vec.push_back(make_tuple(g,l,c,l_del));
      //cout<<g<<endl;
    }
  }
  else
  {
    cout<<"Cant open file :( "<<endl;
  }
  return;
}
void benchmark(genome &g,string path,const string reference_dump_path,const long edits){

	std::cout<<"BENCHMARKING START"<<std::endl;

	benchmark_construction(g);

  vector<tuple<string,long,char,long>> vec;
    get_input(vec,path);
	tuple<long,long,long> count = make_tuple(0,0,0);
	/*for(auto it:vec)
	{
		if(get<2>(it)=='I')
			get<0>(count)+=1;
//		if(get<2>(it)=='D')
//			get<1>(count)+=1;
		if(get<2>(it)=='S')
			get<2>(count)+=1;
<<<<<<< HEAD
	
	}*/

    struct timeval start, end;
    struct timezone tzp;

    gettimeofday(&start, &tzp);
 long totedits = edits;vector<long > d;long lc=0;
cout<<"total edits: "<<totedits;

   for(auto it:vec)
  {
	if(totedits>0)
	{
    //cout<<get<0>(it)<<"\t"<<get<1>(it)<<"\t"<<get<2>(it)<<endl;
	if(get<2>(it)=='I')
	{
//		cout<<"Inserting "<<get<0>(it)<<" at "<<get<1>(it);
		g.insert_at(get<0>(it),get<1>(it));
			get<0>(count)+=1;
	}
	if(get<2>(it)=='D')
	{
		//cout<<"Deletion ! not yet writtten";
		if(!g.delete_at(get<1>(it),get<3>(it)-get<1>(it)+1))	
	{	
				d.push_back(lc);	
				totedits++;	
	}
			get<1>(count)+=1;
		
	}
	if(get<2>(it)=='S')
	{
//		cout<<"Snp "<<get<0>(it)<<" at "<<get<1>(it);
		g.snp_at(get<1>(it),get<0>(it));
			get<2>(count)+=1;
	}
	totedits--;
	}
	else
	{
		cout<<endl<<edits<<" edits completed!";
		break;
	}
	lc++;
  }
 gettimeofday(&end, &tzp);

	std::string message = "Ins: " + std::to_string(get<0>(count)) + "\n"
				"Del: " + std::to_string(get<1>(count)) +"\n"
 				"Snp:  " + std::to_string(get<2>(count))+"\n" ;
    print_time_elapsed(message, &start, &end);
//	cout<<g.get_length();
    benchmark_search(g,10,25);
	cout<<endl<<"writing genome to a file .."<<endl;
	std::string output_file (reference_dump_path);
	std::ofstream outfile (output_file);
	outfile << g.read_reference_at(0,0,LONG_MAX);
	outfile.close();
	std::string output_file1 ("./edit");
	std::ofstream outfile1 (output_file1);
	for(auto it:d)
		outfile1 << it<<"\n";
	outfile1.close();;

//	g.read_reference_at(0,0,LONG_MAX);

//    g.get_skip_list().print_list();

    /*benchmark_snp(g);

    benchmark_insert(g);

    benchmark_search(g,TESTS,25);
    benchmark_search(g,TESTS,35);
    benchmark_search(g,TESTS,70);
    benchmark_search(g,TESTS,100);
   */
    std::cout<<"BENCHMARKING END"<<std::endl;
	//:read_reference_abs_at(const long abs_pos,const long len,long &genome_position,unsigned long &offset,string &kmer)


}

//Open test file
/*	std::fstream referenceFile;
referenceFile.open("./../data/input.txt");
if( !referenceFile || referenceFile.eof() ){
    std::cout << "Error: Reference Sequence File Not Found.\n"; exit(2);
}*/

