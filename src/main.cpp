#include <memory>

#include "genome.h"
#include "utils.h"
#include "test.h"
#include "benchmark.h"
#include "ezOptionParser.hpp"
#include "spdlog/spdlog.h"

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

	namespace spd = spdlog;

	auto stderrLogger = spd::stdout_logger_mt("console");
	//auto fileSink = std::make_shared<spd::sinks::simple_file_sink_mt>(logFileName, true);
	//auto consoleSink = std::make_shared<spd::sinks::stderr_sink_mt>();
	//auto consoleLog = spdlog::create("consoleLog", {consoleSink});
	//auto fileLog = spdlog::create("fileLog", {fileSink});
	//auto jointLog = spdlog::create("jointLog", {fileSink, consoleSink});

	opt.overview = "Test dynamically-updateable index.";
	opt.syntax = "main [OPTIONS]";
	//opt.footer = "v0.1.4 Copyright (C) 2011 Remik Ziemlinski\nThis program is free and without warranty.\n";

	opt.add("", 0, 0, 0, "Help!", "-h", "-help", "--help", "--usage");

	//TODO: Do we need the build option? Don't we *have* to build the hash?
	opt.add("", 0, 0, 0, "Build the hash.", "-b", "--build");
	opt.add("", 0, 1, 0, "The genome file.", "-g", "--genome");

	opt.add("", 0, 1, 0, "The edit file.", "-e", "--editsFile");
	opt.add("", 0, 1, 0, "The number of edits to perform.", "-n", "--numEdits");

	opt.add("", 0, 1, 0, "The edit file which contains both edits and queries (for benchmarking search).", "-q", "--editsQueriesFile");
	opt.add("", 0, 1, 0, "The query frequency (to be used only when benchmarking search).", "-qf", "--queryFrequency");
	opt.add("", 0, 1, 0, "The query count (to be used only when benchmarking search).", "-qc", "--queryCount");
	opt.add("", 0, 1, 0, "The number of iterations of edits and searches to perform (to be used only when benchmarking search).", "-it", "--iterations");

	opt.add("", 0, 1, 0, "The substrings file.", "-s", "--substrFile");

	opt.parse(argc, argv);

	if (opt.isSet("-h")) {
		std::string usage;
		opt.getUsage(usage, 80, ezOptionParser::ALIGN);
		std::cerr << usage;
		return 1;
	}

	genome g;

	std::string genomeFile;

	if (opt.isSet("--genome")) {
		opt.get("--genome")->getString(genomeFile);
	} else {
		stderrLogger->warn() << "You can't build an index without a genome!";
		return 1;
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

	g.get_input(genomeFile);

	//test();
	//benchmark_edits(g,editFile,numEdits);
	//benchmark_substring(g,substrFile);
	//benchmark_search(g, editsFile, queryFrequency, queryCount, iterations);

	return 0;
}
