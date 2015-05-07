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



#include <iostream>
#include <cstdlib>
#include <fstream>
#include <stack>
#include <queue>
#include <functional>
#include <algorithm>
#include <string.h>

#include "dynSA.h"





// -------- DynSA --------
namespace dynsa {

  DynSA::DynSA(bool computeLCP) {
    // array C needed for backwards search
    for(int j=0; j<256+256; j++) C[j] = 0;
    
#if SAMPLE!=0
    sample = new DSASampling(this,sampleInterval);
#endif
    lcp = new LCP(this, computeLCP);
    L = new DynRankS();
    action = DSA_IDLE;
  }

  DynSA::~DynSA(){
    delete lcp;
#if SAMPLE!=0
    delete sample;
#endif
#ifndef NFREE_L
    delete L;
#endif
  }

  uchar* DynSA::getBWT(){
    size_t length = L->getSize();
    length++;
    uchar *text = new uchar[length];
    bool data=true;
    uchar c;
    size_t i = 0;
	
    L->iterateReset();
	
    while (data) {
      c = L->iterateGetSymbol();	
      if (c == 0)
	text[i] = '$';
      else
	text[i] = c;
      data = L->iterateNext();
      i++;
    }
    text[length-1]=0;
	
    return text;
  }

  ostream& DynSA::getBWTStream(ostream& stream){
    bool data=true;
	
    L->iterateReset();
	
    uchar c;
    while (data) {
      c = L->iterateGetSymbol();
      if (c == 0)
        stream << '$';
      else
        stream << c ;
      data = L->iterateNext();
    }

    return stream;
  }

  void DynSA::initEmptyDynSA(const float *f){
    total_resampling_time = 0;
    L->initEmptyDynRankS(f);
    // array C needed for backwards search
    for(int j=0; j<256+256; j++) C[j] = 0;
  }

  void DynSA::insert(uchar c, size_t i){
    // If we are modifying the BWT we have to take care of the discrepancy 
    // introduced.
    if (action & (DSA_INSERTING | DSA_DELETING | DSA_SUBSTITUTING)) {
      // When we insert before the previous cyclic shift, its position
      // is shifted by one position.
      if (i <= previous_cs) {
	previous_cs++;
      }
      // Same phenomenon for the position of first modification
      if (i <= pos_first_modif)
	pos_first_modif++;
    }
    L->insert(c, i);
	
    // Updating the array C
    int j = 256+c;
    while(j>1) {
      C[j]++;
      j=binaryTree_parent(j);
    }
    C[j]++;	
  }


  void DynSA::deleteSymbol(size_t i){
    uchar c = L->deleteSymbol(i);
	
    int j = 256+c;
    while(j>1) {
      C[j]--;
      j=binaryTree_parent(j);
    }
    C[j]--;	

    // In case of modification, we have to update some extra values
    if (action & (DSA_INSERTING | DSA_DELETING | DSA_SUBSTITUTING)) {
      // When we delete a cyclic shift at position i all the cyclic shifts
      // after pos. i are shifted. We have to modify the position of the 
      // previous cyclic shift (and the position of the first modification)
      // corresdpondingly 
      if (previous_cs >= i)
	previous_cs--;
      if (pos_first_modif >= i)
	pos_first_modif--;
    }
  }


  uchar DynSA::operator[](size_t i){
    return (*L)[i];
  }

  size_t DynSA::rank(uchar c, size_t i){
    size_t result = L->rank(c,i);
    // In case of deletion, rank, after the position of the initial
    // substitution (stage Ib) for the letter deleted, has to be corrected.
    // In practice, the letter has not been substituted yet ... hence we have
    // to correct the result.
    if (action == DSA_DELETING && pos_first_modif < i
	&& letter_substituted == c )
      result--;
    return result;
  }

  size_t DynSA::select(uchar c, size_t i){
    return L->select(c,i);
  }

  size_t DynSA::getSize() {
    return L->getSize();
  }

