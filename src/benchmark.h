#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <bits/stdc++.h>
#include "genome.h"
#include "utils.h"
#include "spdlog/spdlog.h"

namespace spd = spdlog;

	void benchmark_construction(genome &);

	void benchmark_search(genome &, int, int);

	void benchmark_search(genome &,std::string, long, long, long);

	void benchmark_edits(genome &, std::string, const long);

	void benchmark_snp(genome &);

	void benchmark_substring(genome &, std::string);

#endif
