/*******************************************************************************
 * @project Static and Adaptive Huffman Coding
 * @author Michal Ormos
 * @email xormos00@stud.fit.vutbr.cz
 * @date March 2020
 * @subject KKO
 * @file helper.cpp
 * @description Implementation of Tree used for encoding as well as bits handling module
*******************************************************************************/

#include "helper.h"
#define ZERO 0

////////////////////////////////////////////////////////////////////////////////

BitHandler::BitHandler() : bytes{ZERO}, bitPositionInByte{ZERO} {}

void BitHandler::addBitToStream(bool bit) {
    if (bitPositionInByte >= CHAR_BIT) {
        bytes.emplace_back(bit << (CHAR_BIT - 1));
        bitPositionInByte = 1;
    } else {
        bytes.back() |= bit << (CHAR_BIT - bitPositionInByte - 1);
        bitPositionInByte = bitPositionInByte + 1;
    }
}

void BitHandler::addBitsToStream(std::vector<bool> &bits) {
    for (bool bit : bits) {
        addBitToStream(bit);
    }
}

void BitHandler::addByteToStream(unsigned char byte) {
    size_t counter;
    for (counter = 0; counter < CHAR_BIT; ++counter) {
        addBitToStream((byte >> (CHAR_BIT - counter - 1)) & 1);
    }
}

unsigned BitHandler::howManyBitLefts() {
    size_t interResults, finalResult;
    interResults = CHAR_BIT - bitPositionInByte;
    finalResult = interResults % CHAR_BIT;
    return finalResult;
}

vector<unsigned char> &BitHandler::getBytesFromStream() {
    return bytes;
}

////////////////////////////////////////////////////////////////////////////////

Tree::Tree()
    :treeWeight{ZERO},
    parentNode{nullptr},
    leftNode{nullptr},
    rightNode{nullptr},
    nodeSymbol{ZERO},
    treeOrder{ZERO}
    {}
Tree::Tree(uint16_t nodeSymbol, size_t treeWeight)
    :treeWeight{treeWeight},
    parentNode{nullptr},
    leftNode{nullptr},
    rightNode{nullptr},
    nodeSymbol{nodeSymbol}
    {}
Tree::Tree(size_t treeWeight, Tree *leftNode, Tree *rightNode)
    :treeWeight{treeWeight},
    parentNode{nullptr},
    leftNode{leftNode},
    rightNode{rightNode}
    {}

uint16_t Tree::getNodeSymbol() {
    return nodeSymbol;
}
size_t Tree::getTreeWeight() {
    return treeWeight;
}
uint16_t Tree::getTreeOrder() {
    return treeOrder;
}

void Tree::setTreeOrder(uint16_t newTreeOrder) {
    treeOrder = newTreeOrder;
}
void Tree::incrementTreeWeight() {
    ++treeWeight;
}
void Tree::setTreeWeight(size_t newTreeWeight) {
    treeWeight = newTreeWeight;
}
void Tree::setNodeSymbol(uint16_t newNodeSymbol) {
    nodeSymbol = newNodeSymbol;
}

void Tree::setLeftTreeNode(Tree *newLeftNode) {
    leftNode = newLeftNode;
}
void Tree::setRightTreeNode(Tree *newRightNode) {
    rightNode = newRightNode;
}
void Tree::setParentTreeNode(Tree *newParentNode) {
    parentNode = newParentNode;
}

Tree *Tree::getLeftTreeNode() {
    return leftNode;
}
Tree *Tree::getRightTreeNode() {
    return rightNode;
}
Tree *Tree::getParentTreeNode() {
    return parentNode;
}

bool Tree::isLeafNode() {
    return leftNode == nullptr && rightNode == nullptr;
}
bool Tree::operator<(Tree &other) const {
    return treeWeight < other.treeWeight;
}
bool compareTreeNodes::operator()(Tree *leftNode, Tree *rightNode) {
    return (leftNode->getTreeWeight() > rightNode->getTreeWeight());
}