  pair<size_t, size_t>* DynSA::backwardssearch(const uchar *pattern){

    size_t i = strlen((char*)pattern);
    size_t sp = 1;

    size_t ep = L->getSize();
    uchar s;
    while ((sp <= ep) && (i >=1)) {
      s=pattern[i-1];
      sp=getNumberOfSymbolsSmallerThan(s) + rank(s, sp-1UL) + 1UL;
      ep=getNumberOfSymbolsSmallerThan(s) + rank(s,ep);
		
      i--;
    }

    pair<size_t, size_t> *p = new pair<size_t, size_t>(sp, ep);
    return p;
  }



  size_t DynSA::count(const uchar *pattern){
    pair<size_t, size_t> *p = backwardssearch(pattern);

    size_t result = p->second - p->first +1;
    delete p;
    return result;
  }

#if SAMPLE!=0
  size_t* DynSA::locate(const uchar *pattern){

    size_t i;
    size_t sp, ep;
	

    pair<size_t, size_t> *p = backwardssearch(pattern);
    sp = p->first;
    ep = p->second;

    delete p;

    size_t numberOfMatches = ep-sp +1;
    size_t *matches = new size_t[numberOfMatches + 1];
    matches[0] = numberOfMatches;

    size_t k = 1;

    for (i=sp; i <= ep; i++) { // each match
      matches[k++] = sample->getSA(i);
    }

    return matches;
  }
#else
  size_t* DynSA::locate(const uchar *pattern){ // if SAMPLE ist turned off
    return 0;
  }
#endif


  size_t DynSA::getISA(size_t pos) {
#if SAMPLE!=0
    return sample->getISA(pos);
#else
    return 1;
#endif
  }


  
  size_t DynSA::LFmapping(size_t i) {
    // We are modifying the BWT and requesting LF for the current  position of 
    // modification? Easy! That's the value of previous_cs.
    if (action != DSA_IDLE && i == position_mod_bwt)
      return previous_cs;
    uchar s = (*L)[i];
    uint smaller = getNumberOfSymbolsSmallerThan(s), r =  rank(s,i);
    // If we are modifying the BWT
    if (action != DSA_IDLE) {
      // If we are reordering or we have inserted the last letter
      //  (that is we are going to reorder).
      if (action == DSA_REORDERING 
	  || (action == DSA_INSERTING && current_modif == 0)) {
	uchar s2 = (*L)[position_mod_bwt];
	size_t tmplf = getNumberOfSymbolsSmallerThan(s2)+rank(s2,position_mod_bwt);
	size_t result = smaller+r;
        // the result is between tmplf and previous_cs.
        // previous_cs has not moved yet hence we have to correct the result
	if (result <= tmplf && result >= previous_cs)
	  result++;
	else if (result >= tmplf && result <= previous_cs)
	  result--;
	return result;
      } else if (action == DSA_INSERTING) {
	uchar s2 = (*L)[position_mod_bwt];
	// Taking into account the substituted letter
	if (s==letter_substituted && i > pos_first_modif)
	  r++;
	// New letter in L has been inserted in F as well, but we have
	// not inserted the c.s. beginning with this letter yet
	if (s > new_letter_L)
	  smaller--;
	// This letter must not be taken into account since its previous c.s.
	// doesn't exist yet
	if (s2 == s && i > position_mod_bwt)
	  r--;
      }
    }
    return smaller+r;
  }

  size_t DynSA::invLF(size_t i) {
    uchar c;
    i = getRankInF(i, c);
    return select(c, i);
  }

