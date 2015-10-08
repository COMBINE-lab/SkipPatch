#include <iostream>
#include <vector>
#include <string>
#include <cassert>

#include "./skip_list/skip_list.h"
#include "./skip_list/skip_list_test.h"

#include "test.h"
#include "genome.h"
#include "utils.h"

using namespace std;

/**
 *
 */
void test_hash_fwd(std::unordered_map<uint64_t, std::vector<long>> m_temp,
		std::unordered_map<uint64_t, std::vector<long>> m_genome,
		skip_list s) {

	for (auto it = m_temp.begin(); it != m_temp.end(); it++) {
		for (auto row_it = (it->second).begin(); row_it != (it->second).end();
				row_it++) {

			//translate from abs to genome pos:
			unsigned long pos;
			long val;
			node *n;
			s.get_prev_node(*row_it, val, pos, &n);
			*row_it = val;
			//LOGDEBUG(FILE_LOGGER, std::to_string(*row_it));
		}

		sort(it->second.begin(), it->second.end());
		sort(m_genome[it->first].begin(), m_genome[it->first].end()); 

		assert(it->second == m_genome[it->first]);
	}
}

/**
 *
 */
void test_hash_rev(std::unordered_map<uint64_t, std::vector<long>> m_temp,
		std::unordered_map<uint64_t, std::vector<long>> m_genome,
		skip_list s) {
	for (auto it = m_genome.begin(); it != m_genome.end(); it++) {

		for (auto row_it = m_temp[it->first].begin();
				row_it != m_temp[it->first].end(); row_it++) {
			//translate from abs to genome pos:
			unsigned long pos;
			long val;
			node *n;
			s.get_prev_node(*row_it, val, pos, &n);
			*row_it = val;
			//LOGDEBUG(FILE_LOGGER, std::to_string(*row_it));
		}

		sort(it->second.begin(), it->second.end());
		sort(m_temp[it->first].begin(), m_temp[it->first].end());

		assert(it->second == m_temp[it->first]);
	}
}

/**
 *
 */
void test_hash(std::unordered_map<uint64_t, std::vector<long>> m_temp,
		std::unordered_map<uint64_t, std::vector<long>> m_genome,
		skip_list s) {

	test_hash_fwd(m_temp, m_genome, s);
	test_hash_rev(m_temp, m_genome, s);
}

/**
 *
 */
void check_insert_at(genome g, string ins, long abs_val, string &reference) {

	//test if the the skip list has been updated..
	skip_list s = g.get_skip_list();
	//check_skip_list_node(s, abs_val, ins); 

	//check if the reference has been updated
	reference.insert(abs_val + 1, ins);
	assert(reference == g.read_reference_at(0, 0, reference.length()));

	//now check the hash
	//update the reference and construct hash for the same	
	genome g_temp;
	g_temp.set_reference(reference);
	g_temp.construct_hash();

	auto m_temp = g_temp.get_hash();
	auto m_genome = g.get_hash();
	test_hash(m_temp, m_genome, s);
}

/**
 * Tests if the search for a string of any length in the hash map is working correctly
 */
void test_search_static_reference() {

	LOGINFO(FILE_LOGGER, "Starting.. ");

	genome g;
	std::string reference =
			"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGA";
	g.set_reference(reference);
	g.construct_hash();

	std::vector<std::string> existing_reads { "AGCTAGCC",
			"TGATCGATCGATCGCAGATCGA", "ATTAGCTAGCCTAGCTAGTA",
			"CCCCCTATCATCATCATCTACTACAT", "GTAGATGGATCTCCCCCTATCATCAT",
			"TGGATCTCCCCCTATCATCA",
			"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACT",
			"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCGA" };

	for (std::string read : existing_reads) {
		assert(g.search(read) == find_substr(reference, read));
	}

	std::vector<std::string> non_existent_reads { "TGATCGATCGATCGCAGATCTT",
			"ATTAGCTAGTTTAGCTAGTA", "CCCCCTATCATTCATCTACTACATC",
			"GTAGATGGATCTCCCCTATCATCAT", "TGGATCTCCCCCTATCATCAAT",
			"ATTAGCTAGCTAGCTAGTAGATGGATCTCCCTATCATCTCATCTACT",
			"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATCGCAGATCAC" };

	for (std::string read : non_existent_reads) {
		assert(g.search(read).size() == 0);
	}

	LOGINFO(FILE_LOGGER, "Complete");
}

/**
 *
 */
