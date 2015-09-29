#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <bits/stdc++.h>
#include "genome.h"
#include "utils.h"
#include "spdlog/spdlog.h"

namespace spd = spdlog;

void benchmark_construction(genome &);

void parse_edit_file(
		std::vector<std::tuple<std::string, std::string, std::string>>&,
		std::string);

void benchmark_edits(genome&, std::string, const long);

void parse_query_file(const std::string,
		std::vector<std::tuple<std::string, std::string, std::string>>&,
		std::vector<std::tuple<std::string, std::string, std::string, long>>&,
		long, long, long);

void benchmark_search(genome&, const std::string, long, long, long);

void benchmark_substring(genome&, std::string);

#endif
