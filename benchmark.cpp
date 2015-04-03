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
        edit.push_back(std::make_pair(randomP,alphabet[randomA]));
    }   

    return edit;
}   

void benchmarkInsert(std::string testName){

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
    g.construct_hash();

    std::vector<std::pair<long,char> > random = generateRandomInserts(g.get_length());

    gettimeofday(&start, &tzp);
    
    for(std::vector<std::pair<long,char> >::iterator i=random.begin(); i!=random.end(); i++){
        long position = i->first;
        char character = i->second;
	g.snp_at(position,1,g.get_length());
        //g.snp_at(); //Insert function here..
    }

    gettimeofday(&end, &tzp);

    print_time_elapsed(testName, &start, &end);
}