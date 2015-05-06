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

#include <stdlib.h>
#include "permutation.h"

void indente(ulong space) {
  for (ulong i=0; i < space; i++) {
    cout << " ";
  }
}

/// PRIVATE ///
void Permutation::build_perm(BVTree *tree, ulong *perm, ulong size, ulong *s_perm
			     , ulong sbit, ulong level, ulong *levelshifts) {
  if (size == 1) {
    s_perm[0] = perm[0];
    return;
  }

  ulong second_half = size/2, first_half = size-second_half;


  build_perm(tree, perm, first_half, s_perm, sbit+levelshifts[level+1],level+1, levelshifts);
  build_perm(tree, &perm[first_half], second_half, &s_perm[first_half], sbit+levelshifts[level+1], level+1, levelshifts);

  ulong *news_perm = new ulong[size];
  ulong i=0,j=first_half,k=0;
  bool res;
  // Merge sort
  while (i < first_half && j < size) {
    res = s_perm[i] > s_perm[j];
    tree->insertBit(res,sbit+k);
    if (! res) {
      news_perm[k] = s_perm[i];
      i++;
    } else {
      news_perm[k] = s_perm[j];
      j++;
    }
    k++;
  }
  if (i == first_half) {
    while (j < size) {
      news_perm[k] = s_perm[j];
      tree->insertBit(true,sbit+k);
      k++;j++;
    }
  } else {
    while (i < first_half) {
      news_perm[k] = s_perm[i];
      tree->insertBit(false,sbit+k);
      k++;i++;
    }
  }
  levelshifts[level] += k;
  memcpy(s_perm, news_perm, k*sizeof(ulong));
  free(news_perm);
}

void Permutation::pathToithLeaf(ulong i, ulong nb_nodes, ulong nb_elem, ulong &path, uint &path_length
				, ulong &range_size, ulong *min, ulong &total_nodes) {
  if (nb_elem == 1) {
    if (nb_nodes == 1) {
      path <<= 1;
      if (i > 1) 
	path |= 1;
      min[path_length-1] = total_nodes*2;
    } else 
      path_length--;
    return ;
  }

  ulong nb_nodes_left = nb_nodes/2;
  if (nb_nodes % 2 == 1)
    nb_nodes_left++;
  nb_elem /= 2;
  path <<= 1;
  if (nb_nodes_left + nb_elem >= i) {
    // Go in the left subtree
    nb_nodes = nb_nodes_left;
    range_size = range_size-range_size/2;
    min[path_length]=min[path_length-1];
  } else {
    // Go in the right subtree
    path |= 1;
    nb_nodes = nb_nodes/2;
    i -= nb_nodes_left + nb_elem;
    total_nodes += nb_nodes_left;
    min[path_length] = min[path_length-1]+(range_size - range_size/2);
    range_size /= 2;
  }
  path_length++;
  pathToithLeaf(i,nb_nodes,nb_elem,path,path_length,range_size, min, total_nodes);
}

// Function used to find the value of the inverse permutation
void Permutation::calculateInv(ulong pos, Permutation::Iterator *it, ulong &num_leaf) {
  
  bool bit = (*tree)[pos];
  ulong nodeSize = it->getNodeSize();

  if ((nodeSize== 3) && bit) {
    num_leaf += 3;
    return;
  }
  if (nodeSize == 2) {
    num_leaf += 1+bit;
    return ;
  }
    
  ulong minrank, rank, minpos=it->getPosition()-1;
  if (minpos == 0)
    minrank=0;
  else
    minrank = tree->rank(bit, minpos);
  
  rank = tree->rank(bit, pos);

  if (bit) {
    num_leaf += nodeSize - nodeSize/2;
    it->right();
  } else {
    it->left();
  }
  pos = it->getPosition()-1+(rank-minrank);

  calculateInv(pos, it, num_leaf);
}

void Permutation::applyUpdate(ulong posOrig, long long shift, Permutation::Iterator *it) {
  bool bit = (*tree)[posOrig];
  ulong rank_block;
  ulong startRank = tree->rank(bit, posOrig);
  ulong endRank;


  if (shift == 0) return;

  if (it->getPosition() > 1) {
    rank_block = tree->rank(bit, it->getPosition()-1);
  } else
    rank_block = 0;
  startRank -= rank_block;

  if (shift < 0) {
    if (posOrig+shift-1 == 0)
      endRank = 1;
    else
      endRank = tree->rank(bit, posOrig+shift-1)-rank_block+1;
  } else
    endRank = tree->rank(bit, posOrig+shift)-rank_block;
  
//   cout << "Appel: bit="<<bit<<",startRank="<<startRank<<",endRank="<<endRank<<",posOrig="<<posOrig<<",dest="<<posOrig+shift<<",shift="<<shift<<endl;

  tree->deleteBit(posOrig);
  tree->insertBit(bit, posOrig+shift);

  if (bit)
    it->right();
  else
    it->left();

  applyUpdate(it->getPosition()+startRank-1, (long long)endRank-(long long)startRank, it);
}

