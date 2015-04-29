#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>

int main(int argc, char* argv[]){

	if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " input_file_name" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
	
	std::string reference; //original reference sequence
    
    //Read the file line by line
	std::ifstream infile(filename);
	std::string input;
    //getline(infile,input); //Skip the first line for a FASTA file (.fa, .fna)
    while(getline(infile, input)){
	    if(std::find(input.begin(), input.end(), '>')==input.end())
	    	reference+=input;
        input.clear();
    }
    infile.close();
	
	reference.erase(std::remove(reference.begin(), reference.end(), 'N'), reference.end()); //removing all the D-s for a fair benchmark

	std::ofstream outfile(filename);
	outfile << reference << std::endl;
	outfile.close();
}
