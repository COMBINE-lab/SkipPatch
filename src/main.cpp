#include <memory>

#include "genome.h"
#include "utils.h"
#include "test.h"
#include "benchmark.h"
#include "ezOptionParser.hpp"
#include "spdlog/spdlog.h"

//#define TEST 1

using namespace ez;
using namespace std;

void wabi_example() {

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

int main(int argc, const char* argv[]) {

	ezOptionParser opt;
	opt.overview = "Test dynamically-updateable index.";
	opt.syntax = "main [OPTIONS]";

	opt.add("", 0, 0, 0, "Help!", "-h", "-help", "--help", "--usage");

	opt.add("", 0, 1, 0, "The genome file.", "-g", "--genome");

	//TODO: The value of K isn't used yet.
	opt.add("", 0, 1, 0, "Value of K.", "-k", "--k");

	opt.add("", 0, 1, 0, "Type of benchmark to perform: edit/substr/search ", "-t", "--benchmarkType");

	opt.add("", 0, 1, 0, "The edit file.", "-e", "--editsFile");
	opt.add("", 0, 1, 0, "The number of edits to perform.", "-n", "--numEdits");

	opt.add("", 0, 1, 0, "The edit file which contains both edits and queries (for benchmarking search).", "-q", "--editsQueriesFile");
	opt.add("", 0, 1, 0, "The query frequency (to be used only when benchmarking search).", "-qf", "--queryFrequency");
	opt.add("", 0, 1, 0, "The query count (to be used only when benchmarking search).", "-qc", "--queryCount");
	opt.add("", 0, 1, 0, "The number of iterations of edits and searches to perform (to be used only when benchmarking search).", "-it", "--iterations");

	opt.add("", 0, 1, 0, "The substrings file.", "-s", "--substrFile");

	opt.add("", 0, 1, 0, "File path for logs", "-l", "--logFile");

	opt.parse(argc, argv);

	//If the log file path is specified as an input parameter, use it, or use the default location.
	std::string logFile = "../logs/skippatch.log";
	if (opt.isSet("--logFile")) {
		opt.get("--logFile")->getString(logFile);
	}

	auto file_logger = spdlog::daily_logger_mt(FILE_LOGGER, logFile);
	auto console_logger = spdlog::stdout_logger_mt("console");
	spd::set_level(spd::level::info); //level of logging
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v ");

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

	int k;
	if (opt.isSet("--k")) {
		opt.get("--k")->getInt(k);
	}

	std::string benchmarkType;
	if (opt.isSet("--benchmarkType")) {
		opt.get("--benchmarkType")->getString(benchmarkType);
	} else {
		console_logger->warn() << "Type of benchmark to be performed has not been specified!";
	}

	std::string editsFile;
	if (opt.isSet("--editsFile")) {
		opt.get("--editsFile")->getString(editsFile);
	}

	long numEdits;
	if (opt.isSet("--numEdits")) {
		opt.get("--numEdits")->getLong(numEdits);
	}


	std::string substrFile;
	if (opt.isSet("--substrFile")) {
		opt.get("--substrFile")->getString(substrFile);
	}

	long queryFrequency;
	if (opt.isSet("--queryFrequency")) {
		opt.get("--queryFrequency")->getLong(queryFrequency);
	}

	long queryCount;
	if (opt.isSet("--queryCount")) {
		opt.get("--queryCount")->getLong(queryCount);
	}

	long iterations;
	if (opt.isSet("--iterations")) {
		opt.get("--iterations")->getLong(iterations);
	}

	console_logger->info() << "Log file: " << logFile;

	genome g;
	g.get_input(genomeFile);

//	test();


	if (benchmarkType == "edit") {
		LOGINFO(FILE_LOGGER, "Benchmarking edits");
		benchmark_edits(g, editsFile, numEdits);
	}
	if (benchmarkType == "substr") {
		LOGINFO(FILE_LOGGER, "Benchmarking substring extraction");
		benchmark_substring(g, substrFile);
	}
	if (benchmarkType == "search") {
		LOGINFO(FILE_LOGGER, "Benchmarking search");
		benchmark_search(g, editsFile, queryFrequency, queryCount, iterations);
	}

   string outputFile;
   if (opt.isSet("--output")) {
		opt.get("--output")->getString(outputFile);
		ofstream myfile;
		myfile.open (outputFile);
		//myfile << "Writing this to a file.\n";
		myfile <<g.read_reference_at(0,0,g.get_length());
		myfile.close();
	}

	return 0;

}
