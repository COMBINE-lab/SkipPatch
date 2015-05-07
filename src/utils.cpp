#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "utils.h"

std::string alphabet="AGCT"; //#used for generating random string

/*Print time elapsed in seconds*/
void print_time_elapsed(std::string desc, struct timeval* start, struct timeval* end) {
    
    /*
    struct timeval {
        time_t      tv_sec;
        suseconds_t tv_usec;      
    }*/
    struct timeval elapsed;
    
    if(start->tv_usec > end->tv_usec) {
        end->tv_usec += 1000000;
        end->tv_sec--;
    }
    elapsed.tv_usec = end->tv_usec - start->tv_usec;
    elapsed.tv_sec  = end->tv_sec  - start->tv_sec;
    float time_elapsed = (elapsed.tv_sec*1000000 + elapsed.tv_usec)/1000000.f;
    std::cout << desc << " Total Time Elapsed = " << time_elapsed << " seconds" <<std::endl;

    return;
}

/* Skips reading one like of the input */
void ignore_first_line()
{
    std::string input;
    getline(std::cin,input);
}

/* Checks if the input is valid */
bool is_valid_input(long pos,long len,long max_len)
{
    if(pos<0)
    {
        std::cout<<"Cant insert at a negative position"<<std::endl;
        return false;
    }
    if(len<=0)
    {
        std::cout<<"Min len = 1"<<std::endl;
        return false;
    }
    if((pos+len-1)>=max_len) 
    {
        std::cout<<"Insertion out of bounds!"<<std::endl;
        return false;
    }
    return true;
}

/* Generates a random string (DNA: ATCG) of the given length */
std::string generate_random_string(long len){


    std::srand(unsigned ( std::time(0) )); //use current time as seed for random generator
    std::string random_str;
    for(long i=0; i<len; i++)
    {   
        random_str+=alphabet[std::rand()%(alphabet.size())]; 
    }
    std::srand(std::time(0)); //use current time as seed for random generator
    random_shuffle(random_str.begin(),random_str.end()); //*now really random :)
    
    return random_str;
}

/* Returns the vector of positions of the 'substr' occurring in 'str' */
std::vector<long> find_substr(std::string str, std::string substr){

    std::vector<long> positions;
    long pos = str.find(substr, 0);
    while(pos != -1)
    {   
        positions.push_back(pos);
        pos = str.find(substr,pos+1);
    }

    return positions;
}

/*Generate "insert_count" random inserts of "sequence_length"*/
std::vector<std::pair<long,char>> generate_random_inserts(int insert_count, long sequence_length){
    
    std::srand(std::time(0)); //use current time as seed for random generator

    std::vector<std::pair<long,char>> random_inserts(insert_count);
    for(int i=0; i<insert_count; i++){
        long random_pos = (std::rand() % sequence_length);   
        char random_alphabet = alphabet[(std::rand() % alphabet.size())];
        random_inserts[i]=std::make_pair(random_pos,random_alphabet);
    }   

    return random_inserts;
}   
