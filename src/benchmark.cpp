#include "benchmark.h"
#include "utils.h"

#define TESTS 100

void benchmark_construction(genome &g){

    //Calculating time elapsed
    struct timeval start, end;
    struct timezone tzp;

    gettimeofday(&start, &tzp);
    g.construct_hash();
    gettimeofday(&end, &tzp);
    
    print_time_elapsed("Constructing Hash: ", &start, &end);

    return;
}

void benchmark_search(genome &g, int num_patterns, int pattern_len){
  
	struct timeval start, end;
	struct timezone tzp;

	gettimeofday(&start, &tzp);
	for(int i=0;i<num_patterns;i++){
		g.find(generate_random_string(pattern_len));
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

void benchmark(genome &g){

	std::cout<<"BENCHMARKING START"<<std::endl;
    
	benchmark_construction(g);

    benchmark_snp(g);

    benchmark_search(g,TESTS,25);
    benchmark_search(g,TESTS,35);
    benchmark_search(g,TESTS,70);
    benchmark_search(g,TESTS,100);
   
    std::cout<<"BENCHMARKING END"<<std::endl;

}
	
//Open test file
/*	std::fstream referenceFile;
referenceFile.open("./../data/input.txt");
if( !referenceFile || referenceFile.eof() ){ 
    std::cout << "Error: Reference Sequence File Not Found.\n"; exit(2); 
}*/
 	