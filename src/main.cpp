#include <memory>

#include "genome.h"
#include "utils.h"
#include "test.h"
#include "benchmark.h"
#include "ezOptionParser.hpp"
#include "spdlog/spdlog.h"

using namespace ez;
using namespace std;

void wabi_example(){

	genome example;
	example.set_reference("AGCTTTTCATTCTGA");
	example.construct_hash();
	example.display_hash();

	example.snp_at(4,"G");
	example.display_hash();

	example.insert_at("TGAA",7);
	example.display_hash();
	example.get_skip_list().print_list();
	example.get_skip_list().print_base_level();
	example.display_updated_genome();

	example.insert_at("AT",3);
	example.insert_at("TAC",16);
	example.display_hash();
	example.get_skip_list().print_list();
	example.get_skip_list().print_base_level();
	example.display_updated_genome();

}

int main(int argc, const char* argv[]){
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
	opt.add( "", 0, 0, 0,
             "Help!",
             "-h", "-help", "--help", "--usage");
	opt.add( "", 0, 0, 0,
             "Build the hash.",
             "-b", "--build");
	opt.add( "", 0, 1, 0,
             "The genome file.",
             "-g", "--genome");
	opt.add( "", 0, 1, 0,
             "The edit file.",
             "-e", "--edits");
	opt.add( "", 0, 1, 0,
             "The number of edits to perform.",
             "-n", "--numEdits");
	opt.add( "", 0, 1, 0,
             "The substrings file.",
             "-s", "--substr");

    opt.parse(argc, argv);

    if (opt.isSet("-h") ) {
        std::string usage;
        opt.getUsage(usage,80,ezOptionParser::ALIGN);
        std::cerr << usage;
        return 1;
    }

    genome g;

    std::string genomeFile;
    //if (opt.isSet("--build")) {
        if (opt.isSet("--genome")) {
            opt.get("--genome")->getString(genomeFile);
        } else {
            stderrLogger->warn("You can't build an index without a genome!");
            return 1;
        }
    //}

	g.get_input(genomeFile);

    std::string editFile;
    if (opt.isSet("--edits")) {
        opt.get("--edits")->getString(editFile);
    }

    long numEdits;
    if (opt.isSet("--numEdits")) {
        opt.get("--numEdits")->getLong(numEdits);
    }
	
	std::string substrFile;
    if (opt.isSet("--substr")) {
        opt.get("--substr")->getString(substrFile);
    }
	
	benchmark(g,editFile,numEdits);
	benchmark_substring(g,substrFile);
	//benchmark_search(g, editFile);

   	/*
	if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " input_file_name" << " K" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
	*/

    // ROB edit --- are we constructing the hash twice?
	//g.construct_hash();

    //wabi_example();

	//test();
	//benchmark(g, argv[2], argv[3], atol(argv[4]));
	//benchmark_search(g,argv[2]);
	return 0;
}
