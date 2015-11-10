#include <algorithm>
#include <fstream>

#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/string.hpp"

#include "genome.h"
#include "utils.h"
#include "spdlog/spdlog.h"

using namespace std;

/*
 unsigned long hashing_func(std::string key)
 {
 return XXH64(key.c_str(), key.length(), 0);
 }

 genome::genome() : m(10000000, hashing_func) {}
 */

genome::genome() {
}

void genome::get_input(string path) {

	if (fileExists(path)) {

		ifstream myfile(path);
		if (myfile.is_open()) {

			std::string input;
			while (getline(myfile, input)) {
				reference += input;
				input.clear();
			}
			LOGDEBUG(FILE_LOGGER,"Input taken!!");

			ins = std::vector<bool>(get_length(), false);
			del = std::vector<bool>(get_length(), false);
		} else {
			LOGWARN(FILE_LOGGER,"File is not open!");
			LOGWARN(FILE_LOGGER,"Quitting.. Bye!");
			exit(-1);
		}
	}
}

//used only in tests
void genome::set_reference(std::string input) {
	reference = input;
	ins = std::vector<bool>(get_length(), false);
	del = std::vector<bool>(get_length(), false);
}

std::string genome::get_reference() {
	return reference;
}

long genome::get_length() {
	return reference.length();
}

std::unordered_map<uint64_t, std::vector<long>>& genome::get_hash() {
	return m;
}

skip_list genome::get_skip_list() {
	return s;
}

void genome::load_hash(const std::string& fname) {
	std::ifstream is(fname);
	cereal::BinaryInputArchive iarchive(is);
	iarchive(m);
}

void genome::save_hash(const std::string& fname) {
	std::ofstream is(fname);
	cereal::BinaryOutputArchive oarchive(is);
	oarchive(m);
}

void genome::construct_hash() {
	for (auto it = reference.begin(); it <= reference.end() - K; it+=S) {
		std::string temp(it, it + K);
		if (!(temp.find('n') != std::string::npos) && !(temp.find('N') != std::string::npos)){
            if(std::distance(reference.begin(),it)%10000000 == 0){
                LOGINFO(FILE_LOGGER, std::to_string(std::distance(reference.begin(),it))+" k mers inserted");
            }
			m[str_to_int(temp)].push_back(it - reference.begin());
	}
    }
}

float genome::get_load_factor() {
	return m.load_factor();
}

void genome::display_genome() {
	std::cout << reference << std::endl;
}

void genome::display_updated_genome() {
	std::cout << read_reference_at(0,0,get_length()) << std::endl;
}

