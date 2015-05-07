/***************************************************************************
 *   DynSA - Dynamic Suffix Array                                          *
 *   2008 - Mikael Salson                                                  *
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

#include "DSASampling.h"

DSASampling::DSASampling(DynSA *dynSA, ulong min):dSA(dynSA),min_sampling_d(min),max_sampling_d(7*min/2),size(0) {
  ISApositions = new SBVTree();
  ISAvalues = new SBVTree();
  pi = new LPermutation();
}

DSASampling::~DSASampling() {
#ifndef NFREE_ISA
  delete ISApositions;
  delete ISAvalues;
#endif
#ifndef NFREE_PERMUTATION
  delete pi;
#endif
}

ulong DSASampling::getSize() {
  return size;
}

ulong DSASampling::getISA(ulong index) {
  size_t nb_sampled = ISApositions->rank(true,index);
  size_t bwt_pos;
  if ((*ISApositions)[index]) {
    return ISAvalues->select(true, pi->getithElement(nb_sampled));
  }
  nb_sampled++;
  size_t shift_pos = ISApositions->select(true,nb_sampled)-index;
  bwt_pos = ISAvalues->select(true, pi->getithElement(nb_sampled));
  for (size_t i=0; i < shift_pos; i++) {
    bwt_pos = dSA->LFmapping(bwt_pos);
  }
  return bwt_pos;
}

ulong DSASampling::getSA(ulong index) {
  size_t shift=0;
  while (! (*ISAvalues)[index]) {
    index = dSA->LFmapping(index);
    shift++;
  }
  ulong result = ISApositions->select(true, pi->getithInvElement(ISAvalues->rank(true, index)))+shift;
  if (result > getSize())
    return result % getSize();
  return result;
}

void DSASampling::insertBWT(ulong pos_text, ulong pos_bwt) {
  ulong rank, pos_right_one, pos_left_one;
  if (pos_bwt!=0) {
    // Insert false bits for the moment, we'll check after the whole insertion
    // in the BWT if we need to add samples.
    ISApositions->insertBit(false, pos_text);
    ISAvalues->insertBit(false, pos_bwt);
    size++;
    return ;
  }
  if (size == 1) {
    ISApositions->deleteBit(1);
    ISAvalues->deleteBit(1);
    ISApositions->insertBit(true, 1);
    ISAvalues->insertBit(true, 1);
    pi->insert(1, 1);
    return ;
  }
  // Testing the positions
  if (pos_text == 1)
    rank = 0;
  else
    rank = ISApositions->rank(true, pos_text-1);
  pos_right_one = ISApositions->select(true,rank+1);
  if (rank == 0)
    pos_left_one = 1;
  else
    pos_left_one = ISApositions->select(true, rank);

  if (pos_right_one - pos_left_one  > max_sampling_d) {
    // We need to add some samplings
    ulong nb_pieces = 2*(pos_right_one-pos_left_one+1)/(max_sampling_d+min_sampling_d)-1;
    for (ulong k = 0; k < nb_pieces; k++) {
      ulong sample_pos = pos_right_one-(k+1)*(min_sampling_d+max_sampling_d)/2;
      ulong isa;
      if (size > 0) {
	  isa = getISA(sample_pos);
      } else
	isa = 1;
      addSample(sample_pos, isa, true);
    }
  }
}


void DSASampling::deleteBWT(ulong pos_text, ulong pos_bwt) {
  ulong rank, pos_right_one, pos_left_one;

  // Just deleting the elements without considering min and max distances.
  if (pos_bwt != 0) {
    if ((*ISApositions)[pos_text]) {
      ulong rank = ISApositions->rank(true, pos_text);
      pi->deleteElem(pi->getithElement(rank));
    }
    // Finally deleting the  bit
    ISApositions->deleteBit(pos_text);
    ISAvalues->deleteBit(pos_bwt);
    size--;
    
    return ;
  }

  if (pos_text == 1)
    rank = 0;
  else
    rank = ISApositions->rank(true, pos_text-1);
  if (rank == 0) 
    return;

  pos_right_one = ISApositions->select(true,rank+1);
  pos_left_one = ISApositions->select(true, rank);
    
  if (pos_left_one - pos_right_one < min_sampling_d) {
    // We have to delete a sample (and maybe to add another one)
    if (rank == 1) {
      deleteSample(pos_left_one);
      if (pos_left_one > (max_sampling_d-min_sampling_d)/2) {
	  pos_left_one -= (max_sampling_d-min_sampling_d)/2;
	  ulong value = getISA(pos_left_one);
	  addSample(pos_left_one,value, true);
      }	  
    } else {
      ulong pos_left_left_one = ISApositions->select(true,rank-1);
      ulong new_pos;
      if (pos_left_one - pos_left_left_one > (min_sampling_d+max_sampling_d)/2) {
	// The distance between the left and left left bit is sufficient to 
	// move the left bit.
	new_pos = pos_left_one + min_sampling_d/2-(pos_left_one-pos_left_left_one)/2; 
	deleteSample(pos_left_one);
	ulong value = getISA(new_pos);
	addSample(new_pos, value, true);
      } else {
	// Distance between left and left left bit is not sufficient.
	// Therefore we delete the left bit.
	deleteSample(pos_left_one);
      }
    }
  }
  
}

void DSASampling::moveBWT(ulong original_bwt_pos, ulong new_bwt_pos) {
  ulong original_rank, new_rank;
  ulong bit = (*ISAvalues)[original_bwt_pos];
  if (bit)
    original_rank = ISAvalues->rank(true, original_bwt_pos);
  // Moves the bit 
  ISAvalues->deleteBit(original_bwt_pos);
  ISAvalues->insertBit(bit, new_bwt_pos);
  if (bit) {
    new_rank = ISAvalues->rank(true, new_bwt_pos);
    // If the rank has been affected by the move, we need to update the
    // permutation as well
    if (original_rank != new_rank)
      pi->update(original_rank, new_rank);
  }
}

    
void DSASampling::addSample(ulong pos, ulong value, bool replaces) {
  ulong ith_value;
  if (value > 1)
    ith_value = ISAvalues->rank(true, value)+1;
  else
    ith_value = 1;
  
  if (replaces)
    ISApositions->deleteBit(pos);
  ISApositions->insertBit(true, pos);
  ulong rank = ISApositions->rank(true, pos);
  
  ISAvalues->deleteBit(value);
  ISAvalues->insertBit(true, value);
  
  pi->insert(ith_value, rank);
  if (! replaces)
    size++;
}

void DSASampling::deleteSample(ulong pos) {
  ulong rank = ISApositions->rank(true, pos);
  ulong ith_value = pi->getithElement(rank);

  ISAvalues->deleteBit(ith_value);
  ISAvalues->insertBit(false,ith_value);
  pi->deleteElem(rank);
  ISApositions->deleteBit(rank);
  ISApositions->insertBit(false, rank);
}


ostream &operator<<(ostream &os, DSASampling &dsas) {
  os << *dsas.ISApositions << *dsas.ISAvalues << dsas.pi ;
  os<< "ISA : ";
  for (ulong i=0; i < dsas.getSize(); i++) {
    os << dsas.getISA(i+1) << " ";
  }
  os << endl << "SA : " ;
  for (ulong i=0; i < dsas.getSize(); i++) {
    os << dsas.getSA(i+1) << " ";
  }
  os << endl;
  return os;
}
