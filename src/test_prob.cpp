#include <utility>
#include <iostream>
#include <vector>
#include <string>
#include <tuple>

#include "common.h"
#include "test_prob.h"
#include "test.h"
#include "benchmark.h"

/**
 * Naively inserts, deletes and searches for substrings to
 * test if SkipPatch works correctly
 */
void test_search_naive(genome &g, const std::string path_to_query_file, long queryFrequency, long queryCount, long iterations) {

	LOGINFO(FILE_LOGGER, "Start: Testing Naive Search");

	std::string reference = g.get_reference();

	std::vector<std::tuple<std::string, std::string, std::string>> edit;
	std::vector<std::tuple<std::string, std::string, std::string, long>> query;

	parse_query_file(path_to_query_file, edit, query, queryFrequency, queryCount, iterations);

	std::string resultsFile = resultsPath + ".query.naive.out";
	std::ofstream query_out_file(resultsFile);
	LOGINFO(FILE_LOGGER, "Naive queries output file: "+resultsFile);

	for (int j = 0; j < iterations; j++) {

		long c = j * queryFrequency;

		for (int i = 0; i < queryFrequency; i++) {
			if (std::get<0>(edit[i + c]) == "I") {
				reference.insert(stol(std::get<1>(edit[i + c])), std::get<2>(edit[i + c]));
			} else if (std::get<0>(edit[i + c]) == "D") {
				reference.erase(stol(std::get<1>(edit[i + c])), stol(std::get<2>(edit[i + c])) - stol(std::get<1>(edit[i + c])) + 1);
			}
		}

		struct timeval start, end;
		struct timezone tzp;

		gettimeofday(&start, &tzp);
		for (int i = 0; i < queryCount; i++) {
			long c = j * queryCount;
			std::string read = std::get<1>(query[i + c]);

			//Naive search for testing
			std::vector<size_t> positions;
			size_t pos = reference.find(read,0);
			while (pos != std::string::npos) {
				positions.push_back(pos);
				pos = reference.find(read, pos + 1);
			}

			query_out_file << read << "\t";
			for (size_t p : positions) {
				query_out_file << p << " ";
			}
			query_out_file << std::endl;
		}
		gettimeofday(&end, &tzp);
		std::string message = "Search Iteration " + std::to_string(j);
		print_time_elapsed(message, &start, &end);

	}

	LOGINFO(FILE_LOGGER, "Complete: Testing Naive Search");

}
