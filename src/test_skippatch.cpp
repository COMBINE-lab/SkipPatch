#include <utility>
#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <cassert>

#include "common.h"
#include "test_skippatch.h"
#include "benchmark.h"
#include "genome.h"

/**
 * Naively inserts, deletes and searches for substrings to
 * test if SkipPatch search works correctly
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

	long err_count = 0;

	for (int j = 0; j < iterations; j++) {

		LOGINFO(FILE_LOGGER,"BEGIN: Iteration " + std::to_string(j));

		long c = j * queryFrequency;

		for (int i = 0; i < queryFrequency; i++) {

			if(reference.length()<=200){
				LOGDEBUG(FILE_LOGGER, "Before:   " + reference);
			}

			if (std::get<0>(edit[i + c]) == "I") {

				std::string ins = std::get<2>(edit[i + c]);
				long pos = stol(std::get<1>(edit[i + c]));
				LOGDEBUG(FILE_LOGGER,"Inserting " + ins + " at " + std::to_string(pos));
				g.insert_at(ins, pos);
				reference.insert(pos+1, ins);

			} else if (std::get<0>(edit[i + c]) == "D") {

				long pos = stol(std::get<1>(edit[i + c]));
				long len = stol(std::get<2>(edit[i + c])) - stol(std::get<1>(edit[i + c])) + 1;
				LOGDEBUG(FILE_LOGGER, "Deleting " + std::to_string(len) + " characters from position " + std::to_string(pos));
				if (g.delete_at(pos, len)) {
					reference.erase(pos, len);
				} else {
					LOGALERT(FILE_LOGGER, "Invalid delete.");
					continue;
				}

			} else if (std::get<0>(edit[i + c]) == "S") {

				std::string snp = std::get<1>(edit[i + c]);
				long pos = stol(std::get<2>(edit[i + c]));
				LOGDEBUG(FILE_LOGGER,"Substituting " + snp + " at " + std::to_string(pos) );
				g.snp_at(pos, snp);
				reference.replace(pos, snp.length(), snp);
			}

			if (reference.length() <= 200) {
				LOGDEBUG(FILE_LOGGER, "After: " + reference);
			}

			if (!(reference == g.read_reference_at(0, 0, LONG_MAX))) {
				LOGALERT(FILE_LOGGER, "Testing Error...");
				LOGALERT(FILE_LOGGER, "For: " + std::get<0>(edit[i + c])  + " " + std::get<1>(edit[i + c])+ " " + std::get<2>(edit[i + c]) );

				if(reference.length()<=200){
					LOGDEBUG(FILE_LOGGER, "Actual:   "+ g.read_reference_at(0, 0, LONG_MAX));
					LOGDEBUG(FILE_LOGGER, "Expected: "+ reference);
				}
				LOGALERT(FILE_LOGGER, "Quitting.");
				exit(-1);
			}

		}

		test_hash(g, reference);

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
			for(auto p: positions){
				LOGDEBUG(FILE_LOGGER, read + " Actual: " + std::to_string(p));
			}

			std::sort(positions.begin(), positions.end());
			std::sort(positions_naive.begin(), positions_naive.end());

			if (positions!=positions_naive) {
				LOGALERT(FILE_LOGGER, "Testing Error...");
				LOGALERT(FILE_LOGGER, "While searching for: " + read);

				for(auto p: positions_naive){
					LOGALERT(FILE_LOGGER, read + " Expected: " + std::to_string(p));
				}
				for(auto p: positions){
					LOGALERT(FILE_LOGGER, read + " Actual: " + std::to_string(p));
				}

				err_count++;
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
		LOGINFO(FILE_LOGGER,"COMPLETE: Iteration " + std::to_string(j));
		LOGINFO(FILE_LOGGER,"----------------------------------------------------------------------");

	}

	if(err_count>0){
		LOGALERT(FILE_LOGGER, "Found " + std::to_string(err_count) + " errors");
		LOGALERT(FILE_LOGGER, "Quitting... Bye!");
		exit(-1);
	}

	LOGINFO(FILE_LOGGER, "Complete: Testing Naive Search");

}

/**
 * Performs edits on the input sequence using the genome functions
 * and the naive string insert, erase and replace functions.
 * Compares at each stage if the reference generated is correct.
 * Calls test_hash() to verify if the hash is correct.
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
	g.construct_hash();
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

				long pos = stol(std::get<1>(it));
				long len = stol(std::get<2>(it)) - stol(std::get<1>(it)) + 1;
				LOGDEBUG(FILE_LOGGER, "Deleting " + std::to_string(len) + " characters from position " + std::to_string(pos));

				if (g.delete_at(pos, len)) {
					reference.erase(pos, len);
				} else {
					LOGALERT(FILE_LOGGER, "Invalid delete.");
					continue;
				}
				del_count++;

			} else if (std::get<0>(it) == "S") {
				LOGDEBUG(FILE_LOGGER,"Substituting " + std::get<1>(it) + " at " + std::get<2>(it));
				reference.replace(stol(std::get<1>(it)), std::get<2>(it).length(), std::get<2>(it));
				g.snp_at(stol(std::get<1>(it)), std::get<2>(it));
				snp_count++;
			}

			total_edits--;

			if (reference != g.read_reference_at(0, 0, LONG_MAX)) {
				LOGALERT(FILE_LOGGER, "Testing Error...");
				LOGALERT(FILE_LOGGER, "For: " + std::get<0>(it)  + " " + std::get<1>(it)+ " " + std::get<2>(it) );
				LOGALERT(FILE_LOGGER, "Quitting.");
				exit(-1);
			}
		}
	}

	test_hash(g, reference);

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
/**
 * Validates the hash which is constructed and modified (due to indels and SNPs).
 * Ensures that sampling does not result in any part of the genome not being hashed, and therefore not searchable
 * Validates three required conditions:
 *  - Two consecutive k-mers hashed should not be more than distance S apart, where S represents the sampling factor
 *  	(in order to avoid segments of the reference which have not been hashed)
 *  - No k-mer which should have been deleted must be present in the hash
 *  - Every k-mer present in the hash must be present at the correct genome position
 *  	(The set of positions of a hashed kmer must at least be a subset of all the positions it occurrs in the updated reference)
 */
