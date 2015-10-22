#include <memory>

#include "genome.h"
#include "utils.h"
#include "test.h"
#include "test_prob.h"
#include "benchmark.h"
#include "ezOptionParser.hpp"
#include "spdlog/spdlog.h"

//#define TEST 1

using namespace ez;
using namespace std;

string loadHashPath="";
string resultsPath="";

void example() {

	genome example;
	example.set_reference("AGCTTTTCATTCTGA");
	example.construct_hash();
	example.display_hash();

	example.snp_at(4, "G");
	example.display_hash();

	example.insert_at("TGAA", 7);
	example.display_hash();
	example.get_skip_list().print_list();
	example.get_skip_list().print_base_level();
	example.display_updated_genome();

	example.insert_at("AT", 3);
	example.insert_at("TAC", 16);
	example.display_hash();
	example.get_skip_list().print_list();
	example.get_skip_list().print_base_level();
	example.display_updated_genome();

}

/**
 * Temporary - for testing.
 * Just loads the hash so that we can check its actual size,
 * and exits the program.
 */
void checkHashSize(std::string loadHashPath) {
	struct timeval start, end;
	struct timezone tzp;
	gettimeofday(&start, &tzp);
	genome g;
	g.load_hash(loadHashPath);
	gettimeofday(&end, &tzp);
	print_time_elapsed("Checking the actual size of the hash: ", &start, &end);
	exit(1);
}

