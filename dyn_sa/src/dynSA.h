/***************************************************************************
 *   DynSA - Dynamic Suffix Array                                          *
 *   Copyright (C) 2006  Wolfgang Gerlach                                  *
 *                 2008  Mikael Salson                                     *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

// ------ Dynamic Sequence with Indels -----
// uses huffman shaped wavelet tree
// space: O(n H_0) time: O((log n) H_0)
// papers: V. Maekinen, G. Navarro. Dynamic Entropy-Compressed Sequences and Full-Text
//           Indexes. CPM 2006, Chapter 3.6 
/* ------ Dynamic Suffix Array -------
 papers : M. Salson, T. Lecroq, M. L\'eonard, L. Mouchard. 
          Dynamic Burrows-Wheeler Transform.
          PSC 2008, pp. 13--25, 2008.

          M. Salson, T. Lecroq, M. L\'eonard, L. Mouchard.
          A Four-Stage Algorithm for Updating a Burrows-Wheeler Transform
          Theoretical Computer Science. To appear

          M. Salson, T. Lecroq, M. L\'eonard, L. Mouchard. 
          Dynamic Extended Suffix Arrays. 
          Journal of Discrete Algorithms. To appear
*/



#ifndef GUARD_DynSA
#define GUARD_DynSA


#include <iostream>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include <bitset>

extern "C" {
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
}
#include "types.h"

#ifndef SAMPLE
#define SAMPLE 16
#endif

#ifndef BUFFER
#define BUFFER 1048576
#endif

/* Constants used to know the current state of our structure.
 * Knowing this state is useful since a discrepancy is introduced
 * in the BWT when we are dealing with modifications.
 */
#define DSA_IDLE 0              /* We are not modifying the structure */
#define DSA_INSERTING 1         /* We are inserting char(s) */
#define DSA_DELETING 2          /* We are deleting char(s)  */
#define DSA_SUBSTITUTING 4      /* We are substituting char(s) -- not used (yet?) */
#define DSA_REORDERING 8        /* We are reordering cyclic shifts */

#include "gRankS.h"
#include "sbvtree.h"
#include "intTree.h"

namespace dynsa { class DynSA; }

#include "lcp.h"
#if SAMPLE!=0
#include "DSASampling.h"
#endif




//using dynsa::DynSA;
using namespace sbvtree;

namespace dynsa {


  //TODO in Klasse schieben
  const size_t sampleInterval = SAMPLE;


  class WaveletNode{
  public:

    WaveletNode *left;
    WaveletNode *right;
    WaveletNode *parent;
    float relativeFrequency; // used only while construction
    uchar c0;      // used also while construction
    uchar c1;
	
    SBVTree *bittree;
	
  WaveletNode(uchar c, float relativeFrequency)
    : left(NULL), right(NULL), parent(NULL), relativeFrequency(relativeFrequency), c0(c), bittree(NULL){}

  WaveletNode(WaveletNode *left, WaveletNode *right)
    : left(left), right(right), parent(NULL), bittree(NULL) {
      relativeFrequency = left->relativeFrequency + right->relativeFrequency;
      left->parent = this;
      right->parent = this;
    }
	
    ~WaveletNode(){
      delete bittree;
    }
	
    bool operator>(const WaveletNode &a) const {
      return (relativeFrequency > a.relativeFrequency);
    }

	
	
  };

  
  class DynSA{
  public:
    size_t nb_updates_lcp;

    /**
     * Constructs an empty dyn SA.
     * If the boolean is false we do NOT compute the LCP table.
     */
    DynSA(bool computeLCP=false);
    
    void initEmptyDynSA(const float *f); //argument: array length 256 containing relative frequencies of characters 
    
    /**
     * Adds the string str of length n at position pos in the original text.
     */
    size_t addChars(uchar *str, size_t n, size_t pos);
    /**
     * Deletes n chars starting at position pos in the text.
     */
    size_t deleteChars(size_t n, size_t pos);

    size_t addTextFromSA(const size_t *SA, uchar *str, size_t n);
    size_t addText(uchar *str, size_t n); // length n of text must include "\0"!
    size_t addTextFromFile(char* filename, size_t n); //data is read directly from disk using only BUFFER(macro) bytes
    uchar* retrieveText();