  size_t DynSA::addChars(uchar *str, size_t n, size_t pos) {

    //cerr << "DynSA::addChars: " << str << " of length " << n << " at pos. " << pos << endl;

    size_t rank_store;
    size_t  oldbwt_pos;

    if (pos > getSize())
      pos = getSize();
  
    // Step Ib in our algorithm : replaces in L, at the position of T^{[i]}, 
    // the letter with str[n-1].
    position_mod_bwt = substitutionIb(pos, str[n-1], rank_store);

    new_letter_L =  str[n-1];
    action = DSA_INSERTING;

    // Computes the position of $T^{[i-1]}
    previous_cs = getNumberOfSymbolsSmallerThan(letter_substituted) + rank_store;
    if (letter_substituted > new_letter_L)
      previous_cs--;
    oldbwt_pos = position_mod_bwt;

    // Computes the position of insertion
    position_mod_bwt = getNumberOfSymbolsSmallerThan(str[n-1]) + rank(str[n-1],oldbwt_pos);

    // Step IIa : insertion of the elements.
    for (size_t k=n-1; k > 0; k--) {
      insert(str[k-1], position_mod_bwt);
      current_modif = k;
      new_letter_L = str[k-1];

#if SAMPLE!=0
      sample->insertBWT(pos, position_mod_bwt);
#endif

      if (position_mod_bwt <= oldbwt_pos)
	oldbwt_pos++;
      lcp->newSuffixAtPos(position_mod_bwt);


      // Updating the values stored with respect to the insertion position.
      oldbwt_pos = position_mod_bwt;


      position_mod_bwt = getNumberOfSymbolsSmallerThan(str[k-1]) 
	+ rank(str[k-1],position_mod_bwt);
      // Consider the case where we have computed rank on the missing letter
      if (pos_first_modif < oldbwt_pos && str[k-1] == letter_substituted)
	position_mod_bwt++;

    }

    new_letter_L = letter_substituted;
    L->insert(letter_substituted, position_mod_bwt);
    current_modif = 0;

    if (position_mod_bwt <= previous_cs) {
      previous_cs++;
    }
    if (position_mod_bwt <= pos_first_modif)
      pos_first_modif++;
    
#if SAMPLE!=0
    sample->insertBWT(pos, position_mod_bwt);
#endif
    if (position_mod_bwt <= oldbwt_pos)
      oldbwt_pos++;

    lcp->newSuffixAtPos(position_mod_bwt);

    // Step IIb : reordering step
      ulong shift = reorderCS();

    // The sample is treated now. So far we have just inserted 0s
    // in the bit vectors. Now we are adding the sample(s) so that
    // the sample respects the min and max distances.
#if SAMPLE!=0
     sample->insertBWT(pos);
#endif

     lcp->update(position_mod_bwt, shift);

    return shift;
  }
  

  size_t DynSA::deleteChars(size_t n, size_t pos) {

    //cerr << "DynSA::deleteChars: " << pos << " to " << pos+n-1 << endl;

    size_t   tmp_rank;
    size_t old_length = getSize();
    uchar current_letter;

    // We don't want to delete after the text nor the last symbol
    if (n+pos-1 >= old_length)
      n = old_length-pos+1;

    pos_first_modif = getISA(n+pos);
    letter_substituted = (*L)[pos_first_modif];

    action = DSA_DELETING;
    position_mod_bwt = getNumberOfSymbolsSmallerThan(letter_substituted)
      +rank(letter_substituted,pos_first_modif);
 
    // Step IIa : deletion of the elements.
    for (size_t k=n-1; k > 0; k--) {
      current_letter = (*L)[position_mod_bwt];
      // Computes the rank before we delete at this position!
      tmp_rank = rank(current_letter,position_mod_bwt);
       
      deleteSymbol(position_mod_bwt);

      lcp->deleteSuffixAtPos(position_mod_bwt);
#if SAMPLE!=0
      sample->deleteBWT(pos+k, position_mod_bwt);
#endif
      // Next position to delete
      position_mod_bwt = getNumberOfSymbolsSmallerThan(current_letter)+tmp_rank;
    }
    
    current_letter = (*L)[position_mod_bwt];
    tmp_rank = rank(current_letter, position_mod_bwt);
#ifdef DBG
    cout << "Deleting "<<current_letter<<" at position " << position_mod_bwt << endl;
#endif
    deleteSymbol(position_mod_bwt);

      lcp->deleteSuffixAtPos(position_mod_bwt);
#if SAMPLE!=0
     startChrono();
     sample->deleteBWT(pos, position_mod_bwt);
     endChrono();
#endif
     previous_cs = tmp_rank+getNumberOfSymbolsSmallerThan(current_letter);

#ifdef DBG     
     cout << getBWT() << endl;
     cout << "Substitution by " << current_letter << " at position " << pos_first_modif << " (current letter at this position is " << (*L)[pos_first_modif] << ")" << endl;
#endif
     // Changes the last letter of the cyclic shift T^{[pos+1]}
     size_t backup = previous_cs;
     position_mod_bwt = pos_first_modif;
     deleteSymbol(position_mod_bwt);
     new_letter_L = current_letter;
     insert(current_letter, position_mod_bwt);
     // Need a backup in the case we delete all the letters.
     // In that situation previous_cs equals 0, which is impossible!
     previous_cs = backup;

     //    newLCPValue(bwt_pos, oldbwt_pos);

    // Step IIb : reordering step
     size_t shift = reorderCS();

    // The sample is treated now. Until now, we have just deleted
    // elements in the bit vectors. 
    // Now we are adding/removing  sample(s) so that
    // DSASampling still respects the min and max distances.
#if SAMPLE!=0
     sample->deleteBWT(pos);
#endif

     lcp->update(position_mod_bwt, shift);

    return shift;
  }


