/***************************************************************************
 *   DynSA - Dynamic Suffix Array                                          *
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

#ifndef LCP_H
#define LCP_H


class LCP;
#include "dynSA.h"
#include "intTree.h"
#include "sbvtree.h"
#include "types.h"

using  sbvtree::SBVTree;
using dynsa::DynSA;

class LCP {
  IntTree *lcp;                 /* a tree that stores the values of LCP */
  SBVTree *to_update;            /* positions that have to be updated */
  SBVTree *bv_updates;           /* mask for the values that are stored */
  SBVTree *deletions;            /* marks the deletions in the LCP array */
  IntTree *stored_values;       /* stored values for remembering 
                                 * the old values, before update */
  DynSA *dsa;                   /* A reference to the DSA this structure
                                 * corresponds to. */

 public:
  
  LCP(DynSA *sa, bool use_it = true);
  ~LCP();

  ulong getLCP(ulong i);
  ulong getSize();

  /**
   * Tells that we have a new element at position pos
   */
  void newSuffixAtPos(ulong pos);
  /**
   * Tells that we have deleted an element at position pos.
   */
  void deleteSuffixAtPos(ulong pos);
  /**
   * Updates all the values (the ones marked in the bit vectors as well as
   * the ones that have not been marked during the reordering step).
   * The two parameters tell where we stopped the reordering.
   */
  void update(ulong last_cs, ulong order);

 private:
  size_t updateValue(ulong pos, ulong next_cs);

  // Computes the LCP value by retrieving two factors
  // The LCP value is bounded by lower_bound and the upper_bound
  ulong LCPRetrieveFactors(ulong pos, ulong pos2, ulong upper_bound, ulong lower_bound);

  bool updateNextLCPValues(size_t pos, size_t previous_pos, int variance, int variance_ref, size_t lower_bound);

   

};

#endif