void check_search(genome g, string reference) {

	//Randomize
	std::vector<long> positions { 14, 21, 33, 11, 46 }; //careful.. index+K should be in bounds!
	std::vector<std::string> existing_reads;

	for (long pos : positions) {

		int kmer_len = 0;
		if ((pos) % 2 == 0) {
			kmer_len = K + 5;
		} else {
			kmer_len = K + 6;
		}

		string temp(reference, pos, kmer_len);
		existing_reads.push_back(temp);
	}

	for (std::string read : existing_reads) {

		auto p_found = g.search(read);
		auto p_actual = find_substr(reference, read);

		//LOGDEBUG(FILE_LOGGER,"Reference: " + reference);
		for(auto it = p_actual.begin();it!=p_actual.end();it++){
			LOGDEBUG(FILE_LOGGER,"Read: " + read + " should be at "  + std::to_string(*it));
		}
		for(auto it = p_found.begin();it!=p_found.end();it++){
			LOGDEBUG(FILE_LOGGER,"Read: " + read + " is actually at "  + std::to_string(*it));
		}

		//Sort
		sort(p_found.begin(), p_found.end());
		sort(p_actual.begin(), p_actual.end());
		//p_found.erase(unique(p_found.begin(),p_found.end()), p_found.end());
		//p_actual.erase(unique(p_actual.begin(),p_actual.end()), p_actual.end());

		assert(p_found == p_actual);
	}

	//non_existent_reads
	for (std::string read : existing_reads) {
		read += std::string("Z");
		assert(g.search(read).size() == 0);
	}
}

/**
 *
 */
void test_search_dynamic_reference() {

	LOGINFO(FILE_LOGGER, "Starting.. ");

	genome g;
	std::string reference =
			"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCAT";
	g.set_reference(reference);
	g.construct_hash();
	check_search(g, reference);

	std::vector<std::string> insertions { "A", "TCA", "AGATC", "ATGATAC", "TTTT" };
	std::vector<long> ins_positions { 42, 9, 33, 12, 50, 25 };

	for (std::string ins : insertions) {

		for (long abs_val : ins_positions) {
			LOGDEBUG(FILE_LOGGER,"Position: " + std::to_string(abs_val) + " Insertion: " + ins);
			g.insert_at(ins, abs_val);
			check_insert_at(g, ins, abs_val, reference);
			check_search(g, reference);
		}
	}

	LOGINFO(FILE_LOGGER, "Complete");
}

/**
 *
 */
void test_insert_at() {

	LOGINFO(FILE_LOGGER, "Starting.. ");

	//length of insertion <K, >K, =K, =1
	std::vector<std::string> insertions { "ATG", "TTGTAC", "ATCG", "C",
			"TTTTTTTT" };

	for (std::string ins : insertions) {

		genome g;
		std::string reference =
				"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGAT";
		g.set_reference(reference);
		g.construct_hash();

		//Randomize? 
		std::vector<unsigned long> positions { reference.length() - K, 2, 5, 4,
				7, 11, 12, 24, 36, 30 };

		for (long p : positions) {

			LOGDEBUG(FILE_LOGGER,"Position: " + std::to_string(p) + " Insertion: " + ins);

			g.insert_at(ins, p);
			check_insert_at(g, ins, p, reference);
		}
	}

	LOGINFO(FILE_LOGGER, "Complete");
}

/**
 *
 */
void check_delete_at(genome &g, long position, long len, string &reference) {

	//test if the the skip list has been updated..
	skip_list s = g.get_skip_list();
	//check_skip_list_node(s, abs_val, ins); 

	//check if the reference has been updated
	reference.erase(position, len);

	//LOG_DEBUG(FILE_LOGGER, "Actual Reference: " + *reference);
	//LOG_DEBUG(FILE_LOGGER, "Incorrect Reference: " + g.read_reference_at(0,0,reference.length())); //TODO: What?
	assert(reference == g.read_reference_at(0, 0, reference.length()));

	//now check the hash
	//update the reference and construct hash for the same	
	genome g_temp;
	g_temp.set_reference(reference);
	g_temp.construct_hash();

	auto m_temp = g_temp.get_hash();
	auto m_genome = g.get_hash();
	test_hash(m_temp, m_genome, s);
}

/**
 *
 */
