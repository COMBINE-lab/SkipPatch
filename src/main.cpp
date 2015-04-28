#include "genome.h"
#include "utils.h"
#include "test.h"
#include "benchmark.h"

using namespace std;

int main(){
	
	genome g;

	g.get_input();
	g.construct_hash();

	test();
	benchmark(g);
	
	return 0;
}
