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
    cerr << "Usage : " << program << " <filename> [lcp] [getSA <num_queries>] [insert (<file to ins.> <pos of ins.>)+] [delete (<pos. to delete> <length of deletion>)+] " << endl;
    exit(1);
}

int main(int argc, char *argv[]) {
  ullong time1;
  bool computes_lcp = false;
  size_t *ins_indexes,*del_indexes,*length_del,*length_patterns,n;
  uchar **patterns;
#ifndef NO_CHECK
  uchar *expectedtext;
  uchar *newtext;
#endif
  uchar *text;
  DynSA *wt;
  float *f;
  size_t nb_ins=0,nb_del=0,original_length;
  int mode_ins = 0,mode_del = 0, pos_deb_ins=0, pos_deb_del=0;
  size_t total_length_ins = 0, total_length_del = 0;
  bool getSA = false;
  size_t nb_getSA=0;

  if (argc < 2) {
    usage(argv[0]);
  }

  for (int i = 2; i<argc; i++) {
    if (strcmp(argv[i],"lcp") == 0) {
      computes_lcp = true;
    } else if (strcmp(argv[i], "getSA") == 0) {
      getSA = true;
      if (i+1 >= argc) 
        usage(argv[0]);
      i++;
      nb_getSA = atol(argv[i]);
    } else if (strcmp(argv[i],"insert") == 0) {
      if (mode_ins == 1 || mode_del == 1)
	usage(argv[0]);
      mode_ins = 1;
      pos_deb_ins = i+1;
    } else if (strcmp(argv[i], "delete") == 0) {
      if (mode_ins == 1)
	mode_ins = 2;
      if (mode_del == 1)
	usage(argv[0]);
      mode_del = 1;
      pos_deb_del = i+1;
    } else if (mode_ins == 1) {
      nb_ins++;
      if (i+1 >= argc)
	usage(argv[0]);
      i++;
    } else if (mode_del == 1) {
      nb_del++;
      if (i+1 >= argc)
	usage(argv[0]);
      i++;
    }
  }

//   cout << nb_ins << " insertions and " << nb_del << " deletions" << endl;
  ins_indexes = new size_t[nb_ins];
  length_patterns = new size_t[nb_ins];
  patterns = (uchar **)malloc(nb_ins*sizeof(uchar *));
  for (size_t i =0; i < nb_ins; i++) {
    ins_indexes[i] = atol(argv[pos_deb_ins+1+2*i]);
    patterns[i] = getFileContent(argv[pos_deb_ins+2*i],length_patterns[i],0);
//     cout << "Insert " << patterns[i] << " at pos. " << ins_indexes[i] << endl;
  }
  del_indexes = new size_t[nb_del];
  length_del = new size_t[nb_del];
  for (size_t i = 0; i < nb_del; i++) {
    del_indexes[i] = atol(argv[pos_deb_del+2*i]);
    length_del[i] = atol(argv[pos_deb_del+1+2*i]);
//     cout << "Delete from " << del_indexes[i] << " to " << del_indexes[i]+length_del[i]-1 << endl;
  }

  text = getFileContent(argv[1],n,1);
  original_length = n;

  // Creates the structure for dynamic suffix array
  wt = new DynSA(computes_lcp);
  f = DynRankS::createCharDistribution(text, (n > 10000000) ? n+1 : 10000000, 1);
  wt->initEmptyDynSA(f);
  time1 = getChrono();
  wt->addText(text,n);
  time1 = getChrono()-time1;
  cerr << "Index built in " << time1/1000000.0 << " s." << endl;


  time1 = getChrono();

  // Inserts the patterns
  for (size_t i=0; i < nb_ins; i++) {
   wt->addChars(patterns[i], length_patterns[i], ins_indexes[i]+1);
   total_length_ins += length_patterns[i];
  }

  // Delete the patterns
  for (size_t i = 0 ; i < nb_del; i++) {
    wt->deleteChars(length_del[i], del_indexes[i]+1);
    total_length_del += length_del[i];
  }

  time1 = getChrono()-time1;

  // Prints the total time for inserting + deleting
  cerr << "Modifications done in " << time1/1000000.0 << " s." << endl;

  n = wt->getSize();

  if (getSA) {
    srandom(time(NULL));
    time1 = getChrono();
    for (uint i=0; i < nb_getSA; i++)
      wt->getSA((random()%(n-1))+1);
    time1 = getChrono()-time1;
    // Prints the total time for querying SA
    cerr << "getSA done in " << time1/(nb_getSA*1000.0) << " ms." << endl;

  }
  
#ifndef NO_CHECK
  size_t i = 1;
  char c = (*wt)[i];
  size_t length = 0;
  newtext = new uchar[n];
  newtext[n-1]='\0';
  // retrieves the text back
  while (c != 0) {
    c = (*wt)[i];
    newtext[n-length-2]=c;
    i = wt->LFmapping(i);
    length++;
    if (length > n)
      break;
  }
  if (n != length) {
    // Clearly, if length is different from the expected length,
    // we have a big problem!
    cerr << "n = " << n << ", length = " << length << endl;
    exit(2);
  } else {
    // Computes what the final text should be

    expectedtext = new uchar[original_length+total_length_ins];
    strcpy((char *)expectedtext,(char *)text);
    size_t current_end = original_length-1;
    for (size_t i=0; i < nb_ins; i++) {
      memmove(&expectedtext[ins_indexes[i]+length_patterns[i]]
	      ,&expectedtext[ins_indexes[i]]
	      ,current_end-ins_indexes[i]+1);
      current_end += length_patterns[i];
      strncpy((char *)&expectedtext[ins_indexes[i]],(char *) patterns[i],length_patterns[i]);
    }
    for (size_t i=0; i < nb_del; i++) {
      memmove(&expectedtext[del_indexes[i]]
	      ,&expectedtext[del_indexes[i]+length_del[i]]
	      ,current_end-(del_indexes[i]+length_del[i])+1);
      current_end -= length_del[i];
    }
    // Compares whether the actual text corresponds to the text retrieved from
    // our structure.
    if (strcmp((char *) expectedtext, (char *)newtext) != 0) {
       cout << expectedtext << endl << newtext << endl;
      exit(3);
    }
  }
  ulong current_LCP, pos1, pos2, new_LCP;
  bool errors = false;
  // Tests if the LCP is correct
  if (computes_lcp) {
    pos1 = wt->getSA(1);
    for (ulong i=0; i < n-1; i++) {
      pos2 = wt->getSA(i+2);
      new_LCP = 0;
      while (newtext[pos1+new_LCP-1] == newtext[pos2+new_LCP-1])
        new_LCP++;
      current_LCP = wt->getLCP(i+1);
      if (new_LCP != current_LCP) {
        cout << "Error pos. " << i << " (" <<new_LCP << " " << current_LCP <<")" << endl;
        errors = true;
      }
      pos1 = pos2;
    }
    cout << endl;
  }
  if (errors)
    exit(4);

#endif


  delete [] text;
  delete [] f;

//   wt->getBWTStream(cout);

  for (size_t i = 0; i < nb_ins; i++)
    delete [] patterns[i];
  delete [] ins_indexes;
  delete [] del_indexes;
  delete [] length_del;
  delete [] length_patterns;
  free(patterns);
#ifndef NO_CHECK
  delete [] newtext;
  delete [] expectedtext;
#endif


//   cout << "Nombre updates LCP : " << wt->nb_updates_lcp<<endl;
#if SAMPLE!=0
  cerr << "Sample density: " << n*1./wt->sample->pi->getSize() << endl;
#endif
//   cout << "Nombre noeuds dans L : " << wt->L->nbNodes()<< endl;
//   cout << "juste comme ça... : " << wt->L->root->bittree->getNbNodes((BVNode *)wt->L->root->bittree->root) << " noeuds pour le BV de la racine du WT"<< endl;
  delete wt;
  
  cout << "Done..." << endl;
//   cin >> c;

  exit(0);
}