void test_delete_at() {

	LOGINFO(FILE_LOGGER, "Starting.. ");

	std::vector<long> del_lengths { 1, 2, 2, 1, 2 };
	std::vector<long> positions { 18, 15, 55, 19, 60 };
	std::string reference =
			"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCTAGCTAGTAGATGGATCTCCCTATCATCTCATCTACT";
	for (long p : positions) {

		genome g;

		g.set_reference(reference);
		g.construct_hash();
		for (long del_len : del_lengths) {

			LOGDEBUG(FILE_LOGGER,"Position: " + std::to_string(p) + " Deletion Length: " + std::to_string(del_len));
			//LOG_DEBUG(FILE_LOGGER, "Reference: " + *reference);

			const string ins = "AGCTAGCC";
			g.insert_at(ins, 4);
			check_insert_at(g, ins, 4, reference);

			g.insert_at(ins, p);
			check_insert_at(g, ins, p, reference);
			g.delete_at(p + 1, del_len);
			check_delete_at(g, p + 1, del_len, reference);
			g.delete_at(p, del_len);
			check_delete_at(g, p, del_len, reference);
			g.delete_at(p - 10, del_len);
			check_delete_at(g, p - 10, del_len, reference);
		}
	}

	LOGINFO(FILE_LOGGER, "Complete");
}

/**
 *
 */
void test_indels() {

	LOGINFO(FILE_LOGGER, "Starting.. ");

	std::string reference =
			"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGAT";
	//length of insertion <K, >K, =K, =1
	std::vector<std::string> insertions { "ATGATGATGATG", "TTGTACATGATG",
			"ATCGATCGATGATG", "CATCGATGATG" };
	std::vector<long> del_lengths { 1, 2 };
	for (std::string ins : insertions) {

		genome g;

		g.set_reference(reference);
		g.construct_hash();

		//Randomize? 
		std::vector<unsigned long> positions { 21, 42, 60, 75, 95, 110, 130 };

		for (long p : positions) {

			LOGDEBUG(FILE_LOGGER,"Position: " + std::to_string(p) + " Insertion: " + ins);

			g.insert_at(ins, p);
			check_insert_at(g, ins, p, reference);
			for (long d : del_lengths) {
				g.delete_at(p + 2, d);
				check_delete_at(g, p + 2, d, reference);

				g.delete_at(p + 1, d);
				check_delete_at(g, p + 1, d, reference);

				g.delete_at(p, d);
				check_delete_at(g, p, d, reference);

				g.delete_at(p - 10, d);
				check_delete_at(g, p - 10, d, reference);

			}

			//g.get_skip_list().print_list();
			//g.get_skip_list().print_base_level();
		}
	}
	del_lengths= {1};
	insertions= {"TT", "TG", "AT", "CA"};
	reference =
			"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGAT";
	for (std::string ins : insertions) {

		genome g;

		g.set_reference(reference);
		g.construct_hash();

		//Randomize? 
		std::vector<unsigned long> positions { 21, 42, 23, 60, 75, 95, 110, 130 };

		for (long p : positions) {

			LOGDEBUG(FILE_LOGGER,"Position: " + std::to_string(p) + " Insertion: " + ins);

			g.insert_at(ins, p);
			check_insert_at(g, ins, p, reference);
			for (long d : del_lengths) {

				g.delete_at(p + 1, d);
				check_delete_at(g, p + 1, d, reference);

				g.delete_at(p, d);
				check_delete_at(g, p, d, reference);

				g.delete_at(p + 10, d);
				check_delete_at(g, p + 10, d, reference);

				g.insert_at(ins, p);
				check_insert_at(g, ins, p, reference);

				g.delete_at(p + 2, d);
				check_delete_at(g, p + 2, d, reference);

				g.delete_at(p + 3, d);
				check_delete_at(g, p + 3, d, reference);

				g.insert_at(ins, p + 3);
				check_insert_at(g, ins, p + 3, reference);

				g.delete_at(p - 10, d);
				check_delete_at(g, p - 10, d, reference);

			}

		}
	}

	LOGINFO(FILE_LOGGER, "Complete");

}

/**
 *
 */
void check_snp_at(genome g, string ins, long abs_val, string &reference) {

	//test if the the skip list has been updated..
	skip_list s = g.get_skip_list();
	//check_skip_list_node(s, abs_val, ins); 

	//check if the reference has been updated
	reference.erase(abs_val, ins.length());
	reference.insert(abs_val, ins);
	assert(reference == g.read_reference_at(0, 0, reference.length()));

	//now check the hash
	//update the reference and construct hash for the same	
	genome g_temp;
	g_temp.set_reference(reference);
	g_temp.construct_hash();

	auto m_temp = g_temp.get_hash();
	auto m_genome = g.get_hash();
	test_hash(m_temp, m_genome, s);
}

/**
 *
 */