void genome::display_hash() {
	std::cout << std::endl << "Hash Table: " << std::endl;
	for (auto it = m.begin(); it != m.end(); it++) {
		std::cout << int_to_str(it->first) << "\t";
		for (auto vector_it = (it->second).begin();
				vector_it != (it->second).end(); vector_it++) {
			std::cout << *vector_it << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void genome::display_load() {
	LOGINFO(FILE_LOGGER, "Size: " + std::to_string(m.size()));
	LOGINFO(FILE_LOGGER, "Bucket Count:  " + std::to_string(m.bucket_count()));
	LOGINFO(FILE_LOGGER, "Load Factor: " + std::to_string(m.load_factor()));
}

void genome::remove_kmer_from_hash_at(long position_to_remove, std::string kmer) {

	uint64_t curr_kmer = str_to_int(kmer);

	if (!m[curr_kmer].empty()) {
		if (std::find(m[curr_kmer].begin(), m[curr_kmer].end(), position_to_remove) != m[curr_kmer].end()) {
			LOGDEBUG(FILE_LOGGER, "Removed " + kmer + " from position " + std::to_string(position_to_remove));
			m[curr_kmer].erase(std::find(m[curr_kmer].begin(), m[curr_kmer].end(), position_to_remove));
		}
	}
	//Having a very long "vector/list" of positions (~1 million) causes a bottleneck here
	//Approximately takes 0.5 seconds to execute this line "once" if the length of "positions" is 1 million

	if (m[curr_kmer].size() == 0) {
		m.erase(curr_kmer);
	}
}

void genome::add_kmer_to_hash_at(long position, std::string new_kmer) {
	m[str_to_int(new_kmer)].push_back(position);
	LOGDEBUG(FILE_LOGGER, "Added " + new_kmer + " from position " + std::to_string(position));
}

long genome::get_genome_position_from_virtual_position(long virtual_position) {
	node *n;
	long genome_position; //genome_position is position in the 'original genome' corresponding to the virtual_position in the 'virtual genome'
	unsigned long offset = 0;
	s.get_prev_node(virtual_position, genome_position, offset, &n);

	return genome_position;
}

void genome::get_genome_position_from_virtual_position(long virtual_position,
		long &genome_position, unsigned long &offset, node **n) {
	s.get_prev_node(virtual_position, genome_position, offset, n);

}

long genome::get_virtual_position_from_genome_position(long genome_position, long offset) { //argument must be a tuple

	return genome_position + s.get_cumulative_count(genome_position) + offset;
}

void genome::snp_at(const long snp_pos_abs, const std::string snp) {

	LOGDEBUG(FILE_LOGGER,"snp_at: " + std::to_string(snp_pos_abs) + " " + snp);
	const long snp_len = snp.length();
	auto kmer_pos_pair = get_kmers(snp_pos_abs - K + 1, snp_len + K - 1 + S);
	int i = 0;

	long genome_position;

	//Edge cases: Use min, max to adjust these values for ends of genome
	std::string snp_update = read_reference_abs_at(snp_pos_abs - K + 1, 2 * K - 2 + snp_len + S, genome_position);
	snp_update.replace(K - 1, snp_len, snp);

	int sample = 0;
	for (auto it : kmer_pos_pair) {
		remove_kmer_from_hash_at(it.second, it.first);
		string new_kmer = snp_update.substr(i, K);
		if (it.first.length() == K) {
			if (sample % S == 0) { //Sampling
				add_kmer_to_hash_at(it.second, new_kmer);
			}
		}
		sample++;
		i++;
	}

	//Anchor the edit by hashing the k-mer occurring right after the SNP location
	auto anchor = get_kmers(snp_pos_abs + K, 1).front();
	LOGDEBUG(FILE_LOGGER, "Anchor: " + anchor.first + " at " + std::to_string(anchor.second));
	if(!is_kmer_hashed(anchor.first, anchor.second)){
		add_kmer_to_hash_at(anchor.second, anchor.first);
	}

	//Update the genome itself
	//At every SNP, investigate if it is inside an inserted segment
	//This should also take care of the cases where a series of SNPs could span several insertions
	unsigned long offset;
	node* n;
	int j = 0;
	for (int j = 0; j < snp_len; j++) {
		get_genome_position_from_virtual_position(snp_pos_abs + j,
				genome_position, offset, &n);
		if (ins[genome_position]) {
			LOGDEBUG(FILE_LOGGER,"I: " + n->next->str + " " + std::to_string(n->next->val) + " " + std::to_string(n->next->offset));
			if (offset == 0) {
				reference[genome_position] = snp[j];
				LOGDEBUG(FILE_LOGGER,"I: Changing " + std::string(1,reference[genome_position]) + " to " + snp[j]);
			} else {
				n->next->str = (n->next->str).replace(offset - 1, 1, string(1, snp[j])); //Doubt??
				LOGDEBUG(FILE_LOGGER,"I: Changing " + std::string(1,n->str[offset-1]) + " to " + snp[j]);
			}
		} else {
			LOGDEBUG(FILE_LOGGER,"G: Changing " + std::string(1,reference[genome_position]) + " to " + snp[j]);
			reference[genome_position] = snp[j]; //No insertion at that point
		}
	}
}

/**
 * Searches for the read in the reference sequence and reports all the positions found.
 *
 * Uses a 'seed and vote' algorithm to search for the read.
 * The algorithm breaks up the read into subreads and tries to find the positions on the genome
 * where all the subreads agree to map to.
 * The algorithm first finds all the potential locations all the subreads could map to.
 * All the potential locations are then grouped into potential sets of positions
 * which exploit the following properties:
 *  - Every group of positions will have nondecreasing subread indices
 *  - A group can consist of  a minimum [read.length()-(S-1 + K-1))/S] positions
 *  	because the edit algorithms ensure that no two consecutive sampled kmers will be more than S apart,
 *  	where S is the sampling factor.
 *  - No two subreads within a group can be more than S apart wrt to the updated reference.
 *
 * This function handles sampling, and hence replaces the earlier search function.
 */
std::vector<long> genome::search(std::string read) {

	std::vector<long> mapped_positions;
	if (read.length()==0) {
		return mapped_positions;
	} else if (read.length() < K + S){
		LOGALERT(FILE_LOGGER,"A read of length less than K+S cannot be searched for an exact match");
		exit(-1);
	}

	struct timeval start, a, b, c, d, e, end;
	struct timezone tzp;
	gettimeofday(&start, &tzp);

	std::vector<std::string> subreads;
	//STEP 1: Shred the read into overlapping subreads of length K
	for (int i = 0; i < read.length() - K + 1; i++) {
		subreads.push_back(read.substr(i, K));
	}

	//STEP 2: Find all the positions of all the kmers and merge them into a list
	//Every position is stored along with the index of the subread it is derived from
	std::vector<std::pair<long,int>> positions;
	int read_index = 0;
	for (std::string subread : subreads) {
		//LOGDEBUG(FILE_LOGGER, "Subread: " + subread);
		auto f = m.find(str_to_int(subread));
		if (f != m.end()) {
			for (long pos : f->second) {
				positions.push_back(std::make_pair(pos,read_index));
				//LOGDEBUG(FILE_LOGGER, "Pos: " + std::to_string(pos) + " " + std::to_string(read_index));
			}
		}
		read_index++;
	}

	//Sort the list of positions. This is automatically sorted by first, the position, then index
	std::sort(positions.begin(), positions.end());

	//STEP 3: Now run through the list of positions and find possible groups
	//In a valid group, no two kmers would be more than S apart, and the group size would at least be (read.length()-K)/S
	std::vector<std::vector<std::pair<long,int>>> groups;
	std::vector<std::pair<long,int>> group;

	for (auto curr = positions.begin(); curr != positions.end(); curr++) {

		auto next = std::next(curr);

		if (group.size() == 0) {
			group.push_back(*curr); //If the group is empty, add the current position to it
		}

		if (next->second < curr->second || next == positions.end()) {
			if (group.size() >= (read.length() - K + 1) / S) {
				std::string group_elements = "";
				for (auto g : group) {
					group_elements += std::to_string(g.first);
					group_elements += " ";
				}
				LOGDEBUG(FILE_LOGGER, "Valid Group Found(size:" + std::to_string(group.size()) + "): " + group_elements);
				groups.push_back(group); //If the group is valid, keep it
			} else {
				LOGDEBUG(FILE_LOGGER, "Invalid Group Found. ");
			}
			LOGDEBUG(FILE_LOGGER, "Clearing group.");
			group.clear(); //The group was either a valid group, or not. Irrespective, clear it.
		}

		if (next->second > curr->second) {
			group.push_back(*next);
		}

	}

	gettimeofday(&d, &tzp);
	//STEP 4: Translate the genome positions into current positions,
	//read the subsequence to ensure that the positions found were valid
	for (auto group : groups) {

		//Find the exact starting position wrt the current genome
		auto lpos = group.front();
		long loffset = 0;
		long start = 0;
		long updated_lpos = get_virtual_position_from_genome_position(lpos.first, loffset); //assuming an offset 0
		if (ins[lpos.first]) { //If the position is an insertion, find the actual offset
			long ins_len = s.find(lpos.first)->str.length();
			std::string subread_ext = read_reference_at(lpos.first, 0, subreads[lpos.second].length()+ins_len);
			loffset = subread_ext.find(subreads[lpos.second]);
			start = get_virtual_position_from_genome_position(lpos.first, loffset);
			start -= lpos.second;
		} else {
			start = updated_lpos-lpos.second;
		}

		//Find the exact end position wrt the current genome
		auto rpos = group.back();
		long roffset = 0;
		long end = 0;
		long updated_rpos = get_virtual_position_from_genome_position(rpos.first, roffset); //assuming an offset 0
		if (ins[rpos.first]) { //If the position is an insertion, find the actual offset
			long ins_len = s.find(rpos.first)->str.length();
			std::string subread_ext = read_reference_at(rpos.first, 0, subreads[rpos.second].length()+ins_len);
			roffset = subread_ext.find(subreads[rpos.second]);
			end = get_virtual_position_from_genome_position(rpos.first, roffset) + K + read.length() - K - rpos.second;
		} else {
			end = updated_rpos + K + read.length() - K - rpos.second;
		}

		long temp = 0;
		//So, it is straightforward if the end-start <2*read.length()
		//This should be happenning in 99% of the cases
		if (end - start <= 2 * read.length()) {
			//Read off the length of the read starting from start to end and check if the read matches
			std::string read_ext = read_reference_abs_at(start, end - start, temp);
			long pos = read_ext.find(read);
			while (pos != std::string::npos) {
				LOGDEBUG(FILE_LOGGER, "FOUND: " + std::to_string(pos));
				mapped_positions.push_back(start + pos);
				pos = read_ext.find(read, pos + 1);
			}
		} else {
			//If it isn't, then the group possibly contains some erroneous positions. Eliminate them.
			//Translate all the positions to absolute positions
			/*
			LOGINFO(FILE_LOGGER, "\nCorner Case!");
			LOGINFO(FILE_LOGGER, "START " + std::to_string(start) + " END " + std::to_string(end) + " --> " + std::to_string(end-start));
			LOGINFO(FILE_LOGGER, "READ: " + read);
			LOGINFO(FILE_LOGGER, "Positions: " + std::to_string(positions.size()));
			LOGINFO(FILE_LOGGER, "Group Size: " + std::to_string(group.size()));
			*/
			std::vector<long> group_abs_positions;
			for (auto pos : group) {
				long offset = 0;
				long abs_pos = 0;
				long updated_pos = get_virtual_position_from_genome_position(pos.first, offset); //assuming an offset 0
				if (ins[pos.first]) { //If the position is an insertion, find the actual offset
					long ins_len = s.find(pos.first)->str.length();
					std::string subread_ext = read_reference_at(pos.first, 0, subreads[pos.second].length()+ins_len);
					offset = subread_ext.find(subreads[pos.second]);
					abs_pos = get_virtual_position_from_genome_position(pos.first, offset);
				} else {
					abs_pos = updated_pos;
				}
				group_abs_positions.push_back(abs_pos);
				//LOGINFO(FILE_LOGGER, "abs_pos  " + std::to_string(pos.first) + " " + std::to_string(abs_pos));
				std::string read_ext = read_reference_abs_at(abs_pos, read.length(), temp);
				if (read == read_ext) {
					long fpos = abs_pos - pos.second;
					if(std::find(mapped_positions.begin(), mapped_positions.end(), fpos)!=mapped_positions.end()){
						mapped_positions.push_back(fpos);
					}
				}
			}
		}
	}

	gettimeofday(&end, &tzp);
	float ex = get_time_elapsed(&d, &end);

	gettimeofday(&end, &tzp);
	float tx = get_time_elapsed(&start, &end);

	if (tx > 0.01) {
		LOGINFO(FILE_LOGGER, "READ: " + read);
		LOGINFO(FILE_LOGGER, "Positions: " + std::to_string(positions.size()));
		LOGINFO(FILE_LOGGER, "Groups: " + std::to_string(groups.size()));
		LOGINFO(FILE_LOGGER, "E: " + std::to_string(ex));
		print_time_elapsed("TOTAL: ", &start, &end);
	}

	return mapped_positions;
}

/*
 * reads the reference from the specified genome(original) position and offset (in the skip list)
 * up to the specified length or until it reaches the end of the genome.
 * TODO: inefficient
 *
 */
string genome::read_reference_at(const long genome_position, const long offset,
		const long len) {

	long rem_len = len, curr_genome_pos = genome_position, curr_offset = offset;
	string kmer;

	while (rem_len > 0 && curr_genome_pos <= get_length() - 1) {
		if (ins[curr_genome_pos]) {

			if (!curr_offset) {
				kmer += reference[curr_genome_pos];
				rem_len--;
				curr_offset = 1;
			} else {
				//s.print_list();
				//cout<<"\nCurr genome pos: "<<curr_genome_pos<<endl;
				node *n = s.find(curr_genome_pos);
				if(!n){
					LOGWARN(FILE_LOGGER,"n is null! at genome pos: "+to_string(curr_genome_pos)+" offset is: "+to_string(curr_offset));
				}
				string temp(n->str, curr_offset - 1, rem_len);
				kmer += temp;
				rem_len -= temp.length();

				if (!rem_len)
					break;
				curr_genome_pos++;
				curr_offset = 0;
			}
		} else if (del[curr_genome_pos]) {
			curr_genome_pos++;
		} else {
			kmer += reference[curr_genome_pos];
			rem_len--;
			curr_genome_pos++;
			curr_offset = 0;
		}
	}

	return kmer;
}


/**
 * A more efficient read_reference
 */
/*
std::string genome::read_reference_at(const long gpos, const long offset, const long len) {

    node *n;
    long absolute_position = get_virtual_position_from_genome_position(gpos, offset);
    unsigned long initial_offset = offset;
    long genome_position=gpos;
    s.get_prev_node(absolute_position, genome_position, initial_offset, &n);

    std::string ref = "";

    long rem_len = len;
    long curr_genome_pos = genome_position;
    while (rem_len > 0 && curr_genome_pos <= reference.length() && n) {

        if (n->val == curr_genome_pos) {
            if (n->offset >= 0) {
                std::string r = "";
                r += reference[curr_genome_pos];
                r += n->str;
                ref += r.substr(initial_offset, rem_len);
                initial_offset = 0;
                rem_len -= r.length();
                curr_genome_pos++;
            } else if (n->offset < 0) {
                curr_genome_pos += (-1) * n->offset;
            }
        }

        if (n->val != curr_genome_pos) {
            long ref_len = reference.length();
            long next = ref_len;
            if (n->next) {
                next = min(n->next->val, ref_len);
            }
            long readoff_len = min(rem_len, next - curr_genome_pos);
            ref += reference.substr(curr_genome_pos, readoff_len);
            curr_genome_pos+=readoff_len;
            rem_len -= readoff_len;
        }

        n = n->next;
    }

    return ref;

}*/

string genome::read_reference_abs_at(const long abs_pos, const long len,
		long &genome_position) {


	unsigned long offset;
	node* n;
	string kmer;
	get_genome_position_from_virtual_position(abs_pos, genome_position, offset, &n);
	kmer = read_reference_at(genome_position, offset, len);
	return kmer;
}

void genome::read_reference_abs_at(const long abs_pos, const long len, long &genome_position, unsigned long &offset, string &kmer) {

	node* n;
	get_genome_position_from_virtual_position(abs_pos, genome_position, offset, &n);
	kmer = read_reference_at(genome_position, offset, len);
}

/**
 * Gets "kmer_count" number of k-mers from a "start" position on the updated genome.
 * Returns k_mers along with the positions it has been hashed at wrt the original reference.
 *
 * TODO: Handle edge cases, inefficient, reads the refernce everytime
 */
vector<pair<string, long>> genome::get_kmers(const long start, const unsigned long kmer_count) {

	vector<pair<string, long> > vec;
	for (long i = start; i < start + kmer_count; i++) {//could be faster

		long genome_position;
		string kmer = read_reference_abs_at(i, K, genome_position);
		vec.push_back(make_pair(kmer, genome_position));
	}
	return vec;
}

/**
 * Get "kmer_count" k-mers from a "start" position on the updated genome,
 * including the offset at which the k-mer occurs.
 *
 * TODO: Handle edge cases, inefficient
 */
vector<tuple<string, long, unsigned long>> genome::get_kmers_with_offset(
		const long start, const unsigned long kmer_count) {

	vector<tuple<string, long, unsigned long> > vec;
	for (long i = start; i < start + kmer_count; i++) //could be faster
			{
		long genome_position;
		unsigned long offset;
		string kmer;
		read_reference_abs_at(i, K, genome_position, offset, kmer);
		vec.push_back(make_tuple(kmer, genome_position, offset));
	}
	return vec;
}

/**
 * Inserts a string at the 'insert_pos'.
 * Updates the k-mers which lie in the "region of change"
 * Splits the problem into two segments:
 * - Updating the k-mers which begin at a point before the insertion
 * - Adding the new k-mers which are a result of the insertion (begin at a point in the inserted segment)
 *
 * Sampling ensures that no two k-mers which are hashed are more than 'S' apart on the current genome
 * The insert is 'anchored' by hashing the kmer occurring immediately after the insert
 */
void genome::insert_at(const std::string insertion, const unsigned long insert_pos_abs) {

	LOGDEBUG(FILE_LOGGER,"insert_at: " + insertion + " at "  + std::to_string(insert_pos_abs));

	const long ins_len = insertion.length();

	auto kmer_pos_pair = get_kmers(insert_pos_abs - K + 2, K - 1);
	int i = 0;

	long genome_position;
	//The k length substring of the genome which begins from just after the genome
	const string end_kmer = read_reference_abs_at(insert_pos_abs + 1, K - 1, genome_position);
	string ins_copy = insertion + end_kmer;

	LOGDEBUG(FILE_LOGGER, std::to_string(insert_pos_abs) + " " );

	int sample = 0;
	for (auto it : kmer_pos_pair) {
		remove_kmer_from_hash_at(it.second, it.first);
		string new_kmer = it.first.substr(0, K - i - 1) + ins_copy.substr(0, i + 1);
		if (it.first.length() == K) { //For handling edge cases: (insertion at the end of the genome)
			if (sample % S == 0) { //Sampling
				add_kmer_to_hash_at(it.second, new_kmer);
				LOGDEBUG(FILE_LOGGER, "Replaced " + it.first + " with " + new_kmer +  " at " + std::to_string(it.second));
			}
		}
		sample++;
		i++;
	}

	long add_pos = genome_position;
	if (!kmer_pos_pair.empty()) { //For handling edge cases: (insertion at the beginning of the genome)
		add_pos = kmer_pos_pair.back().second;
	}

	sample = 0;
	for (i = 0; i < insertion.length(); i++) {
		string temp = insertion.substr(i, K);
		string temp2 = end_kmer.substr(0, K - temp.length());
		if ((temp + temp2).length() == K) { //For handling edge cases: (insertion at the end of the genome)
			if (sample % S == 0) { //Sampling
				add_kmer_to_hash_at(add_pos, temp + temp2);
				LOGDEBUG(FILE_LOGGER, "Added new kmer " + temp + temp2 + " at " + std::to_string(add_pos));
			}
		}
		sample++;
	}

	//Sampling - anchoring the insertion by hashing the kmer appearing right after the insertion
	auto anchor = get_kmers(insert_pos_abs + 1, 1).front();
	LOGDEBUG(FILE_LOGGER, "Anchor: " + anchor.first);
	if(!is_kmer_hashed(anchor.first, anchor.second)){
		add_kmer_to_hash_at(anchor.second, anchor.first);
		LOGDEBUG(FILE_LOGGER, "Adding anchor kmer " + anchor.first + " at " + std::to_string(anchor.second));
	}

	//Set the ins bit to true
	ins[add_pos] = 1;

	//Update the skip list
	s.insert_and_update_abs(insert_pos_abs, insertion);

}

/**
 * Checks if the kmer was hashed at the specified genome position
 */
bool genome::is_kmer_hashed(std::string kmer, long position) {

	auto f = m.find(str_to_int(kmer)); // Search for the kmer in the hash
	if (f != m.end()) { //If the kmer is found, check if it is present at that position
		auto p = std::find(f->second.begin(), f->second.end(), position);
		if (p != f->second.end()) { //If you find it at that position
			return true;
		}
		return false;
	}
	return false;
}

/**
 * Deletes from the given updated genome position, a string of length "del_len"
 *
 * Sampling ensures that no two k-mers which are hashed are more than 'S' apart on the current genome
 * The delete is 'anchored' by hashing the kmer occurring immediately after the deletion position
 */

//temporary sanity function
/*
void genome::check_node_correctness(long pos,long delete_pos_abs){

	node *n=s.find(pos);
	if(n){
		if(n->val!= pos){
			LOGWARN(FILE_LOGGER,"Node not found, prev node found in skip list: genome_pos = "+to_string(pos)+ " abs position= "+to_string(delete_pos_abs));
		}
		else{
			if((n->str).empty() && (n->offset)>0){
				LOGWARN(FILE_LOGGER,"No string present: genome_pos = "+to_string(pos)+ " abs position= "+to_string(delete_pos_abs));
			}
		}
	}
	else{
		LOGWARN(FILE_LOGGER,"Node not found in skip list: genome_pos = "+to_string(pos)+ " abs position= "+to_string(delete_pos_abs));
	}
}*/

bool genome::delete_at(const unsigned long delete_pos_abs, const unsigned long del_len) {

	auto kmers_to_replace = get_kmers(delete_pos_abs - K + 1, K - 1);
	int i = 0;

	long genome_position;
	const string end_kmer = read_reference_abs_at(delete_pos_abs + del_len,
	K - 1, genome_position);
	//genome_position -= del_len; //will not work for generic case

	LOGDEBUG(FILE_LOGGER, std::to_string(delete_pos_abs)+ " - "+std::to_string(delete_pos_abs+del_len-1) );

	int sample = 0;
	for (auto it : kmers_to_replace) {
		remove_kmer_from_hash_at(it.second, it.first);
		string temp = it.first.substr(0, K - i - 1) + end_kmer.substr(0, i + 1);
		if (it.first.length() == K) { //For handling edge cases: (insertion at the end of the genome)
			if (sample % S == 0) { //Sampling
				add_kmer_to_hash_at(it.second, temp);
				LOGDEBUG(FILE_LOGGER, "Replaced " + it.first + " with " + temp +  " at " + std::to_string(it.second));
			}
		}

		sample++;
		i++;
	}

	vector<tuple<string, long, unsigned long>> kmers_to_delete =
			get_kmers_with_offset(delete_pos_abs, del_len + 1);

	for (auto it = kmers_to_delete.begin(); it != (kmers_to_delete.end() - 1); it++) {
		long g_pos = get<1>(*it);
		string kmer = get<0>(*it);
		remove_kmer_from_hash_at(g_pos, kmer);
		del[g_pos] = 1;
	}

	//case where the kmers must be "moved in"
	if (!(get<2>(*kmers_to_delete.begin()))) {
		if (get<1>(*kmers_to_delete.begin()) == get<1>(*(kmers_to_delete.end() - 1))) {
			del[get<1>(*kmers_to_delete.begin())] = 0;
			string temp = get<0>(*(kmers_to_delete.end() - 1));
			reference[get<1>(*(kmers_to_delete.end() - 1))] = temp[0]; //pushing kmer in
		}
	}

	//Update the skip list
	mod_kmers m_kmers = s.delete_and_update_abs(delete_pos_abs, del_len);

	for(auto val:m_kmers.insertions){

		//check_node_correctness(val,delete_pos_abs);
		LOGDEBUG(FILE_LOGGER, "Insertions "+to_string(val));
		ins[val]=1;
		del[val]=0;
	}
	for(auto val:m_kmers.deletions){

		//check_node_correctness(val,delete_pos_abs);
		LOGDEBUG(FILE_LOGGER, "Deletions "+to_string(val));
			ins[val]=0;
			del[val]=1;
		}

	if(m_kmers.num_kmers_moved>0){

		//sanity checks
			ins[m_kmers.final_genome_pos]=1;
			del[m_kmers.final_genome_pos]=0;

			ins[m_kmers.initial_genome_pos]=0;
			del[m_kmers.initial_genome_pos]=1;

		//check_node_correctness(m_kmers.final_genome_pos,delete_pos_abs);
		LOGDEBUG(FILE_LOGGER, "Final genome pos "+to_string(m_kmers.final_genome_pos));
		//check_node_correctness(m_kmers.initial_genome_pos,delete_pos_abs);
		LOGDEBUG(FILE_LOGGER, "Initial genome pos "+to_string(m_kmers.initial_genome_pos));

		string ref=read_reference_at(m_kmers.final_genome_pos,m_kmers.final_offset+1,m_kmers.num_kmers_moved+K-1);

		for(int i=0;i<m_kmers.num_kmers_moved;i++){
			remove_kmer_from_hash_at(m_kmers.initial_genome_pos,ref.substr(i,K));
			add_kmer_to_hash_at(m_kmers.final_genome_pos,ref.substr(i,K));
		}
	}

	//Sampling - anchoring the deletion by hashing the kmer appearing right after the insertion
	auto anchor = get_kmers(delete_pos_abs + 1, 1).front();
	LOGDEBUG(FILE_LOGGER, "Anchor: " + anchor.first);
	if(!is_kmer_hashed(anchor.first, anchor.second)){
		add_kmer_to_hash_at(anchor.second, anchor.first);
		LOGDEBUG(FILE_LOGGER, "Adding anchor kmer " + anchor.first + " at " + std::to_string(anchor.second));
	}

	return true;
}


