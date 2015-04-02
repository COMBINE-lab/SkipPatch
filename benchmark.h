#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <iostream>
#include <vector>
#include <pair>
#include <string>

std::vector<std::pair<int,char> > generateRandomInserts(int);
void benchmarkInsert(std::string);

#endif