void test_snp_at() {

	LOGINFO(FILE_LOGGER, "Starting.. ");

	std::string reference =
			"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGAT";
	//length of insertion <K, >K, =K, =1
	std::vector<std::string> insertions { "ATGATGATGATG", "TTGTACATGATG",
			"ATCGATCGATGATG", "CATCGATGATG" };
	std::vector<long> del_lengths { 1, 2 };
	for (std::string ins : insertions) {

		genome g;

		g.set_reference(reference);
		g.construct_hash();

		//Randomize? 
		std::vector<unsigned long> positions { 21, 42, 60, 75, 95, 110, 130 };

		for (long p : positions) {
			g.snp_at(p, "ATC");
			check_snp_at(g, "ATC", p, reference);

			g.insert_at(ins, p);
			check_insert_at(g, ins, p, reference);

			g.snp_at(p, "C");
			check_snp_at(g, "C", p, reference);

			for (long d : del_lengths) {
				g.delete_at(p + 2, d);
				check_delete_at(g, p + 2, d, reference);

				g.snp_at(p, "AGT");
				check_snp_at(g, "AGT", p, reference);
				g.delete_at(p + 1, d);
				check_delete_at(g, p + 1, d, reference);

				g.snp_at(p, "TTTTTTTTT");
				check_snp_at(g, "TTTTTTTTT", p, reference);
				g.delete_at(p, d);
				check_delete_at(g, p, d, reference);

				g.snp_at(p, "GGG");
				check_snp_at(g, "GGG", p, reference);

				g.delete_at(p - 10, d);
				check_delete_at(g, p - 10, d, reference);

				g.snp_at(p, "AT");
				check_snp_at(g, "AT", p, reference);
			}

			//g.get_skip_list().print_list();
			//g.get_skip_list().print_base_level();
		}
	}
	del_lengths= {1};
	insertions= {"TT", "TG", "AT", "CA"};
	reference =
			"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGAT";
	for (std::string ins : insertions) {

		genome g;

		g.set_reference(reference);
		g.construct_hash();

		//Randomize? 
		std::vector<unsigned long> positions { 21, 42, 23, 60, 75, 95, 110, 130 };

		for (long p : positions) {

			LOGDEBUG(FILE_LOGGER,"Position: " + std::to_string(p) + " Insertion: " + ins);

			g.insert_at(ins, p);
			check_insert_at(g, ins, p, reference);

			g.snp_at(p, "TT");
			check_snp_at(g, "TT", p, reference);
			for (long d : del_lengths) {

				g.snp_at(p, "TTT");
				check_snp_at(g, "TTT", p, reference);
				g.delete_at(p + 1, d);
				check_delete_at(g, p + 1, d, reference);

				g.snp_at(p, "ATC");
				check_snp_at(g, "ATC", p, reference);
				g.delete_at(p, d);
				check_delete_at(g, p, d, reference);

				g.snp_at(p, "AG");
				check_snp_at(g, "AG", p, reference);
				g.delete_at(p + 10, d);
				check_delete_at(g, p + 10, d, reference);

				g.snp_at(p, "ATCCG");
				check_snp_at(g, "ATCCG", p, reference);
				g.insert_at(ins, p);
				check_insert_at(g, ins, p, reference);

				g.snp_at(p, "T");
				check_snp_at(g, "T", p, reference);
				g.delete_at(p + 2, d);
				check_delete_at(g, p + 2, d, reference);

				g.snp_at(p, "A");
				check_snp_at(g, "A", p, reference);
				g.delete_at(p + 3, d);
				check_delete_at(g, p + 3, d, reference);

				g.snp_at(p, "ATG");
				check_snp_at(g, "ATG", p, reference);
				g.insert_at(ins, p + 3);
				check_insert_at(g, ins, p + 3, reference);

				g.delete_at(p - 10, d);
				check_delete_at(g, p - 10, d, reference);

			}

		}
	}

	LOGINFO(FILE_LOGGER, "Complete");

}

/**
 *
 */
void test_search() {

	LOGINFO(FILE_LOGGER, "Starting.. ");

	test_search_static_reference();
	test_search_dynamic_reference();

	LOGINFO(FILE_LOGGER, "Complete");

}

/**
 *
 */