    /**
     * If T is the text, retrieves T[pos..length+pos-1]
     */
    uchar *retrieveTextFactor(ulong pos, ulong length);
		
    size_t count(const uchar *pattern); // only counting, no locate!
    size_t* locate(const uchar *pattern); // needs macro SAMPLE > 0! (default)
    // returns array: [length,{position,}^length]
		
    pair<size_t, size_t>* backwardssearch(const uchar *pattern);
    // returns pair: (lower bound, upper bound)
    pair<size_t, size_t>* SAlookup(size_t j); // needs macro SAMPLE > 0! (default)

    //LF(i)-mapping: C[L[i]]+rank_L[i](L,i)
    size_t LFmapping(size_t i);

    /**
     * Inverse of the LF-mapping: LFmapping(invLF(i)) == i
     */
    size_t invLF(size_t i);
/*     size_t oldLFmapping(size_t i); */

    /**
     * returns ISA[pos]
     */
    size_t getISA(size_t pos);
		
    /**
     * returns the whole BWT
     */
    uchar*   getBWT();
    ostream& getBWTStream(ostream& stream);

    /**
     * returns LCP[pos]
     */
    ulong getLCP(ulong pos);
    /**
     * returns SA[pos] 
     */
    ulong getSA(ulong pos);

    /**
     * Returns the current state (DSA_INSERTING,DSA_DELETING,DSA_SUBSTITUTING,
     * DSA_REORDERING,DSA_IDLE)
     */
    int getAction();
		
    size_t getNumberOfSymbolsSmallerThan(uchar c);
    /**
     * Inverse of getNumberOfSymbolsSmallerThan
     * Returns the rank of C[pos].
     * The letter c is passed by reference so that we can get it
     * in the calling function.
     */
    size_t getRankInF(size_t pos, uchar &c);

    /* Operations on the BWT */

    /**
     * returns L[i]
     */
    uchar operator[](size_t i);
    
    /**
     * returns rank_c(L,i)
     */
    size_t rank(uchar c, size_t i);

    /**
     * returns select_c(L,i)
     */
    size_t select(uchar c, size_t i);

    /**
     * inserts c in L at position i
     */
    void insert(uchar c, size_t i);

    /**
     * deletes symbol at position i in L
     */
    void deleteSymbol(size_t i);

    /**
     * returns the length of L
     */
    size_t getSize();

    ullong getTotalChrono();
    void resetChrono();
		
    ~DynSA();

  public:
    size_t C[256+256];
    ullong total_resampling_time;
    ullong start_time;
    DynRankS *L;

    int action;                 /* one of the state DSA_*  */
    uchar letter_substituted;   /* the letter which is replaced
                                 * in L during stage Ib of our algorithm */
    uchar new_letter_L;         /*  the letter we are to insert in L */
    size_t position_mod_bwt;    /* the position where the modification 
                                 * takes place in the BWT */
    size_t previous_cs;         /* the position of the previous cyclic shift
                                 * (meaning the 
                                 * cyclic shift that starts at the position
                                 * on the left) */
    size_t pos_first_modif;     /* position where the first modification
                                 * in the BWT took place*/
    size_t  current_modif;      /* number of modification remaining
                                 * (not used in deletions)*/

#if SAMPLE!=0
    DSASampling *sample;        /* sample of ISA and SA */
#endif		
				

    LCP *lcp;

    void startChrono();
    void endChrono();

    // private functions

    /* performs stage Ib */
    size_t substitutionIb(size_t pos, uchar c, size_t &rank);

    /* reorders cyclic shifts until the are all well-ordered.
     * returns the number of cyclic shift reordered. */
    size_t reorderCS();

    // small help functions

    static double log2(double x){
      return (log10(x) / log10((double)2));
    }
		
    static int binaryTree_parent(int i){
      return (int)floor((double)i/2);
    }

    static int binaryTree_left(int i){
      return 2*i;
    }

    static int binaryTree_right(int i){
      return 2*i + 1;
    }

    static bool binaryTree_isLeftChild(int i){
      return (i%2==(int)0);
    }

    static bool binaryTree_isRightChild(int i){
      return (i%2==(int)1);
    }
		
  };

} //namespace




#endif

