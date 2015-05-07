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

#include "lpermutation.h"

LPermutation::LPermutation() {
  perm = new LinkedTree();
  invperm = new LinkedTree();
  length = 0;
}

LPermutation::LPermutation(ulong *pi, ulong size) {
  perm = new LinkedTree();
  invperm = new LinkedTree();
  for (ulong i=0; i < size; i++) {
    insert(pi[i],i+1);
  }
  length = size;
}
  
LPermutation::~LPermutation() {
  delete perm;
  delete invperm;
}

  /***   Requests ***/
  
const ulong LPermutation::getithElement(ulong i) {
  LinkedNode *node = (LinkedNode *)perm->getIthNode(i);
  LinkedNode *invNode = (LinkedNode *)node->element;
  return invperm->getNumNode(invNode);
}

const ulong LPermutation::getithInvElement(ulong i) {
  LinkedNode *invNode = (LinkedNode *)invperm->getIthNode(i);
  LinkedNode *node = (LinkedNode *)invNode->element;
  return perm->getNumNode(node);
}


const ulong LPermutation::getSize() {
  return length;
}

  /*** Commands ***/
  
int LPermutation::deleteElem(ulong i) {
  LinkedNode *invNode = (LinkedNode *)invperm->getIthNode(i);
  LinkedNode *node = (LinkedNode *)invNode->element;
  invperm->deleteNode(invNode);
  perm->deleteNode(node);
  length--;
  return 0;
}

int LPermutation::insert(ulong i, ulong pos) {
  perm->insertElement(NULL, pos);
  LinkedNode *node = (LinkedNode *)perm->getInsertedNode();
  invperm->insertElement(node, i);
  node->element = invperm->getInsertedNode();
  length++;
  return 0;
}

int LPermutation::update(ulong i, ulong j) {
  LinkedNode *invNode = (LinkedNode *)invperm->getIthNode(i);
  LinkedNode *node = (LinkedNode *)invNode->element;
  invperm->deleteNode(invNode);
  invperm->insertElement(node, j);
  node->element = invperm->getInsertedNode();
  return 0;
}


ostream &operator<<(ostream &os,  LPermutation *pi) {
  for (ulong i=0; i < pi->getSize(); i++) {
    os << pi->getithElement(i+1) << " ";
  }
  os << endl;
  for (ulong i=0; i < pi->getSize(); i++) {
    os << pi->getithInvElement(i+1) << " ";
  }
  os << endl;
  return os;
}

