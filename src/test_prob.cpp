#include <utility>
#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <cassert>

#include "common.h"
#include "test_prob.h"
#include "benchmark.h"

/**
 * Naively inserts, deletes and searches for substrings to
 * test if SkipPatch works correctly
 *
 * I 442077 ATC
 * D 3975001 3975005
 * I 2052837 GCG
 * Q GCGCCAGCGTCGGCAAGGGT N 1
 * Q TGTTAACGCGCTGGCGGCGA N 1
 * Q TTCCACGCCTTACACCGTAC N 1
 *
 */
void test_search_naive(genome &g, const std::string path_to_query_file, long queryFrequency, long queryCount, long iterations) {

	LOGINFO(FILE_LOGGER, "Start: Testing Naive Search");
	g.construct_hash();
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

				std::string ins = std::get<2>(edit[i + c]);
				long pos = stol(std::get<1>(edit[i + c]));
				LOGDEBUG(FILE_LOGGER,"Inserting " + ins + " at " + std::to_string(pos));
				reference.insert(pos+1, ins);
				g.insert_at(ins, pos);

			} else if (std::get<0>(edit[i + c]) == "D") {

				long pos = stol(std::get<1>(edit[i + c]));
				long len = stol(std::get<2>(edit[i + c])) - stol(std::get<1>(edit[i + c])) + 1;
				LOGDEBUG(FILE_LOGGER, "Deleting " + std::to_string(len) + " characters from position " + std::to_string(pos));
				reference.erase(pos, len);
				g.delete_at(pos, len);

			} else if (std::get<0>(edit[i + c]) == "S") {

				std::string snp = std::get<1>(edit[i + c]);
				long pos = stol(std::get<2>(edit[i + c]));
				LOGDEBUG(FILE_LOGGER,"Substituting " + snp + " at " + std::to_string(pos) );
				reference.replace(pos, snp.length(), snp);
				g.snp_at(pos, snp);

			}
			if (!(reference == g.read_reference_at(0, 0, reference.length()))) {
				LOGALERT(FILE_LOGGER, "Testing Error...");
				LOGALERT(FILE_LOGGER, "For: " + std::get<0>(edit[i + c])  + " " + std::get<1>(edit[i + c])+ " " + std::get<2>(edit[i + c]) );
				LOGALERT(FILE_LOGGER, "Quitting.");
				exit(-1);
			}

		}

		struct timeval start, end;
		struct timezone tzp;

		gettimeofday(&start, &tzp);
		for (int i = 0; i < queryCount; i++) {
			long c = j * queryCount;
			std::string read = std::get<1>(query[i + c]);

			//Search substring
			std::vector<long> positions = g.search(read);
			for(auto p: positions){
				LOGDEBUG(FILE_LOGGER, read + " Found: " + std::to_string(p));
			}

			//Naive search for testing
			std::vector<long> positions_naive;
			long pos = reference.find(read,0);
			while (pos != std::string::npos) {
				positions_naive.push_back(pos);
				pos = reference.find(read, pos + 1);
			}
			for(auto p: positions_naive){
				LOGDEBUG(FILE_LOGGER, read + " Expected: " + std::to_string(p));
			}

			std::sort(positions.begin(), positions.end());
			std::sort(positions_naive.begin(), positions_naive.end());

			if (positions!=positions_naive) {
				LOGALERT(FILE_LOGGER, "Testing Error...");
				LOGALERT(FILE_LOGGER, "While searching for: " + read);

				for(auto p: positions_naive){
					LOGALERT(FILE_LOGGER, read + " Found: " + std::to_string(p));
				}
				for(auto p: positions){
					LOGALERT(FILE_LOGGER, read + " Actual: " + std::to_string(p));
				}

				LOGALERT(FILE_LOGGER, "Quitting.");
				exit(-1);
			}

			query_out_file << read << "\t";
			for (size_t p : positions_naive) {
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

/**
 * Performs edits on the input sequence using the genome functions
 * and the naive string insert, erase and replace functions.
 * Compares at each stage if the reference generated is correct.
 *
 * I 4439799 T
 * S 2261415 C
 * D 494753 494753
 *
 */
void test_edits_naive(genome &g, std::string edits_file, const long number_of_edits) {

	LOGINFO(FILE_LOGGER, "Start: Testing Naive Edits");

	//Create a copy of the genome, and the reference.
	//Naively edit the copy of the reference sequence and check if the updated reference sequence is the same.
	std::string reference = g.get_reference();

	std::vector<std::tuple<std::string, std::string, std::string>> edits;
	parse_edit_file(edits, edits_file);

	LOGINFO(FILE_LOGGER,"Total number of edits to be performed: " + std::to_string(edits.size()));
	long ins_count = 0, del_count = 0, snp_count = 0;

	struct timeval start, end;
	struct timezone tzp;

	long total_edits = edits.size();
	if (number_of_edits > 0) {
		total_edits = number_of_edits; //only if the number of edits has been specified
	}

	gettimeofday(&start, &tzp);

	for (auto it : edits) {

		if (total_edits > 0) {

			if (std::get<0>(it) == "I") {
				LOGDEBUG(FILE_LOGGER,"Inserting " + std::get<2>(it) + " at " + std::get<1>(it));
				reference.insert(stol(std::get<1>(it))+1, std::get<2>(it));
				g.insert_at(std::get<2>(it),stol(std::get<1>(it)));
				ins_count++;
			} else if (std::get<0>(it) == "D") {
				LOGDEBUG(FILE_LOGGER,"Deleting " + std::to_string(stol(std::get<2>(it)) - stol(std::get<1>(it)) + 1) + " characters from position " + std::get<1>(it));
				reference.erase(stol(std::get<1>(it)), stol(std::get<2>(it))-stol(std::get<1>(it))+1);
				g.delete_at(stol(std::get<1>(it)), stol(std::get<2>(it)) - stol(std::get<1>(it)) + 1);
				del_count++;
			} else if (std::get<0>(it) == "S") {
				LOGDEBUG(FILE_LOGGER,"Substituting " + std::get<1>(it) + " at " + std::get<2>(it));
				reference.replace(stol(std::get<1>(it)), std::get<2>(it).length(), std::get<2>(it));
				g.snp_at(stol(std::get<1>(it)), std::get<2>(it));
				snp_count++;
			}

			total_edits--;

			if (!(reference == g.read_reference_at(0, 0, reference.length()))) {
				LOGALERT(FILE_LOGGER, "Testing Error...");
				LOGALERT(FILE_LOGGER, "For: " + std::get<0>(it)  + " " + std::get<1>(it)+ " " + std::get<2>(it) );
				LOGALERT(FILE_LOGGER, "Quitting.");
				exit(-1);
			}

		}
	}

	gettimeofday(&end, &tzp);

	print_time_elapsed("Edits: ", &start, &end);

	LOGINFO(FILE_LOGGER, "Total Insertions: " + std::to_string(ins_count));
	LOGINFO(FILE_LOGGER, "Total Deletions: " + std::to_string(del_count));
	LOGINFO(FILE_LOGGER, "Total SNPs: " + std::to_string(snp_count));

	LOGINFO(FILE_LOGGER, "End: Testing Naive Edits");

}


/**
 * Performs edits on the input sequence as specified in the edits_file_path
 * Then extracts substrings
 *
 */
void test_substr_naive(genome &g, const std::string substr_file_path, const std::string edits_file_path, long num_edits) {

	LOGINFO(FILE_LOGGER, "Start: Testing Naive Substring Extraction");
	g.construct_hash();
	//benchmark_edits(g, edits_file_path, num_edits);
	test_edits_naive(g, edits_file_path, num_edits);

	//TODO: Incorrect. Fix this.
	std::string edited_reference = g.read_reference_at(0, 0, g.get_reference().length());

	std::vector<std::pair<long, long>> substrings;
	std::ifstream substr_file(substr_file_path);
	std::string line, pos, len;

	if (substr_file.is_open()) {
		while (!substr_file.eof())
    	{
    		std::getline(substr_file,line);
    		if(line.length()>0){
				std::stringstream stream(line);
				std::string l;
				std::vector<std::string> substr_details;
				while (std::getline(stream, l, ',')) {
					substr_details.push_back(l);
				}
				substrings.push_back(
						std::make_pair(stol(substr_details[0], nullptr, 10),
								stol(substr_details[1], nullptr, 10)));
    		}
    	}
	} else {
		std::string error_message = "Failed to open file: " + substr_file_path;
		LOGALERT(FILE_LOGGER, error_message);
		LOGALERT(FILE_LOGGER, "Exiting program");
		exit(-1);
	}

	struct timeval start, end;
	struct timezone tzp;
	long temp = 0;
	gettimeofday(&start, &tzp);

	for (auto substr : substrings) {
		std::string substring = g.read_reference_abs_at(substr.first, substr.second, temp);
		std::string substring_naive = edited_reference.substr(substr.first, substr.second);

		if (substring != substring_naive) {
			LOGALERT(FILE_LOGGER, "Testing Error...");
			LOGALERT(FILE_LOGGER, "For: " + std::to_string(substr.first)  + " " + std::to_string(substr.second));
			LOGALERT(FILE_LOGGER, "Expected: " + substring_naive);
			LOGALERT(FILE_LOGGER, "Found: " + substring);
			//exit(-1);
		}

	}

	gettimeofday(&end, &tzp);

	std::string message = "Extracted " + std::to_string(substrings.size())
			+ " substrings: ";
	print_time_elapsed(message, &start, &end);




	LOGINFO(FILE_LOGGER, "Complete: Testing Naive Substring Extraction");

}