void test_hash(genome &g, std::string updated_reference) {

	LOGINFO(FILE_LOGGER, "START: Testing Hash");

	std::unordered_map<uint64_t, std::vector<long>> hash = g.get_hash();

	LOGINFO(FILE_LOGGER,"Checking if at least every Sth k-mer is hashed");
	bool valid_hash = true;
	int off = 0;
	for (auto it = updated_reference.begin(); it < updated_reference.end()-K-1; it++) {

		std::string kmer(it, it + K);

		auto f = hash.find(str_to_int(kmer));
		if (f != hash.end()) { //kmer is found
			LOGTRACE(FILE_LOGGER, kmer);
			valid_hash = true;
			off = 0;
		} else {
			off++;
			if (off > S) {
				valid_hash = false;
				LOGALERT(FILE_LOGGER, "Invalid hash. Two hashed kmers are more than S apart.");
				LOGDEBUG(FILE_LOGGER, kmer);
				LOGALERT(FILE_LOGGER, "Quitting.. Bye!");
				exit(-1);
			}
		}
	}
	LOGINFO(FILE_LOGGER,"SUCCESS. At least every Sth k-mer is hashed");

	//Check if every k-mer in the hash actually exists in the updated genome
	//Ensure that no kmer which should have been deleted from a position was actually deleted

	LOGINFO(FILE_LOGGER,"Checking if every kmer in tha hash is present at valid positions and no nonexistent kmers are present in the hash");
	for (auto entry : hash) {

		std::string key = int_to_str(entry.first);

		//Make sure there isn't a kmer in the hash table which was supposed to be deleted
		if(updated_reference.find(key) == std::string::npos){
			LOGALERT(FILE_LOGGER, "Nonexistent k-mer present in hash: " + key);
			exit(-1);
		}

		//Find all positions in the hash
		std::vector<long> p_found = entry.second;
		std::sort(p_found.begin(), p_found.end()); // genome positions

		//Find all positions in the updated reference
		std::vector<long> p_actual;
		long pos = updated_reference.find(key);
		while (pos != std::string::npos) {
			p_actual.push_back(g.get_genome_position_from_virtual_position(pos));
			pos = updated_reference.find(key, pos + 1);
		}
		std::sort(p_actual.begin(), p_actual.end());

		//p_found should be a subset of p_actual
		if (!std::includes(p_actual.begin(), p_actual.end(), p_found.begin(), p_found.end())) {
			LOGALERT(FILE_LOGGER, "K-mer present at incorrect position: " + key);
			for(auto p: p_actual){
				LOGALERT(FILE_LOGGER, key + " Actual: " + std::to_string(p));
			}
			for(auto p: p_found){
				LOGALERT(FILE_LOGGER, key + " Found: " + std::to_string(p));
			}
			LOGALERT(FILE_LOGGER, "Quitting.. Bye!");
			exit(-1);
		}
	}
	LOGINFO(FILE_LOGGER,"SUCCESS. Every kmer in the hash is present at valid positions and no nonexistent kmers are present in the hash");


	LOGINFO(FILE_LOGGER, "COMPLETE: Testing Hash");

}