  size_t DynSA::addTextFromSA(const size_t *SA,  uchar *str, size_t n) {
    size_t i, pos;

    for (i=0; i < n; i++) {
      pos = SA[i];
      if (pos == 0)
	pos = n-1;
      else 
	pos--;
      insert(str[pos], i+1);
    }

    return n;
  }
  
  // length n of text must include "\0"!
  size_t DynSA::addText( uchar *str, size_t n){
    // Initialises the different structures
    insert(str[n-1], 1);
#if SAMPLE!=0
    sample->insertBWT(1,1);
    sample->insertBWT(1);
#endif
    lcp->newSuffixAtPos(1);
    addChars(str, n-1, 1);
    return 1;
  }


  size_t DynSA::addTextFromFile(char* filename, size_t n){
    std::ifstream str(filename, ios::binary);
    if (!str)
      {
	cerr << "error reading file: " << filename << endl;
	exit(EXIT_FAILURE);
      }
    
    
    char c;
    size_t i=0;
    uchar *text = new uchar[n+1];
    
    while (str.get(c) && i < n)
      {
	text[i]=c;//(c=='\n'||c=='\r')?'X':c;
	i++;
      }
    str.close();
    text[i]='\0';
    i=addText(text, i+1);
    delete [] text;
    return i;
  }


  uchar* DynSA::retrieveText(){
    uchar *text=0;
    size_t n=L->getSize();
	
    text = new uchar[n]; // last byte 0 for cout
	
    uchar c;
    size_t i = 1;
    
    for (size_t t=n-1; t > 0; t--) {
      c = (*L)[i];
      if (action == DSA_INSERTING && i == position_mod_bwt)
	text[t-1] = letter_substituted;
      else
	text[t-1] = c;
      i= LFmapping(i);
    }
//     text[n-1] = '$';
    text[n-1] = '\0';
    return text;
  }



  size_t DynSA::getNumberOfSymbolsSmallerThan(uchar c){
    int j = 256+c;
    size_t r=0;
    while(j>1) {
      if (binaryTree_isRightChild(j)) 
	r += C[binaryTree_left(binaryTree_parent(j))];
		
      j=binaryTree_parent(j);
    }
    // We have not really deleted the "letter_substituted" in L, so it is 
    // still taken into account in F as well.
    // The deletion of letter_substituted will only take place at the end
    // of the algorithm. Once we know with which letter we have to substitute
    // it.
    if (action == DSA_DELETING) {
      if (c > letter_substituted)
	r--;
    }
    return r;
  }


