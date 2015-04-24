#include "genome.h"
#include "utils.h"
#include "test.h"
#include "benchmark.h"

using namespace std;

int main(){
	
	genome g;
	g.get_input();
	g.construct_hash();

	//std::cout << g.get_reference() << std::endl;
	//g.insert_at("CAT",7);
	//g.insert_at("TTTTT",15);
	//std::cout << g.get_updated_reference() << std::endl;
	
	test();
	//benchmark(g);
	
	return 0;
}
