#include "benchmark.h"

#define TESTS 100
std::vector<std::pair<long,char> > generateRandomInserts(long sequenceLength){

    std::srand(std::time(0)); //use current time as seed for random generator
    
    char alphabets[] = {'A', 'T', 'C', 'G'};
    std::vector<char> alphabet (alphabets, alphabets + sizeof(alphabets) / sizeof(char));    
    std::vector< std::pair<long,char> > edit(TESTS);

    for(int i=0; i<TESTS; i++){
        long randomP = (std::rand() % sequenceLength);   
        int randomA = (std::rand() % alphabet.size());  
        edit[i]=std::make_pair(randomP,alphabet[randomA]);
    }   

    return edit;
}   

void benchmarkSearch(genome &g,int num_patterns,int pattern_len){
  
  struct timeval start, end;
  struct timezone tzp;
  gettimeofday(&start, &tzp);
  for(int i=0;i<num_patterns;i++)
  {
    g.find(generate_random_string(pattern_len));
  }
  gettimeofday(&end, &tzp);
  cout<<" for "<<num_patterns<<" of len: "<<pattern_len<<"\t";
  print_time_elapsed("search: ", &start, &end);
}

void benchmarkSNP(genome &g)
{
    struct timeval start, end;
    struct timezone tzp;
    
    std::vector<std::pair<long,char> > random = generateRandomInserts(g.get_length());

    gettimeofday(&start, &tzp);
    
    for(std::vector<std::pair<long,char> >::iterator i=random.begin(); i!=random.end(); i++){
        long position = i->first;
        char character = i->second;
	g.snp_at(position,1);
    }
    gettimeofday(&end, &tzp);
    cout<<" for "<<TESTS<<" snips \t";
    print_time_elapsed("snips: ", &start, &end);
    
}

void benchmark(){

    //Calculating time elapsed
    struct timeval start, end;
    struct timezone tzp;

    //Open test file
    /*std::fstream referenceFile;
    referenceFile.open("data/input.txt");
    if( !referenceFile || referenceFile.eof() ){ 
        std::cout << "Invalid Reference Sequence File.\n"; exit(2); 
    }*/
    genome g;
    g.get_input();
    /*std::string reference;
    referenceFile >> reference;*/
    gettimeofday(&start, &tzp);
    g.construct_hash();
    
    gettimeofday(&end, &tzp);
    print_time_elapsed(" constructing hash: ", &start, &end);
    
    benchmarkSNP(g);
    /*std::vector<std::pair<long,char> > random = generateRandomInserts(g.get_length());

    gettimeofday(&start, &tzp);
    
    for(std::vector<std::pair<long,char> >::iterator i=random.begin(); i!=random.end(); i++){
        long position = i->first;
        char character = i->second;
	g.snp_at(position,1,g.get_length());
    }
    gettimeofday(&end, &tzp);
    print_time_elapsed("snips: ", &start, &end);*/
    /*
    benchmarkSearch(g,10,15);
    benchmarkSearch(g,10,25);
    benchmarkSearch(g,10,35);
    benchmarkSearch(g,10,70);
    benchmarkSearch(g,10,100);*/
}
