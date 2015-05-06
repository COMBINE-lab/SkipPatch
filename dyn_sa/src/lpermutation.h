/***************************************************************************
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

// ------ Dynamic Suffix Array --- Dynamic FM-Index -----
//  Storage of a Dynamic Permutation using two trees.
// paper: M. Salson and T. Lecroq and M. L\'{e}onard and L. Mouchard
//       Dynamic Extended Suffix Arrays, JDA, 2008

#ifndef LPERMUTATION_H
#define LPERMUTATION_H

#include <iostream>
#include "linkedTree.h"
#include "types.h"

/**
 * This class manages a permutation that allows updates in it.
 */
class LPermutation {

 private:
  LinkedTree *perm;             /* Tree corresponding to the positions */
  LinkedTree *invperm;          /* Tree corresponding to the values */
  ulong length;

 public:

  LPermutation();
  /* Initialises a permutation given an array. Each value of the array
   * is inserted one by one */
  LPermutation(ulong *pi, ulong size);
  
  ~LPermutation();

  /***   Requests ***/
  
  /**
   * @return the i-th element of the permutation, done in O(log n) time
   */
  const ulong getithElement(ulong i);

  /**
   * @return the i-th element of the inverse permutation, done in O(log n) 
   * time
   */
  const ulong getithInvElement(ulong i);


  const ulong getSize();

  /*** Commands ***/
  
  /**
   * Deletes the element i
   * At the end, we are still storing a permutation of size: old getSize()-1
   * @return 0 if OK
   */
  int deleteElem(ulong i);

  /**
   * Inserts the element i at position pos. The positions start at position 0.
   * At the end, we are still storing a permutation of size: old getSize()+1
   * @return 0 if OK
   */
  int insert(ulong i, ulong pos);

  /**
   * Let k such that pi(k)=i. At the end,  pi(k)=j. Additionally, pi is still 
   * a permutation, that is each value from 1 to getSize() appears only once.
   * @return 0 if OK.
   */
  int update(ulong i, ulong j);

};

ostream &operator<<(ostream &os, LPermutation *pi);


#endif
