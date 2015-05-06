/***************************************************************************
 *   Copyright (C) 2008  Mikael Salson                                     *
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
// Storage of a dynamic permutation using a tree.
// paper: M. Salson and T. Lecroq and M. L\'{e}onard and L. Mouchard
//       Dynamic suffix arrays, JDA, 2008

#ifndef PERMUTATION_H
#define PERMUTATION_H

#include "sbvtree.h"

using sbvtree::SBVTree;



class Permutation {

 public:
class Iterator {

  private:
     Permutation *pi;
     ulong current_pos;
     ulong nb_nodes_left;
     ulong nb_nodes_subtree;
     ulong nb_elements_left;
     ulong level;
     ulong current_size;
   public:
     
     Iterator(Permutation *p);

     /* Requests */
     
     /**
      * @return the size of the current node
      */
     ulong getNodeSize();

     /**
      * @return the current position in the "tree"
      */
     ulong getPosition();
     
     
     /* Command */
     
     /**
      * Goes in the left subtree, if possible. 
      * Doesn't move otherwise.
      */
     void left();
     
     
     void right();
     
   };



 private:
  SBVTree *tree;
  ulong size;
  ulong nb_levels;
  ulong nb_leaves_last_level;
  ulong nb_elements_last_complete_level;
  bool is_perfect_tree;
  
  
  void build_perm(SBVTree *tree, ulong *perm, ulong size, ulong *s_perm
		  , ulong sbit, ulong level, ulong *levelshifts);

  void pathToithLeaf(ulong i, ulong nb_nodes, ulong nb_elem, ulong &path
		     , uint &path_length, ulong &range_size, ulong *min, ulong &total_nodes);
  void calculateInv(ulong pos, Iterator *it, ulong &num_leaf);
  void applyUpdate(ulong posOrig, long long shift, Permutation::Iterator *it);
  

 public:

  Permutation(ulong *perm, ulong size);
  
  ~Permutation();

  /***   Requests ***/
  
  /**
   * @return the i-th element of the permutation, done in O(log^2 n) time
   */
  ulong getithElement(ulong i);

  /**
   * @return the i-th element of the inverse permutation, done in O(log^2 n) 
   * time
   */
  ulong getithInvElement(ulong i);


  ulong getSize();

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
   * Moves the element from position i to position j
   * @return 0 if OK.
   */
  int move(ulong i, ulong j);

  /**
   * Let k such that pi(k)=i. At the end,  pi(k)=j. Additionally, pi is still 
   * a permutation, that is each value from 1 to getSize() appears only once.
   * @return 0 if OK.
   */
  int update(ulong i, ulong j);


  friend class Iterator;
};



#endif
