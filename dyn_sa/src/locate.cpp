#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
using namespace std;

extern "C" {
#include "time.h"
#include "sys/time.h"
}

#include "dynSA.h"

#ifndef uchar
#define uchar unsigned char
#endif
#ifndef ulong
#define ulong unsigned long
#endif
#ifndef ullong
#define ullong unsigned long long
#endif

// Index functions
extern "C" {

  double getTime (void)
  {
    
    struct timeval time;
    
    if (gettimeofday(&time,NULL) == -1) {
      return 0;
    }
    return time.tv_sec*1000000+time.tv_usec;
  }
}

int main(int argc, char **argv) {
  double chrono_start;

  int n;
  ifstream file;
  uchar *chars;
  char *input_file;
  char *pattern_file;
  char *string;
  uchar **pattern_tab;
  DynSA* dsa;
  ulong length_pattern, nb_patterns, pos_first_pattern_length;
  float *f;
  size_t *occs_l;

  if (argc < 5) {
    cerr << "Usage: " << argv[0] << " text_file pattern_file nb_patterns length_patterns..." << endl << endl << "  patterns must NOT be separated by a new line." << endl << "  The maximal pattern length must be set first." << endl;
    exit(1);
  }
    
  input_file = argv[1];
  pattern_file = argv[2];
  nb_patterns = atol(argv[3]);
  length_pattern = atol(argv[4]);
  pos_first_pattern_length = 4;

  // Loading patterns
  string = new char[nb_patterns*length_pattern+1];
  FILE *patterns = fopen(pattern_file, "r");
  if (patterns) {
    if (! fread(string, sizeof(char), nb_patterns*length_pattern, patterns)) {
      cerr << "Read error!" << endl;
      exit(3);
    }
  } else {
    cerr << "Unable to open pattern file" << endl;
    exit(3);
  }


  // Loading text
  file.open(input_file, ios::in|ios::binary|ios::ate);
  if (file.is_open()) {
    n = file.tellg();
    chars = new uchar[n+1];
    file.seekg(0,ios::beg);
    file.read((char *)chars,n);
    file.close();
  } else {
    cerr << "Can't open text_file" << endl;
    exit(1);
  }
  chars[n]=0;

  // Creating DSA
  dsa = new DynSA;
  f = DynRankS::createCharDistribution(chars, (n+1), 1);
  dsa->initEmptyDynSA(f);
  dsa->addText(chars, n);
  
  delete [] chars;

  pattern_tab = new uchar*[nb_patterns];
  for (int k=pos_first_pattern_length; k < argc; k++) {
    length_pattern = atol(argv[k]);

    for (ulong i = 0; i < nb_patterns; i++) {
      pattern_tab[i] = new uchar[length_pattern+1];
      strncpy((char *)pattern_tab[i], &string[i*length_pattern], length_pattern);
      pattern_tab[i][length_pattern] = 0;
    }

    cout << "Pattern length: " << length_pattern << endl;
    chrono_start = getTime();
    for (ulong i = 0; i < nb_patterns; i++) {
      occs_l = dsa->locate(pattern_tab[i]);
      delete [] occs_l;
    }
    chrono_start = getTime()-chrono_start;
    cout << "DFMI : " << chrono_start/(nb_patterns*1000.0) << " ms. per pattern" << endl;
    for (ulong i = 0; i < nb_patterns; i++) {
      delete [] pattern_tab[i];
    }
  }
  delete [] pattern_tab;
  delete [] string;
  delete dsa;
}
