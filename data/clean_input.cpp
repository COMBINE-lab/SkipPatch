#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>

void remove_repeat_mask(std::string &reference){

	reference.erase(std::remove(reference.begin(), reference.end(), 'D'), reference.end()); //removing all the D-s for a fair benchmark
	//remember the difference b/w erase and remove!
}

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
    getline(infile,input); //Skip the first line for a FASTA file (.fa, .fna)
    while(getline(infile, input)){
        reference+=input;
        input.clear();
    }
    infile.close();
	
	remove_repeat_mask(reference);

	std::ofstream outfile(filename);
	outfile << reference << std::endl;
	outfile.close();
}
