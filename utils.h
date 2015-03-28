#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>

void print_time_elapsed(std::string, struct timeval*, struct timeval*);
void ignore_first_line();
bool is_valid_input(long,long,long);
std::string generate_random_string(long len);
std::list<long> find_substr(std::string, std::string);
std::vector<std::pair<long,char>> generateRandomInserts(long);

#endif