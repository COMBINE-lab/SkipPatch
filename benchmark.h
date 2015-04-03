#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <bits/stdc++.h>
#include "genome.h"
#include "utils.h"

std::vector<std::pair<long,char> > generateRandomInserts(long);
void benchmark();
void benchmarkSearch(genome);
void benchmarkSNP(genome);

#endif