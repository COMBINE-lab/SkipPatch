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


// Mikael: I put the code specific to this structure in a different class
//         so that both this class and dynSA class are easier to read.

#ifndef GRANK_S_H
#define GRANK_S_H


#include <iostream>
#include <cstdlib>
#include <math.h>

#include "types.h"
#include "sbvtree.h"


using namespace sbvtree;
using namespace std;

using  sbvtree::SBVTree;
using sbvtree::SBVNode;

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
   : left(0), right(0), parent(0), relativeFrequency(relativeFrequency), c0(c), bittree(0){}
  
 WaveletNode(WaveletNode *left, WaveletNode *right)
   : left(left), right(right), parent(0), bittree(0) {
    relativeFrequency = left->relativeFrequency + right->relativeFrequency;
    left->parent = this;
    right->parent = this;
  }

  size_t countNodes() {
    size_t nb_nodes = 0;
    if (left != NULL)
      nb_nodes += left->countNodes();
    if (right != NULL)
      nb_nodes += right->countNodes();
    if (bittree != NULL && bittree->root != NULL)
      nb_nodes += bittree->getNbNodes((SBVNode *)bittree->root);
    return nb_nodes;
  }
  
  ~WaveletNode(){
    delete bittree;
  }
  
  bool operator>(const WaveletNode &a) const {
    return (relativeFrequency > a.relativeFrequency);
  }
  
  
};

  
class DynRankS{
 public:
  void initEmptyDynRankS(const float *f); //argument: array length 256 containing relative frequencies of characters
  void empty();
		
		
		
		
  /**
     You can use these functions to compute the relative character frequencies.
     Example:  createUniformCharDistribution("ACGT", 10000000, 500) - means 500 DNA-sequences
     of overall length  10000000; including separation character, probabilities are 1/5.
  **/
  static float* createUniformCharDistribution(const uchar *sampleText, size_t expectedTotalLength, size_t expectedNumberOfTexts); //for equal length encoding of characters
  static float* createCharDistribution(const uchar *sampleText, size_t expectedTotalLength, size_t expectedNumberOfTexts); //for variable length encoding of characters
		
  uchar operator[](size_t i);
		
  size_t rank(uchar c, size_t i);
  size_t select(uchar c, size_t i);
	
  void insert(uchar c, size_t i);
  uchar deleteSymbol(size_t i);

  size_t getSize();

  ~DynRankS();

  size_t nbNodes() {
    return root->countNodes();
  }

  //Iterator
  void iterateReset();
  bool iterateNext();
  uchar iterateGetSymbol();
  void recursiveIterateResetWaveletNode(WaveletNode *w);

 public:
  // private:
  WaveletNode *root;    // root of the wavelet tree
  WaveletNode **leaves; // needed for construction and select
		
  size_t codes[256];
  int codelengths[256];
  size_t iterate;


  // functions
  void makeCodes(size_t code, int bits, WaveletNode *node);
  void deleteLeaves(WaveletNode *node);
  void appendBVTrees(WaveletNode *node);
		
  void deleteDynRankSNodes(WaveletNode *n);


};

namespace std
{

  template<> struct greater<WaveletNode*>
    {
      bool operator()(WaveletNode const* p1, WaveletNode const* p2)
      {
	if(!p1)
	  return false;
	if(!p2)
	  return true;
	return *p1 > *p2;
      }
    };
}


#endif

