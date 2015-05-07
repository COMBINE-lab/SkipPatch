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

/*
 * This is the implementation of the method for sampling a SA and an ISA
 * described in:
 * Dynamic Extended Suffix Arrays. 
 * M. Salson and T. Lecroq and M. L\'eonard and L. Mouchard
 * Journal of Discrete Algorithms
 * To appear
 */


#ifndef DSA_SAMPLING_H
#define DSA_SAMPLING_H

#include <iostream>
#include "lpermutation.h"
#include "sbvtree.h"

class DSASampling;
#include "dynSA.h"

using namespace sbvtree;
using namespace dynsa;

/**
 * DSASampling is a class that manages the sampling for a Dynamic Suffix Array
 * It mainly consists in two bit vectors that mark positions and values sampled
 * in ISA as well as a permutation for mapping a position to its value.
 */
class DSASampling {
 public:
  // private:
  DynSA *dSA;			/* the dynamic SA sampling is linked to */
  ulong min_sampling_d;		/* Min distance between two sampled positions */
  ulong max_sampling_d;		/* Max distance between two sampled positions */
  SBVTree *ISApositions;		/* positions (of ISA) that are sampled */
  SBVTree *ISAvalues;		/* sampled values of ISA */
  LPermutation *pi;		/* mapping between positions and values */
  ulong size;

  void addSample(ulong pos, ulong value, bool replaces=false);
  void deleteSample(ulong pos);
 public:
  /* max sampling is calculated to be at  3.5*min */
  DSASampling(DynSA *dynSA, ulong min=20);
  ~DSASampling();

  /**
   * returns the length of the sampling
   */
  ulong getSize();

  /**
   * returns the value of ISA[index]
   */
  ulong getISA(ulong index);

  /**
   * returns the value of SA[index]
   */
  ulong getSA(ulong index);

  /**
   * If pos_bwt != 0:
   *   Warns that we have inserted an element in the text at position pos_text
   *   and that it was inserted in the BWT at the position pos_bwt.
   * If pos_bwt == 0, we check if the sample has to be updated at pos_text
   *   (new sample(s) may be added). 
   *
   * This strategy allows to check the need for an update after having
   * inserted everything in the BWT.
   */
  void insertBWT(ulong pos_text, ulong pos_bwt=0);

  /**
   * If pos_bwt != 0:
   *  Warns that we have deleted an element in the text at position pos_text.
   *  This element was also deleted at position pos_bwt in the BWT.
   * If pos_bwt == 0, we check if the sample has to be updated at pos_text
   *   (sample(s) may be deleted). 
   */
  void deleteBWT(ulong pos_text, ulong pos_bwt=0);

  /**
   * Tells us that the element at position original_bwt_pos is moved
   * to the position new_bwt_pos.
   * We update accordingly the structure but the density of the samples
   * is not affected by this modification.
   */
  void moveBWT(ulong original_bwt_pos, ulong new_bwt_pos);


friend ostream &operator<<(ostream &os, DSASampling &dsas);

};

/**
 * For outputting the sample (debugging purpose)
 */
ostream &operator<<(ostream &os, DSASampling &dsas);
#endif
