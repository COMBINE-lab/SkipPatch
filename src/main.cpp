#include "genome.h"
#include "utils.h"
#include "test.h"
#include "benchmark.h"

using namespace std;

void wabi_example(){

	genome example;
	example.set_reference("AGCTTTTCATTCTGA");
	example.construct_hash();
	example.display_hash();
	
	example.snp_at(4,"G");
	example.display_hash();
	
	example.insert_at("TGAA",7);
	example.display_hash();
	example.get_skip_list().print_list();
	example.get_skip_list().print_base_level();
	example.display_updated_genome();
	
	example.insert_at("AT",3);
	example.insert_at("TAC",16);
	example.display_hash();
	example.get_skip_list().print_list();
	example.get_skip_list().print_base_level();
	example.display_updated_genome();

}

int main(int argc, char* argv[]){
	
	/*
	if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " input_file_name" << " K" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
	*/

	genome g;
	g.get_input(argv[1]);
	g.construct_hash();

    //wabi_example();

	//test();
	benchmark(g,argv[2],argv[3],atol(argv[4]));
	
	return 0;
}
