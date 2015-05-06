/***************************************************************************
 *   DynSA - Dynamic Suffix Array                                          *
 *   Copyright (C) 2006  Wolfgang Gerlach                                  *
 *                2008  Mikael Salson                                     *
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



#include <iostream>
#include <cstdlib>
#include <fstream>
#include <stack>
#include <queue>
#include <functional>
#include <algorithm>

#include "gRankS.h"





DynRankS::~DynRankS(){
  empty();
}

void DynRankS::empty(){
  deleteDynRankSNodes(root);
  delete[] leaves;
}


void DynRankS::deleteDynRankSNodes(WaveletNode *n){
  if (n->right) deleteDynRankSNodes(n->right);
  if (n->left) deleteDynRankSNodes(n->left);

  delete n;
}


void DynRankS::iterateReset(){
  iterate = 1;
  recursiveIterateResetWaveletNode(root);
}

void DynRankS::recursiveIterateResetWaveletNode(WaveletNode *w){
  w->bittree->iterateReset();
	
  if (w->left) recursiveIterateResetWaveletNode(w->left);
  if (w->right) recursiveIterateResetWaveletNode(w->right);
}


bool DynRankS::iterateNext(){
  iterate++;
  return !(iterate > getSize());
}

uchar DynRankS::iterateGetSymbol(){

  size_t i = iterate;
  WaveletNode *walk = root;	
  bool bit;
		
  while (true) {
		
    bit = walk->bittree->iterateGetBit(); // TODO improve
    i=walk->bittree->iterateGetRank(bit);
		
    walk->bittree->iterateNext();
		
		
    if (bit) { //bit = 1
      if (walk->right == 0) return walk->c1;
      walk=walk->right;
    } else { // bit = 0
      if (walk->left == 0) return walk->c0;
      walk=walk->left;
    }
		
		
  } // end of while
	
}


void DynRankS::deleteLeaves(WaveletNode *node){
  bool leaf = true;

  if (node->left) {
    // internal node
    leaf = false;
    deleteLeaves(node->left);
	
  }
  if (node->right){
    leaf = false;
    deleteLeaves(node->right);
  } 
	
  if (leaf) {
    // is a leaf, delete it!
    if (node->parent) {
      if (node==node->parent->left) node->parent->left=0;
      else node->parent->right=0;
    }
    delete node;
  }
}

void DynRankS::makeCodes(size_t code, int bits, WaveletNode *node){
  if (node->left) {
    makeCodes(code | (0 << bits), bits+1, node->left);
    makeCodes(code | (1 << bits), bits+1, node->right);
  } else {
    codes[node->c0] = code;
    codelengths[node->c0] = bits+1;
  }
}

void DynRankS::appendBVTrees(WaveletNode *node){
  node->bittree = new SBVTree();

  if (node->left) appendBVTrees(node->left);
  if (node->right) appendBVTrees(node->right);
}


void DynRankS::initEmptyDynRankS(const float *f){
  // pointers to the leaves for select
  leaves = (WaveletNode**) new WaveletNode*[256];

  for(int j=0; j<256; j++) leaves[j]=0; 

  for(int j=0; j<256; j++) { //all possible characters
    if (f[j]!=0) { //only those that exist
      leaves[j] = new WaveletNode((uchar)j, f[j]); 
    }
  }
	
  // Huffman shape, Veli's approach:
  priority_queue< WaveletNode*, vector<WaveletNode*>, greater<WaveletNode*> > q;
  for(int j=0; j<256; j++){
    if (leaves[j]!=0) {
      q.push( (leaves[j]) );
    }
    codes[j] = 0;
    codelengths[j] = 0;
  }
	
  // creates huffman shape:
  while (q.size() > 1) {
		
    WaveletNode *left = q.top();
    q.pop();
		
    WaveletNode *right = q.top();
    q.pop();
		
    q.push(  new WaveletNode(left, right) );
  }	
	

  root = q.top();
  q.pop();
	
			
  makeCodes(0,0, root);	// writes codes and codelengths

	
  // merge leaves	(one leaf represent two characters!)
  for(int j=0; j<256; j++){
	
    if (leaves[j]) {
		
      if (leaves[j]->parent->left==leaves[j]) {
	leaves[j]->parent->c0=j;
      } else {
	leaves[j]->parent->c1=j;
      }
      leaves[j]=leaves[j]->parent; // merge
    }
  }

	
  deleteLeaves(root);
	
  appendBVTrees(root);
	
}



void DynRankS::insert(uchar c, size_t i){
  size_t level = 0;
  size_t code = codes[c];

  bool bit;
  WaveletNode *walk = root;	
		
  while (walk) {
		
    bit = ((code & (1u << level)) != 0); 
		
    walk->bittree->insertBit(bit,i); // TODO improve
    i=walk->bittree->rank(bit, i);

    if (bit) { //bit = 1
      walk=walk->right;
    } else { // bit = 0
      walk=walk->left;
    }
		
    level++;		
  } // end of while
}

uchar DynRankS::deleteSymbol(size_t i){
  WaveletNode *walk = root;	
  bool bit;
  uchar c;
  while (true) {

    walk->bittree->deleteBit(i);
    bit=walk->bittree->getLastDeletedBit();
    i=walk->bittree->getLastDeletedRank();
		
    if (bit) { //bit = 1
      if (walk->right == 0) {
	c = walk->c1;
	break;
      }
      walk=walk->right;
    } else { // bit = 0
      if (walk->left == 0) {
	c = walk->c0;
	break;
      }
      walk=walk->left;
    }
		
		
  } // end of while
  return c;
}


uchar DynRankS::operator[](size_t i){
  WaveletNode *walk = root;	
  bool bit;
		
  while (true) {
		
    bit = (*walk->bittree)[i]; //TODO improve by reducing
    i=walk->bittree->rank(bit, i);

    if (bit) { //bit = 1
      if (walk->right == 0) return walk->c1;
      walk=walk->right;
    } else { // bit = 0
      if (walk->left == 0) return walk->c0;
      walk=walk->left;
    }
  } // end of while
}

size_t DynRankS::rank(uchar c, size_t i){
  if (i==0) return 0;

  size_t level = 0;
  size_t code = codes[c];
	
	
  bool bit;
  WaveletNode *walk = root;	

  while (i > 0) {
    bit = ((code & (1u << level)) != 0);
		
    i=walk->bittree->rank(bit, i);
    if (bit) { //bit = 1
      if (walk->right == 0) return i;
      walk=walk->right;
    } else { // bit = 0
      if (walk->left == 0) return i;
      walk=walk->left;
    }
	
    level++;		
  } // end of while
  return 0;
	
}

size_t DynRankS::select(uchar c, size_t i){
	
  WaveletNode *walk = leaves[c];	
	
  bool bit = (walk->c1==c);
	
  while (walk->parent) {
    i=walk->bittree->select(bit, i);
		
    bit = (walk == walk->parent->right);
    walk=walk->parent;
  } // end of while
	
  i=walk->bittree->select(bit, i);

  return i;
}

size_t DynRankS::getSize() {
  return root->bittree->getLength();
}


float* DynRankS::createUniformCharDistribution(const uchar *sampleText, size_t expectedTotalLength, size_t expectedNumberOfTexts){
  bool count[256];
  count['\0'] = true;
  for (int i=1; i<256; i++) count[i] = false;

  size_t i=0;
  size_t sampleLength;

  while (sampleText[i] != '\0') {
    count[sampleText[i]] = true;
    i++;
  }

  sampleLength=i;

	

  int alphabetSize = 0;
	
  for (int i=0; i<256; i++) if (count[i]) alphabetSize++;
	

  float *f = new float[256];
  for (int i=0; i<256; i++) f[i]=0;
	

  for (int i=0; i<256; i++) {
    if (count[i]) f[i] = 1/(double)alphabetSize; 
#ifndef NDEBUG	
    if ((i >= 39) && (i<=122)) cout << (char)i << ": " << f[i] << endl;
    else cout << i << ": " << f[i] << endl;
#endif		

  }


#ifndef NDEBUG
  float sum = 0;
  for (int i=0; i<256; i++) {
    if (f[i] < 0) {
      cerr << "createUniformCharDistribution: f[i] must not be negative! i=" << i << endl;
      exit(0);
    }

    sum += f[i];
  }
  if ((sum < 0.95) || (sum > 1.05)) {
    cerr << "createUniformCharDistribution: sum != 1: " << sum << endl;
    exit(0);
  } else  cout << "createUniformCharDistribution, ok: uniform sum= " << sum << endl;
#endif


  return f;
}

float* DynRankS::createCharDistribution(const uchar *sampleText, size_t expectedTotalLength, size_t expectedNumberOfTexts){
  size_t count[256];

  for (int i=0; i<256; i++) count[i] = 0;

  size_t i=0;
  size_t sampleLength;

  while (sampleText[i] != '\0') {
    count[sampleText[i]]++;
    i++;
  }

  //for (int i=0; i<256; i++) cout << count[i] << endl;

  sampleLength=i;

  for (int i=1; i<256; i++) {
    if (count[i] != 0) count[i] = (size_t)(count[i]*(expectedTotalLength/(double)sampleLength));  // scale
  }
  count['\0'] = expectedNumberOfTexts;

  float *f = new float[256];

  //sampleLength += count['\0'];

  for (int i=0; i<256; i++) {
    if (count[i]) {
      f[i] = (float)(count[i]/(double)(expectedTotalLength + expectedNumberOfTexts));
    }
    else {
      f[i]=0;
    }
  }


  return f;
}


