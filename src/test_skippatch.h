#ifndef TEST_PROB_H
#define TEST_PROB_H

#include "genome.h"
#include "utils.h"
#include "test.h"

void test_edits_naive(genome&, std::string, long);

void test_search_naive(genome&, std::string, long, long, long);

void test_hash(genome&, std::string);

void test_substr_naive(genome&, std::string, std::string, long);

#endif
