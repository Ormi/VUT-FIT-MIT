/*******************************************************************************
 * @project Static and Adaptive Huffman Coding
 * @author Michal Ormos
 * @email xormos00@stud.fit.vutbr.cz
 * @date March 2020
 * @subject KKO
 * @file static.h
 * @description Declaration of class for static Huffman Coding
*******************************************************************************/

#ifndef STATIC_H
#define STATIC_H

#include <stdio.h>
#include <algorithm>
#include <queue>

#include "helper.h"
using namespace std;

class Static {
	vector<size_t> symbolFrequency;
  	vector<pair<unsigned char, vector<bool>>> symbolCode;
  	priority_queue<Tree *, vector<Tree *>, compareTreeNodes>huffmanCodeTree;
  	unsigned char *fileBytes;
  	size_t fileLength;
  	size_t countOfBytes;
  	size_t bitPositionInByte;
  	unsigned pad;
	public:
		Static(unsigned char *fileBytes, size_t fileLength);
		void buildTree();
		void firstComeFirstServer();
		void decodeBytes(vector<unsigned char> &decodeBytes);
		void encodeBytes(FILE *outputBytes);
		void makeTreeCanonical();
		void saveCode();
		void saveCode(Tree *treeNode, vector<bool> treeCode = {});
		void freeTree(Tree *treeNode);
		~Static();
};

#endif