void test_search_at() {

	LOGINFO(FILE_LOGGER, "Starting.. ");

	std::string reference =
			"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGAT";
	//length of insertion <K, >K, =K, =1
	std::vector<std::string> insertions { "ATGATGATGATG", "TTGTACATGATG",
			"ATCGATCGATGATG", "CATCGATGATG" };
	std::vector<long> del_lengths { 1, 2 };
	for (std::string ins : insertions) {

		genome g;

		g.set_reference(reference);
		g.construct_hash();

		//Randomize? 
		std::vector<unsigned long> positions { 21, 42, 60, 75, 95, 110, 130 };

		for (long p : positions) {
			g.snp_at(p, "ATC");
			check_snp_at(g, "ATC", p, reference);
			check_search(g, reference);
			g.insert_at(ins, p);
			check_insert_at(g, ins, p, reference);
			check_search(g, reference);
			g.snp_at(p, "C");
			check_snp_at(g, "C", p, reference);
			check_search(g, reference);
			for (long d : del_lengths) {
				g.delete_at(p + 2, d);
				check_delete_at(g, p + 2, d, reference);
				check_search(g, reference);
				g.snp_at(p, "AGT");
				check_snp_at(g, "AGT", p, reference);
				check_search(g, reference);
				g.delete_at(p + 1, d);
				check_delete_at(g, p + 1, d, reference);
				check_search(g, reference);
				g.snp_at(p, "TTTTTTTTT");
				check_snp_at(g, "TTTTTTTTT", p, reference);
				g.delete_at(p, d);
				check_delete_at(g, p, d, reference);
				check_search(g, reference);
				g.snp_at(p, "GGG");
				check_snp_at(g, "GGG", p, reference);
				check_search(g, reference);
				g.delete_at(p - 10, d);
				check_delete_at(g, p - 10, d, reference);
				check_search(g, reference);
				g.snp_at(p, "AT");
				check_snp_at(g, "AT", p, reference);
				check_search(g, reference);
			}

			//g.get_skip_list().print_list();
			//g.get_skip_list().print_base_level();
		}
	}
	del_lengths= {1};
	insertions= {"TT", "TG", "AT", "CA"};
	reference =
			"ATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGATATTAGCTAGCCTAGCTAGTAGATGGATCTCCCCCTATCATCATCATCTACTACATCAGCATGATCGATCGAT";
	for (std::string ins : insertions) {

		genome g;

		g.set_reference(reference);
		g.construct_hash();

		//Randomize? 
		std::vector<unsigned long> positions { 21, 42, 23, 60, 75, 95, 110, 130 };

		for (long p : positions) {

			LOGDEBUG(FILE_LOGGER,"Position: " + std::to_string(p) + " Insertion: " + ins);

			g.insert_at(ins, p);
			check_insert_at(g, ins, p, reference);

			g.snp_at(p, "TT");
			check_snp_at(g, "TT", p, reference);
			check_search(g, reference);
			for (long d : del_lengths) {

				g.snp_at(p, "TTT");
				check_snp_at(g, "TTT", p, reference);
				g.delete_at(p + 1, d);
				check_delete_at(g, p + 1, d, reference);
				check_search(g, reference);
				g.snp_at(p, "ATC");
				check_snp_at(g, "ATC", p, reference);
				g.delete_at(p, d);
				check_delete_at(g, p, d, reference);
				check_search(g, reference);
				g.snp_at(p, "AG");
				check_snp_at(g, "AG", p, reference);
				g.delete_at(p + 10, d);
				check_delete_at(g, p + 10, d, reference);
				check_search(g, reference);
				g.snp_at(p, "ATCCG");
				check_snp_at(g, "ATCCG", p, reference);
				g.insert_at(ins, p);
				check_insert_at(g, ins, p, reference);
				check_search(g, reference);
				g.snp_at(p, "T");
				check_snp_at(g, "T", p, reference);
				g.delete_at(p + 2, d);
				check_delete_at(g, p + 2, d, reference);
				check_search(g, reference);
				g.snp_at(p, "A");
				check_snp_at(g, "A", p, reference);
				g.delete_at(p + 3, d);
				check_delete_at(g, p + 3, d, reference);
				check_search(g, reference);
				g.snp_at(p, "ATG");
				check_snp_at(g, "ATG", p, reference);
				g.insert_at(ins, p + 3);
				check_insert_at(g, ins, p + 3, reference);

				g.delete_at(p - 10, d);
				check_delete_at(g, p - 10, d, reference);
				check_search(g, reference);
			}

		}
	}

	LOGINFO(FILE_LOGGER, "Complete");

}

void test() {

	LOGINFO(FILE_LOGGER, "Starting.. ");

	test_skip_list();
	test_insert_at();
	test_delete_at();
	test_indels();
	test_snp_at();
	test_search_at();
	test_search();

	LOGINFO(FILE_LOGGER, "Testing Complete");

}
