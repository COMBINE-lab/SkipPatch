#include <utility>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <string>

#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/string.hpp"

#include "genome.h"
#include "utils.h"

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
	//ignore_first_line();
	ifstream myfile(path);
	if (myfile.is_open()) {

		std::string input;
		while (getline(myfile, input)) {
			reference += input;
			input.clear();
		}
#ifdef DEBUG
		std::cout<<"Input taken! "<<std::endl;
#endif

		ins = std::vector<bool>(get_length(), false);
		del = std::vector<bool>(get_length(), false);
	} else {
		cout << "File is not open";
	}
}

void genome::set_reference(std::string input) {
	reference = input;
	ins = std::vector<bool>(get_length(), false);
	del = std::vector<bool>(get_length(), false);
}

std::string genome::get_reference() {
	return reference;
}

std::string genome::get_updated_reference() {

	std::string updated_reference;

	node *temp = s.get_head();
	while (temp->down) {
		temp = temp->down;
	}
	temp = temp->next;

	long start = 0, end = 0;

	while (temp->next) {
		end = (temp->val) + 1;
		updated_reference += std::string(reference.begin() + start,
				reference.begin() + end);
		updated_reference += temp->str;
		start = end;
		temp = temp->next;
	}
	updated_reference += std::string(reference.begin() + start,
			reference.end());

	return updated_reference;

	//return read_reference_at(0,0,get_length());

}

long genome::get_length() {
	return reference.length();
}

std::unordered_map<std::string, std::vector<long>> genome::get_hash() {
	return m;
}

/*
 std::unordered_map<std::string, std::vector<long>, std::function<unsigned long(std::string)>> genome::get_hash()
 {
 return m;
 }
 */

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
	for (auto it = reference.begin(); it <= reference.end() - K; it++) {
		std::string temp(it, it + K);
		if (!(temp.find('n') != std::string::npos)
				&& !(temp.find('N') != std::string::npos))
			m[temp].push_back(it - reference.begin());
	}
}

float genome::get_load_factor() {
	return m.load_factor();
}

void genome::display_genome() {
	std::cout << reference << std::endl;
}

void genome::display_updated_genome() {
	std::cout << get_updated_reference() << std::endl;
}

