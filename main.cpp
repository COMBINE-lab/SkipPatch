#include "genome.h"
#include "utils.h"
#include "test.h"

#include <bits/stdc++.h>

using namespace std;

int main(){
	
	genome g;

	g.get_input();
	g.construct_hash();

	g.display_genome();
	g.display_hash();

	test_search();

	if(!g.snp_at(1,4,g.get_length()))
		cout<<"insert failed"<<endl;
	
	g.display_genome();
	g.display_hash();

	if(!g.snp_at(5,1,g.get_length()))
		cout<<"insert failed"<<endl;

	g.display_genome();
	g.display_hash();

	if(!g.snp_at(5,3,g.get_length()))
		cout<<"insert failed"<<endl;

	g.display_genome();
	g.display_hash();

	if(!g.snp_at(2,5,g.get_length()))
		cout<<"insert failed"<<endl;

	g.display_genome();
	g.display_hash();

	test_search();
	
	return 0;
}