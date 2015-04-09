#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "utils.h"

using namespace std;

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
  string input;
  getline(cin,input);
}


/* Checks if the input is valid */
bool is_valid_input(long pos,long len,long max_len)
{
  if(pos<0)
  {
    cout<<"Cant insert at a negative position"<<endl;
    return false;
  }
  if(len<=0)
  {
    cout<<"Min len = 1"<<endl;
    return false;
  }
  if((pos+len-1)>=max_len) 
  {
    cout<<"Insertion out of bounds!"<<endl;
    return false;
  }
  return true;
}

/* Generates a random string (DNA: ATCG) of the given length */
string generate_random_string(long len){

  string random;
  for(long i=0;i<len;i++)
  {
    random+=alphabet[i%(alphabet.size())]; 
  }
  random_shuffle(random.begin(),random.end()); //*now really random :)
  return random;
}

/* Returns the vector of positions of the 'substr' occurring in 'str' */
std::vector<long> find_substr(std::string str, std::string substr){

    vector<long> positions;
    long pos = str.find(substr, 0);
    while(pos != -1)
    {   
        positions.push_back(pos);
        pos = str.find(substr,pos+1);
    }

    return positions;
}

/*Generate random */
std::vector<std::pair<long,char>> generate_random_inserts(int insert_count, long sequence_length){
    
    char alphabets[] = {'A', 'T', 'C', 'G'};
    std::vector<char> alphabet (alphabets, alphabets+sizeof(alphabets) / sizeof(char));    
    std::vector<std::pair<long,char>> random_inserts(insert_count);

    for(int i=0; i<insert_count; i++){
        long random_pos = (std::rand() % sequence_length);   
        int random_alphabet = (std::rand() % alphabet.size());  
        random_inserts[i]=std::make_pair(random_pos,alphabet[random_alphabet]);
    }   

    return random_inserts;
}   