void genome::display_hash() {
	std::cout << std::endl << "Hash Table: " << std::endl;
	for (auto it = m.begin(); it != m.end(); it++) {
		std::cout << it->first << "\t";
		for (auto vector_it = (it->second).begin();
				vector_it != (it->second).end(); vector_it++) {
			std::cout << *vector_it << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void genome::display_load() {
	std::cout << "size = " << m.size() << std::endl;
	std::cout << "bucket_count = " << m.bucket_count() << std::endl;
	std::cout << "max_load_factor = " << m.max_load_factor() << std::endl;
}

void genome::remove_kmer_from_hash_at(long position_to_remove,
		std::string curr_kmer) {

	if (!m[curr_kmer].empty()) {
		if (std::find(m[curr_kmer].begin(), m[curr_kmer].end(),
				position_to_remove) != m[curr_kmer].end()) {
			m[curr_kmer].erase(
					std::find(m[curr_kmer].begin(), m[curr_kmer].end(),
							position_to_remove));
		}
	}
	//Having a very long "vector/list" of positions (~1 million) causes a bottleneck here
	//Approximately takes 0.5 seconds to execute this line "once" if the length of "positions" is 1 million

	if (m[curr_kmer].size() == 0) {
		m.erase(curr_kmer);
	}
}

void genome::add_kmer_from_hash_at(long position, std::string new_kmer) {
	m[new_kmer].push_back(position);
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

long genome::get_virtual_position_from_genome_position(long genome_position,
		long offset) //arguement must be a tuple
		{
	return genome_position + s.get_cumulative_count(genome_position) + offset;
}

void genome::snp_at(const long snp_pos_abs, const std::string snp) {

	LOGDEBUG(FILE_LOGGER,"snp_at: " + std::to_string(snp_pos_abs) + " " + snp);
	const long snp_len = snp.length();

	auto kmer_pos_pair = get_kmers(snp_pos_abs - K + 1, snp_len + K - 1);
	int i = 0;

	long genome_position;

	//Edge cases: Use min, max to adjust these values for ends of genome
	std::string snp_update = read_reference_abs_at(snp_pos_abs - K + 1,
			2 * K - 2 + snp_len, genome_position);
	snp_update.replace(K - 1, snp_len, snp);

	for (auto it : kmer_pos_pair) {
		remove_kmer_from_hash_at(it.second, it.first);
		string new_kmer = snp_update.substr(i, K);
		if (it.first.length() == K) {
			add_kmer_from_hash_at(it.second, new_kmer);
		}
		LOGDEBUG(FILE_LOGGER,"Replaced " + it.first + " with " + new_kmer + " at " + std::to_string(it.second));
		i++;
	}
	//s.print_base_level();

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

std::vector<long> genome::search(std::string read) {

	std::vector<long> positions;

	//Read the first k-mer of the read and find all the positions it occurrs at
	std::string read_kmer = read.substr(0, K);
	auto f = m.find(read_kmer);

	if (f != m.end()) {
		auto& search = f->second;

		//if(!search.empty()){

		//Remove duplicates, take care of multiple occurrences of a read starting from within an insertion later
		std::sort(search.begin(), search.end());
		search.erase(std::unique(search.begin(), search.end()), search.end());

		for (long pos : search) {

			long offset = 0;

			if (!ins[pos]) { //If the position doesn't start from within an insertion
				LOGDEBUG(FILE_LOGGER,"POS(G): " + std::to_string(pos));
				if (::memcmp(read.c_str(),
						read_reference_at(pos, offset, read.length()).c_str(),
						read.length()) == 0) {
					positions.push_back(
							get_virtual_position_from_genome_position(pos, offset));
					LOGDEBUG(FILE_LOGGER,"Adding at(1): " + std::to_string(get_virtual_position_from_genome_position(pos,offset)));
				} else {
					// TODO: What happens in this case? It is an error, right?
				}
			} else {
				LOGDEBUG(FILE_LOGGER,"POS(I): " + std::to_string(pos));

				//A read can occurr multiple times within an insertion itself
				//And hence might have different offsets for those locations
				//Eureka! It works! ;) And that's how horrible some test cases can get :P

				//Read off length of insertion + K - 1, and find all the offsets of the first k-mer of the read
				long insertion_length = (s.find(pos)->str).length();
				std::string insertion_ext = read_reference_at(pos, offset,
						insertion_length + K);
				offset = insertion_ext.find(read_kmer);
				while (offset != std::string::npos) {
					LOGDEBUG(FILE_LOGGER,"read_ext: " + std::to_string(pos) + " " + std::to_string(offset) + " " + std::to_string(read.length()) + " " + read_reference_at(pos, offset, read.length()));
					if (::memcmp(read.c_str(), read_reference_at(pos, offset, read.length()).c_str(), read.length()) == 0) {
						positions.push_back(get_virtual_position_from_genome_position(pos, offset));
						LOGDEBUG(FILE_LOGGER,"Adding at(2): " + std::to_string(get_virtual_position_from_genome_position(pos,offset)));
					}
					offset = insertion_ext.find(read_kmer, offset + 1);
				}
			}
		}
	}

	return positions;
}

/*
 * reads the reference from the specified genome(origional) position and offset (in the skip list)
 * up to the specified length or until it reaches the end of the genome.
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
				node *n = s.find(curr_genome_pos);
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

string genome::read_reference_abs_at(const long abs_pos, const long len,
		long &genome_position) {

	unsigned long offset;
	node* n;
	string kmer;
	get_genome_position_from_virtual_position(abs_pos, genome_position, offset,
			&n);
	kmer = read_reference_at(genome_position, offset, len);
	return kmer;
}

void genome::read_reference_abs_at(const long abs_pos, const long len,
		long &genome_position, unsigned long &offset, string &kmer) {
	node* n;
	get_genome_position_from_virtual_position(abs_pos, genome_position, offset,
			&n);
	kmer = read_reference_at(genome_position, offset, len);
}

/**
 * Get "kmer_count" k-mers from a "start" position on the updated genome
 *
 * TODO: Handle edge cases
 */
vector<pair<string, long>> genome::get_kmers(const long start,
		const unsigned long kmer_count) {

	vector<pair<string, long> > vec;
	for (long i = start; i < start + kmer_count; i++) //could be faster
			{
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
 * TODO: Handle edge cases
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
 */
void genome::insert_at(const std::string insertion,
		const unsigned long insert_pos_abs) {

	LOGDEBUG(FILE_LOGGER,"insert_at: " + insertion + " at "  + std::to_string(insert_pos_abs));

	const long ins_len = insertion.length();

	auto kmer_pos_pair = get_kmers(insert_pos_abs - K + 2, K - 1);
	int i = 0;

	long genome_position;
	const string end_kmer = read_reference_abs_at(insert_pos_abs + 1, K - 1,
			genome_position);
	string ins_copy = insertion + end_kmer;

	LOGDEBUG(FILE_LOGGER, std::to_string(insert_pos_abs) + " " + std::to_string(genome_position));

	for (auto it : kmer_pos_pair) {
		remove_kmer_from_hash_at(it.second, it.first);
		string temp = it.first.substr(0, K - i - 1) + ins_copy.substr(0, i + 1);
		if (it.first.length() == K) { //For handling edge cases: (insertion at the end of the genome)
			add_kmer_from_hash_at(it.second, temp);
		}
		i++;
	}

	long add_pos = genome_position;
	if (!kmer_pos_pair.empty()) { //For handling edge cases: (insertion at the beginning of the genome)
		add_pos = kmer_pos_pair.back().second;
	}

	for (i = 0; i < insertion.length(); i++) {
		string temp = insertion.substr(i, K);
		string temp2 = end_kmer.substr(0, K - temp.length());
		if ((temp + temp2).length() == K) { //For handling edge cases: (insertion at the end of the genome)
			add_kmer_from_hash_at(add_pos, temp + temp2);
		}
	}

	//Set the ins bit to true
	ins[add_pos] = 1;

	//Update the skip list
	s.insert_and_update_abs(insert_pos_abs, insertion);

}

/**
 * Deletes from the updated genome position, a string of length "del_len"
 *
 */
bool genome::delete_at(const unsigned long delete_pos_abs,
		const unsigned long del_len) {
	if (!s.is_valid_delete(delete_pos_abs, del_len)) {
		cout << ".";
		// cout<<"Invalid delete caught!"<<endl;
		return false;
	}
	auto kmers_to_replace = get_kmers(delete_pos_abs - K + 1, K - 1);
	int i = 0;

	long genome_position;
	const string end_kmer = read_reference_abs_at(delete_pos_abs + del_len,
	K - 1, genome_position);
	genome_position -= del_len; //will not work for generic case

	LOGDEBUG(FILE_LOGGER, std::to_string(delete_pos_abs+del_len) + " " + std::to_string(genome_position));

	for (auto it : kmers_to_replace) {
		remove_kmer_from_hash_at(it.second, it.first);
		string temp = it.first.substr(0, K - i - 1) + end_kmer.substr(0, i + 1);
		if (it.first.length() == K) { //For handling edge cases: (insertion at the end of the genome)
			add_kmer_from_hash_at(it.second, temp);
		}
		LOGDEBUG(FILE_LOGGER, "Replaced " + it.first + " with " + temp +  " at " + std::to_string(it.second));
		i++;
	}

	vector<tuple<string, long, unsigned long>> kmers_to_delete =
			get_kmers_with_offset(delete_pos_abs, del_len + 1);

	for (auto it = kmers_to_delete.begin(); it != (kmers_to_delete.end() - 1);
			it++) {
		long g_pos = get<1>(*it);
		string kmer = get<0>(*it);
		remove_kmer_from_hash_at(g_pos, kmer);
		del[g_pos] = 1;
		/*
		 if(!(get<2>(*kmers_to_delete.begin())) && get<1>(it)==(get<1>(*kmers_to_delete.begin())))
		 {
		 del[(get<1>(*kmers_to_delete.begin()))]=0
		 reference[(get<1>(*kmers_to_delete.begin()))] =
		 }*/
		//std::cout << "Removed " << it.first << " from " << it.second << std::endl;
	}

	//case where the kmers must be "moved in"
	if (!(get<2>(*kmers_to_delete.begin()))) {
		if (get<1>(*kmers_to_delete.begin())
				== get<1>(*(kmers_to_delete.end() - 1))) {
			del[get<1>(*kmers_to_delete.begin())] = 0;
			string temp = get<0>(*(kmers_to_delete.end() - 1));
			reference[get<1>(*(kmers_to_delete.end() - 1))] = temp[0]; //pushing kmer in
		}
	}
	//Update the skip list
	s.delete_and_update_abs(delete_pos_abs, del_len);
	return true;
}