int main(int argc, const char* argv[]) {

	ezOptionParser opt;
	opt.overview = "Test dynamically-updateable index.";
	opt.syntax = "main [OPTIONS]";

	opt.add("", 0, 0, 0, "Help!", "-h", "-help", "--help", "--usage");

	opt.add("", 0, 1, 0, "The genome file.", "-g", "--genome");

	opt.add("", 0, 1, 0, "The edit file.", "-e", "--editsFile");
	opt.add("", 0, 1, 0, "The number of edits to perform.", "-n", "--numEdits");

	opt.add("", 0, 1, 0, "The edit file which contains both edits and queries (for benchmarking search).", "-q", "--editsQueriesFile");
	opt.add("", 0, 1, 0, "The query frequency (to be used only when benchmarking search).", "-qf", "--queryFrequency");
	opt.add("", 0, 1, 0, "The query count (to be used only when benchmarking search).", "-qc", "--queryCount");
	opt.add("", 0, 1, 0, "The number of iterations of edits and searches to perform (to be used only when benchmarking search).", "-it", "--iterations");

	opt.add("", 0, 1, 0, "The substrings file.", "-s", "--substrFile");

	opt.add("", 0, 1, 0, "Directory for writing logs", "-l", "--logPath");
	opt.add("", 0, 1, 0, "File path for writing the updated genome", "-o", "--output");
	opt.add("", 0, 1, 0, "File path for writing the output queries or substrings", "-r", "--resultsPath");

	opt.add("", 0, 1, 0, "Run all the tests", "-t", "--runTests");
	opt.add("", 0, 1, 0, "Path to save the hash", "-sh", "--saveHashPath");
	opt.add("", 0, 1, 0, "Path from where to load the hash", "-lh", "--loadHashPath");

	opt.parse(argc, argv);

	//If the log file path is specified as an input parameter, use it, or use the default location.
	std::string logPath = "../logs/SP.log";
	if (opt.isSet("--logPath")) {
		opt.get("--logPath")->getString(logPath);
		format_path(logPath);
		logPath+="SP.log";
	}

	auto file_logger = spdlog::daily_logger_mt(FILE_LOGGER, logPath, true);
	LOGINFO(FILE_LOGGER, "Happy Cows!");
	auto console_logger = spdlog::stdout_logger_mt("console");
	spd::set_level(spd::level::info); //level of logging
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v ");

	console_logger->info() << "Log path: " << logPath;

	if (opt.isSet("-h")) {
		std::string usage;
		opt.getUsage(usage, 80, ezOptionParser::ALIGN);
		std::cerr << usage;
		return 1;
	}

	std::string genomeFile;
	if (opt.isSet("--genome")) {
		opt.get("--genome")->getString(genomeFile);
	} else {
		console_logger->warn() << "You can't build an index without a genome!";
		return 1;
	}

	std::string editsFile = "";
	if (opt.isSet("--editsFile")) { opt.get("--editsFile")->getString(editsFile); }

	long numEdits = 0;
	if (opt.isSet("--numEdits")) { opt.get("--numEdits")->getLong(numEdits); }

	std::string substrFile = "";
	if (opt.isSet("--substrFile")) { opt.get("--substrFile")->getString(substrFile); }

	std::string editsQueriesFile = "";
	if (opt.isSet("--editsQueriesFile")) { opt.get("--editsQueriesFile")->getString(editsQueriesFile); }

	long queryFrequency = 0;
	if (opt.isSet("--queryFrequency")) { opt.get("--queryFrequency")->getLong(queryFrequency); }

	long queryCount = 0;
	if (opt.isSet("--queryCount")) { opt.get("--queryCount")->getLong(queryCount); }

	long iterations = 0;
	if (opt.isSet("--iterations")) { opt.get("--iterations")->getLong(iterations); }

	if (opt.isSet("--loadHashPath")) { opt.get("--loadHashPath")->getString(loadHashPath);}

	if (opt.isSet("--resultsPath")) { opt.get("--resultsPath")->getString(resultsPath); }
	else { resultsPath = logPath;}

	//checkHashSize(loadHashPath);

	genome g;
	g.get_input(genomeFile);


	if (opt.isSet("--runTests")) {

		LOGINFO(FILE_LOGGER, "Running tests..");
		if (opt.isSet("--editsFile") && !opt.isSet("--substrFile")) {
			LOGINFO(FILE_LOGGER, "Testing Naive Edits..");
			if (fileExists(editsFile)) {
				test_edits_naive(g, editsFile, numEdits);
			}
		}

		if (opt.isSet("--substrFile") && opt.isSet("--editsFile")) {
			LOGINFO(FILE_LOGGER, "Benchmarking substring extraction");
			if (fileExists(substrFile) && fileExists(editsFile)) {
				test_substr_naive(g, substrFile, editsFile, numEdits);
			}
		}

		if (opt.isSet("--editsQueriesFile") && opt.isSet("--queryFrequency") && opt.isSet("--queryCount") && opt.isSet("--iterations")) {
			if (queryFrequency > 0 && queryCount > 0 && iterations > 0) {
				test_search_naive(g, editsQueriesFile, queryFrequency, queryCount, iterations);
			} else {
				LOGINFO(FILE_LOGGER,
						"There was a problem with one or more of the parameters provided for benchmarking search.");
			}
		}

		//TODO: test substring

		LOGINFO(FILE_LOGGER, "Completed all tests successfully!");
		LOGINFO(FILE_LOGGER, "Quitting... Bye!");
		exit(0);
	}

	if (opt.isSet("--editsFile") && !opt.isSet("--substrFile")) {
		LOGINFO(FILE_LOGGER, "Benchmarking edits");
		if (fileExists(editsFile)) {
			benchmark_construction(g);
			benchmark_edits(g, editsFile, numEdits);
		}
	}

	if (opt.isSet("--substrFile") && opt.isSet("--editsFile")) {
		LOGINFO(FILE_LOGGER, "Benchmarking substring extraction");
		if (fileExists(substrFile) && fileExists(editsFile)) {
			benchmark_construction(g);
			benchmark_substring(g, substrFile, editsFile, numEdits);
		}
	}

	if(opt.isSet("--editsQueriesFile") && opt.isSet("--queryFrequency") && opt.isSet("--queryCount") && opt.isSet("--iterations") ) {
		if(queryFrequency>0 && queryCount>0 && iterations>0 ) {
			LOGINFO(FILE_LOGGER, "Benchmarking search");
			if (fileExists(editsQueriesFile)) {
				benchmark_construction(g);
				benchmark_search(g, editsQueriesFile, queryFrequency, queryCount, iterations);
			}
		} else {
			LOGINFO(FILE_LOGGER,
					"There was a problem with one or more of the parameters provided for benchmarking search.");
		}
	}

	string outputPath;
	if (opt.isSet("--output")) {
		LOGINFO(FILE_LOGGER, "Writing the updated genome to disk.");
		opt.get("--output")->getString(outputPath);
		format_path(outputPath);
		outputPath += "SPGenome.fa";
		ofstream myfile;
		myfile.open(outputPath);
		myfile << g.read_reference_at(0, 0, LONG_MAX);
		myfile.close();
	}

   string saveHashPath = "";
	if (opt.isSet("--saveHashPath")) {
		opt.get("--saveHashPath")->getString(saveHashPath);
		g.save_hash(saveHashPath);
	}

	return 0;

}
