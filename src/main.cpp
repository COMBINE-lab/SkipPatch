#include "genome.h"
#include "utils.h"
#include "test.h"
#include "benchmark.h"

using namespace std;

int main(){
	
	genome g;
	g.get_input();
	g.construct_hash();

	g.insert_at("CAT",7);
	
	test();
	//benchmark(g);
	
	return 0;
}