/// PUBLIC ///

Permutation::Permutation(ulong *perm, ulong size) {
  this->size = size;
  nb_levels = log2(size);
  nb_elements_last_complete_level = (1 << nb_levels);
  nb_leaves_last_level = (size-nb_elements_last_complete_level)*2;
  is_perfect_tree = (nb_leaves_last_level == 0);
  if (! is_perfect_tree)
    nb_levels++;
  
  tree = new BVTree();
  ulong *tmp_perm = new ulong[size];
  ulong *levelshifts = (ulong *)calloc(nb_levels, sizeof(ulong));
  build_perm(tree, perm, size, tmp_perm, 1, 0, levelshifts);
  delete [] tmp_perm;
  delete [] levelshifts;
}

Permutation::~Permutation() {
  delete tree;
}


ulong Permutation::getithElement(ulong i) { 
  ulong path=0;
  uint path_length=1;
  ulong range_size = this->size, total_nodes=0;
  ulong *min = (ulong *)calloc(this->nb_levels+1,sizeof(ulong));
  pathToithLeaf(i, nb_leaves_last_level/2, nb_elements_last_complete_level, path
		, path_length, range_size, min, total_nodes);
  bool bit;
  ulong vmin, rank=1, tmprank;
  for (uint j=path_length; j>0 ; j--) {
    bit = (path & 1) == 1;
    path >>=1;
    vmin = (j-1)*this->size+min[j-1];
    if (vmin > 0)
      tmprank = tree->rank(bit, vmin)+rank;
    else
      tmprank = rank;
    rank = tree->select(bit, tmprank) - vmin;
  }
  delete [] min;
  return rank;
}

ulong Permutation::getithInvElement(ulong i){ 
  ulong result=0;
  Permutation::Iterator *it = new Permutation::Iterator(this);
  //  calculateInv(i,0,size,0,0,nb_leaves_last_level/2, result);
  calculateInv(i,it,result);
  return result;
}


  ulong Permutation::getSize(){ 
    return this->size; 
  }

  /*** Commands ***/
  
  /**
   * Deletes the element i
   * At the end, we are still storing a permutation of size: old getSize()-1
   * @return 0 if OK
   */
  int Permutation::deleteElem(ulong i){ return 0; }

  /**
   * Inserts the element i at position pos. The positions start at position 0.
   * At the end, we are still storing a permutation of size: old getSize()+1
   * @return 0 if OK
   */
  int Permutation::insert(ulong i, ulong pos){ return 0; }

  /**
   * Moves the element from position i to position j
   * @return 0 if OK.
   */
  int Permutation::move(ulong i, ulong j){ return 0; }

  /**
   * Let k such that pi(k)=i. At the end,  pi(k)=j. Additionally, pi is still 
   * a permutation, that is each value from 1 to getSize() appears only once.
   * @return 0 if OK.
   */
  int Permutation::update(ulong i, ulong j) { 
    if (i == j)
      return 0;

    Permutation::Iterator *it = new Iterator(this);
    long long shift = j;
    shift -= i;
    applyUpdate(i,shift,it);
    
    return 0; 
  }
  

Permutation::Iterator::Iterator(Permutation *p):pi(p),current_pos(1),nb_nodes_left(p->nb_leaves_last_level/2),nb_nodes_subtree(p->nb_leaves_last_level/2),nb_elements_left(0),level(0),current_size(p->size) {}

ulong Permutation::Iterator::getNodeSize() {
  return current_size;
}

ulong Permutation::Iterator::getPosition() {
  return current_pos;
}

void Permutation::Iterator::left() {
  if (current_size == 2)
    return ;

  current_size -=  current_size/2;
  level++;
  nb_nodes_left -= nb_nodes_subtree/2;
  nb_nodes_subtree -= nb_nodes_subtree/2;
  
  if (level+1 < pi->nb_levels || pi->is_perfect_tree)
    current_pos = pi->size*level+1+nb_elements_left;
  else 
    current_pos = pi->size*level+1+(nb_nodes_left-1)*2;
}

void Permutation::Iterator::right() {
  if (current_size < 4)
    return;
  ulong old_size = current_size;
  current_size /= 2;
  nb_nodes_subtree /= 2;
  level++;
  nb_elements_left += old_size - current_size;

  if (level+1 < pi->nb_levels || pi->is_perfect_tree)
    current_pos = pi->size*level+1+nb_elements_left;
  else
    current_pos = pi->size*level+1+(nb_nodes_left-1)*2;
}
