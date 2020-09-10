/*******************************************************************************
 * @project Static and Adaptive Huffman Coding
 * @author Michal Ormos
 * @email xormos00@stud.fit.vutbr.cz
 * @date March 2020
 * @subject KKO
 * @file adaptive.h
 * @description Declaration of Adaptive Huffman functions
*******************************************************************************/

#ifndef ADAPTIVE_H
#define ADAPTIVE_H
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <queue>
#include "helper.h"

using namespace std;

class Adaptive {
  Tree *treeNodeParent;
  Tree *treeSymbols[2 * 257];
  Tree *treeOrders[2 * 257];
  unsigned char *fileBytes;
  size_t fileLength;
  deque<bool> bitsSymbols;
  public:
    Adaptive(unsigned char *fileBytes, size_t fileLength);
    Tree *createTreeTree(unsigned char treeSymbol);
    void editTree(Tree *treeNode);
    void encodeFile(FILE *outputFile);
    void decodeFile(vector<unsigned char> &bytesDecoded);
    void freeTree(Tree *treeNode);
    ~Adaptive();
};

#endif
