#include <iostream>
#include <cstdlib>

#include <fstream>

#include "dynSA.h"
#include "utils.h"
#include <string.h>
#include "types.h"
using namespace std;
using namespace dynsa;

extern "C" {
#include "time.h"
#include "sys/time.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
}


uchar *getFileContent(char *filename, size_t &n, int termin) {
  std::ifstream file(filename);
  if (!file)
    {
      cerr << "error reading file: " << filename << endl;
      exit(EXIT_FAILURE);
    }

  file.seekg(0, ios::end);
  n = file.tellg();
  if (termin)
    n++;
  file.seekg(0, ios::beg);

  uchar *text = new uchar[n+1];

  char c;
  size_t i=0;

  while (file.get(c))
    {
      text[i]=c;//(c=='\n'||c=='\r')?'X':c;
      i++;
    }
  file.close();

  if (termin)
    text[n-1]=0;
  text[n]=0;
  return text;
}

void usage(char *program) {
    cerr << "Usage : " << program << " <filename> [lcp] <sliding window length> <nb shift>" << endl;
    exit(1);
}

int main(int argc, char *argv[]) {
  ullong time1;
  bool computes_lcp = false;
  size_t n;
  uchar *text;
  uchar *expectedtext;
  DynSA *wt;
  float *f;
  size_t window_length;
  size_t nb_shift;

  if (argc < 4) {
    usage(argv[0]);
  }

  if (argc == 5) {
    if (strcmp(argv[2],"lcp") == 0) 
      computes_lcp = true;
    window_length = atol(argv[3]);
    nb_shift = atol(argv[4]);
  } else {
    
    window_length = atol(argv[2]);
    nb_shift = atol(argv[3]);
  }

  text = getFileContent(argv[1],n,1);

  // Creates the structure for dynamic suffix array
  wt = new DynSA(computes_lcp);
  // Initialises for a text of length AT MOST 10,000,000
  f = DynRankS::createCharDistribution(text, 10000000, 1);
  wt->initEmptyDynSA(f);

  time1 = getChrono();
  expectedtext = new uchar[window_length+1];
  strncpy((char *)expectedtext,(char *)text,window_length);
  expectedtext[window_length]=0;
  cout << "Creating index" << endl;
  wt->addText(expectedtext,window_length+1);
  if (strncmp((char *)text,(char *)wt->retrieveText(), window_length)!= 0) {
    cout << "erreur" << endl;
    exit(1);
  }
  cout << "Sliding... " << endl;
  for (ulong i=window_length; i < n-nb_shift; i+=nb_shift) {

    if (i % 500 == 0)
      cout << i << endl;
    wt->deleteChars(nb_shift,1);
    wt->addChars(&text[i], nb_shift, window_length);

//     if (strncmp((char *)&text[i-window_length+nb_shift],(char *)wt->retrieveText(), window_length)!= 0) {
//       cout << "erreur" << endl;
//       exit(1);
//     }
//     if (strncmp((char *)wt->retrieveText(),(char *)&text[i-window_length+1],window_length) != 0) {
      
//       cout << wt->retrieveText() << endl << strncpy((char *)expectedtext,(char *)&text[i-window_length+1],window_length) << endl;
//       exit(1);
//     }
  }
  time1 = getChrono()-time1;

  cout << "Index built in " << time1/1000000.0 << " s." << endl;


  time1 = getChrono();
  ulong nb_tests = 10000;
	srand(time(NULL));
  for (ulong i=0; i < nb_tests; i++) {
    wt->getSA((rand()%window_length)+1);
  }
  time1 = getChrono()-time1;
  cout << "Access time: " << time1*1. / nb_tests << endl;

  cerr << "Sample density: " << window_length*1./wt->sample->pi->getSize() << endl;

  uchar *newtext = new uchar[window_length];
  ulong pos1, pos2, new_LCP, current_LCP;
  bool errors = false;
  newtext = wt->retrieveText();
  ulong SA_1 = wt->getSA(1), SA_2;
  ulong min;
  ulong length = 100;
  char output[length+1];
  output[length] = 0;
  int result;
  for (ulong i=1; i < window_length; i++) {
    SA_2 = wt->getSA(i+1);
    min = (SA_1 < SA_2) ? (window_length-SA_2+1) : (window_length-SA_1+1);
    result = strncmp((char *)&newtext[SA_1-1],(char *) &newtext[SA_2-1],min);
    if (result > 0 || (result == 0 && SA_1 < SA_2)) {
      cout << "Erreur SA : " << SA_1-1 << " " << SA_2-1 << " result = " << result << ", min = " << min <<  endl;
      output[min] = 0;
      cout << strncpy(output, (char *)&newtext[SA_1-1], min) << endl;
      cout << strncpy(output, (char *)&newtext[SA_2-1], min) << endl << endl;
    } 
    SA_1 = SA_2;
  }

  // Tests if the LCP is correct
  if (computes_lcp) {
    pos1 = wt->getSA(1);
    for (ulong i=0; i < window_length-1; i++) {
      pos2 = wt->getSA(i+2);
      new_LCP = 0;
      while (newtext[pos1+new_LCP-1] == newtext[pos2+new_LCP-1])
        new_LCP++;
      current_LCP = wt->getLCP(i+1);
      if (new_LCP != current_LCP) {
        cout << "Error pos. " << i+1 << " (" <<new_LCP << " " << current_LCP <<")  SA["<<i+1<<"]="<<wt->getSA(i+1) << ", SA["<<i+2<<"]="<<wt->getSA(i+2) << endl;
        cout << wt->retrieveTextFactor(wt->getSA(i+1), new_LCP+1) << endl 
             << wt->retrieveTextFactor(wt->getSA(i+2), new_LCP+1) << endl;
        errors = true;
      }
      pos1 = pos2;
    }
    cout << endl;
  }

  if (errors)
    exit(4);

  delete [] text;
  delete [] f;
  delete [] expectedtext;

//   cout << "Nombre updates LCP : " << wt->nb_updates_lcp<<endl;
//   cout << "Nombre noeuds dans L : " << wt->L->nbNodes()<< endl;
//   cout << "juste comme ça... : " << wt->L->root->bittree->getNbNodes((BVNode *)wt->L->root->bittree->root) << " noeuds pour le BV de la racine du WT"<< endl;
  delete wt;

  exit(0);
}
