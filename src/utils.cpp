#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <sys/resource.h>
#include <sys/time.h>
#include <time.h>

#include "genome.h"
#include "utils.h"

std::string alphabet = "AGCT"; //used for generating random string

enum DNA {A='A', T='T', C='C', G='G'};
/**
 * Logs time elapsed in seconds
 */
void print_time_elapsed(std::string desc, struct timeval* start,
		struct timeval* end) {

	/*
	 struct timeval {
	 time_t      tv_sec;
	 suseconds_t tv_usec;
	 }*/
	struct timeval elapsed;

	if (start->tv_usec > end->tv_usec) {
		end->tv_usec += 1000000;
		end->tv_sec--;
	}
	elapsed.tv_usec = end->tv_usec - start->tv_usec;
	elapsed.tv_sec = end->tv_sec - start->tv_sec;
	float time_elapsed = (elapsed.tv_sec * 1000000 + elapsed.tv_usec)/1000000.f;

	std::string message = desc + " Total Time Elapsed: "  + std::to_string(time_elapsed)  + " seconds";

	spdlog::get(FILE_LOGGER)->info() << message;

	return;
}

/**
 * Skips reading one line of the input
 */
void ignore_first_line() {
	std::string input;
	getline(std::cin, input);
}

/**
 * Checks if the input is valid
 */
bool is_valid_input(long pos, long len, long max_len) {

	if (pos < 0) {
		std::cout << "Position at which insertion is to be done cannot be negative" << std::endl;
		return false;
	}
	if (len <= 0) {
		std::cout << "Minimum length of the insertion should be 1" << std::endl;
		return false;
	}
	if ((pos + len - 1) >= max_len) {
		std::cout << "Insertion out of bounds" << std::endl;
		return false;
	}
	return true;
}

/**
 * Generates a random string (DNA: ATCG) of the given length
 */
std::string generate_random_string(long len) {

	std::srand(unsigned(std::time(0))); //use current time as seed for random generator
	std::string random_str;
	for (long i = 0; i < len; i++) {
		random_str += alphabet[std::rand() % (alphabet.size())];
	}
	std::srand(std::time(0)); //use current time as seed for random generator
	random_shuffle(random_str.begin(), random_str.end()); //*now really random :)

	return random_str;
}

/**
 * Returns a vector of the list of positions where a substring occurs in a string
 */
std::vector<long> find_substr(std::string str, std::string substr) {

	std::vector<long> positions;
	long pos = str.find(substr, 0);
	while (pos != -1) {
		positions.push_back(pos);
		pos = str.find(substr, pos + 1);
	}

	return positions;
}

/**
 * Generate a certain number (insert_count) of random inserts of a particular length (sequence_length)
 */
std::vector<std::pair<long, char>> generate_random_inserts(int insert_count,
		long sequence_length) {

	std::srand(std::time(0)); //use current time as seed for random generator

	std::vector<std::pair<long, char>> random_inserts(insert_count);
	for (int i = 0; i < insert_count; i++) {
		long random_pos = (std::rand() % sequence_length);
		char random_alphabet = alphabet[(std::rand() % alphabet.size())];
		random_inserts[i] = std::make_pair(random_pos, random_alphabet);
	}

	return random_inserts;
}

void format_path(std::string &path){
	if(path[path.length()-1]!='/')
		path+="/";
}

/**
 * Checks if a file exists. If it doesn't, exit the program
 */
bool fileExists(std::string filePath) {

	if (!std::ifstream(filePath)) {
		LOGALERT(FILE_LOGGER, "File not found: " + filePath);
		LOGALERT(FILE_LOGGER, "Quitting.. Bye!");
		exit(-1);
		return false;
	}
	return true;
}

/**
 * Converts a string of "ATCG" to a uint64_t
 * where each character is represented by using only two bits
 */
uint64_t str_to_int(std::string str) {

	uint64_t strint = 0;

	for (auto it = str.begin(); it != str.end(); it++) {

		uint8_t curr = 0;

		switch (*it) {
			case DNA::A: {
				curr = 0;
				break;
			}
			case DNA::T: {
				curr = 1;
				break;
			}
			case DNA::C: {
				curr = 2;
				break;
			}
			case DNA::G: {
				curr = 3;
				break;
			}
			default:{
				///?
			}
		}
		strint = strint | curr;
	    strint = strint << 2;
	}

	return strint >> 2;
}

/**
 * Converts a uint64_t consisting of alphabets ATCG (represented using two bits each)
 * into a readable string containing the alphabets as characters
 */
std::string int_to_str(const uint64_t strint) {

	std::string str = "";
	uint64_t intstr;

	int shift = 64 - 2*K;

	int i = 0;
	while (i < K) {

		intstr = strint;
		intstr = intstr >> (K-i-1)*2;
		intstr = intstr & 3;

		char curr;
		switch (intstr) {
		case 0: {
			curr = 'A';
			break;
		}
		case 1: {
			curr = 'T';
			break;
		}
		case 2: {
			curr = 'C';
			break;
		}
		case 3: {
			curr = 'G';
			break;
		}
		default: {
			///?
		}
		}

		str += curr;
		i++;
	}

	return str;
}