  size_t DynSA::getRankInF(size_t pos, uchar &c) {
    size_t j=1, sag, sad;
    while (j < 256) {
      sag = C[binaryTree_left(j)];
      sad = C[binaryTree_right(j)];
      if (sag >= pos) 
	j = binaryTree_left(j);
      else {
	j = binaryTree_right(j);
	pos -= sag;
      }
    }
    c = (j-256);
    return pos;
  }


  void DynSA::startChrono() {
    struct timeval time;
    time_t sec;
    suseconds_t usec;

    if (gettimeofday(&time,NULL) != -1) {
      sec = time.tv_sec;
      usec = time.tv_usec;
      start_time = (ullong)sec*1000000+usec;
    }
  }

  void DynSA::endChrono() {
    struct timeval time;
    time_t sec;
    suseconds_t usec;

    if (gettimeofday(&time,NULL) != -1) {
      sec = time.tv_sec;
      usec = time.tv_usec;
      total_resampling_time += (ullong)sec*1000000+usec-start_time;
    }
  }

  ullong DynSA::getTotalChrono() {
    return total_resampling_time;
  }
  
  void DynSA::resetChrono() {
    total_resampling_time = 0;
  }

  size_t DynSA::substitutionIb(size_t pos, uchar c, size_t &rank_store) {
    // Computing the position of the <pos>-th cyclic shift of the text, in the BWT.
    size_t bwt_pos = getISA(pos);

    if (getSize() > 0) {
      letter_substituted = (*L)[bwt_pos];
      rank_store = rank(letter_substituted, bwt_pos);

      L->deleteSymbol(bwt_pos);
    } else {
      rank_store = 0;
    }
    insert(c, bwt_pos);
    pos_first_modif = bwt_pos;
    return bwt_pos;
  }

  // Uses the value in new_letter, position_mod_bwt and previous_cs
  size_t DynSA::reorderCS() {
    action = DSA_REORDERING;
    uchar L_store = new_letter_L;
    size_t expected_position = getNumberOfSymbolsSmallerThan(L_store)+rank(L_store,position_mod_bwt);
    size_t smaller;
    size_t shift = 0;


    smaller = getNumberOfSymbolsSmallerThan(L_store);

    while (expected_position != previous_cs) {
      shift++;
      L_store = (*L)[previous_cs];
      smaller = getNumberOfSymbolsSmallerThan(L_store);
      size_t tmp_previous_cs = rank(L_store,previous_cs)+smaller;

      // Updating the LCP
      lcp->deleteSuffixAtPos(previous_cs);
      lcp->newSuffixAtPos(expected_position);

      // Places the letter at the right place
      deleteSymbol(previous_cs);
      insert(L_store, expected_position);

#if SAMPLE!=0
      sample->moveBWT(previous_cs, expected_position);
#endif

      // Go to the preivous cyclic shift.
      position_mod_bwt = expected_position;
      previous_cs = tmp_previous_cs;
      expected_position = smaller+rank(L_store,position_mod_bwt);

    }
    action = DSA_IDLE;

    position_mod_bwt = expected_position;
    return shift;
  }


  ulong DynSA::getLCP(ulong pos) {
    return lcp->getLCP(pos);
  }

  ulong DynSA::getSA(ulong pos) {
#if SAMPLE!=0
    return sample->getSA(pos);
#else
    return 1;
#endif
  }

  uchar *DynSA::retrieveTextFactor(ulong pos, ulong length) {
    ulong bwt_pos;
    bool overflow = false;
    if (pos+length > L->getSize()) {
      overflow = true;
      pos--;
    }
    bwt_pos = getISA(pos+length);

    uchar *chars = new uchar[length+1];
    if (overflow)
      length--;
    for (ulong i = 0; i < length; i++) {
	chars[length-i-1] = (*L)[bwt_pos];
	bwt_pos = LFmapping(bwt_pos);
    }
    if (overflow) {
      length++;
      chars[length-1] = '\0';
    }
    chars[length] = '\0';
    return chars;
  }


} // namespace

