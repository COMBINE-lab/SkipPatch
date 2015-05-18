#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <bits/stdc++.h>
#include "genome.h"
#include "utils.h"

//void benchmark(genome &,std::string, const std::string,const long);
void benchmark(genome &, std::string, const long);

void benchmark_construction(genome &);

void benchmark_search(genome &, int, int);

void benchmark_search(genome &,std::string);

void benchmark_snp(genome &);

void benchmark_substring(genome &, std::string);

#endif
