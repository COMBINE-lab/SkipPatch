#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <bits/stdc++.h>
#include "utils.h"

void print_time_elapsed(std::string, struct timeval*, struct timeval*);

void ignore_first_line();

bool is_valid_input(long,long,long);

std::string generate_random_string(long len);

std::vector<long> find_substr(std::string, std::string);

std::vector<std::pair<long,char>> generate_random_inserts(int,long);

#endif