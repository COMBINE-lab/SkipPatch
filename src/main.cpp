#include "genome.h"
#include "utils.h"
#include "test.h"
#include "benchmark.h"

using namespace std;

int main(){
	
	genome g;
	g.get_input();
	g.construct_hash();

	std::cout << g.get_reference() << std::endl;
	g.insert_at("CAT",7);
	g.insert_at("TTTTT",15);
	g.insert_at("CCGGCCGG",50);
	std::cout << g.get_updated_reference() << std::endl;

	std::cout<<"Searching.. "<<std::endl;	

	std::vector<std::string> reads {"ATTAGC","TATCAT","GATCGCAGATCGA"};
	for(std::string read: reads){
		std::cout << read << ": ";
		
		std::vector<long> genome_p = g.find(read);
		for(long p: genome_p){
			std::cout<<p<<"\t";
		}
		std::vector<long> virtual_p = find_substr(g.get_updated_reference(),read);
		for(long p: virtual_p){
			std::cout<<p<<"\t";
		}

		std::cout << std::endl;
	}	

	test();
	//benchmark(g);
	
	return 0;
}
