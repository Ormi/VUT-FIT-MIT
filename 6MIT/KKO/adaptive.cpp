/*******************************************************************************
 * @project Static and Adaptive Huffman Coding
 * @author Michal Ormos
 * @email xormos00@stud.fit.vutbr.cz
 * @date March 2020
 * @subject KKO
 * @file adaptive.cpp
 * @description Function implementation of adaptive Huffman Coding
*******************************************************************************/

#include "adaptive.h"
using namespace std;

Adaptive::Adaptive(unsigned char *fileBytes, size_t fileLength)
    : treeSymbols{}, treeOrders{}, fileBytes{fileBytes}, fileLength{fileLength} {
        treeNodeParent = new Tree();
        treeNodeParent->setNodeSymbol(256);
        treeNodeParent->setTreeOrder(2 * 257);
        treeSymbols[256] = treeNodeParent;
    }

Adaptive::~Adaptive() {
    freeTree(treeNodeParent);
}

void Adaptive::freeTree(Tree *treeNode) {
    if (treeNode == nullptr) {
        return;
    }
    freeTree(treeNode->getLeftTreeNode());
    freeTree(treeNode->getRightTreeNode());
    delete treeNode;
}

/**
 * Createting subtree for the individual symbol node
 * Creating two children
 * Setting NYT node as parent of a children
 * Adjusting order to keep correct ordering
 */
Tree *Adaptive::createTreeTree(unsigned char treeSymbol) {
    Tree *orderNode = treeSymbols[256];
    Tree *leftChildTree = new Tree();
    Tree *rightChildTree = new Tree();
    uint16_t numberOfOrderNode;
    orderNode->setLeftTreeNode(leftChildTree);
    orderNode->setRightTreeNode(rightChildTree);
    orderNode->setNodeSymbol(257);
    numberOfOrderNode = orderNode->getTreeOrder();
    leftChildTree->setParentTreeNode(orderNode);
    leftChildTree->setTreeOrder(numberOfOrderNode - 2);
    leftChildTree->setNodeSymbol(256);
    rightChildTree->setParentTreeNode(orderNode);
    rightChildTree->setTreeOrder(numberOfOrderNode - 1);
    rightChildTree->setNodeSymbol(treeSymbol);
    treeOrders[numberOfOrderNode - 1] = rightChildTree;
    treeSymbols[256] = leftChildTree;
    treeSymbols[treeSymbol] = rightChildTree;
    treeSymbols[2 * 257 - (numberOfOrderNode / 2)] = orderNode;
    return rightChildTree;
}

/**
 * Finding highest node in the node block
 * Swap node with the highest node
 */
void Adaptive::editTree(Tree *treeNode) {
    Tree *treeNodeRoot = treeNode->getParentTreeNode();
    if ( treeNodeRoot == treeNodeParent) {
        treeNode->incrementTreeWeight();
        treeNodeParent->incrementTreeWeight();
        return;
    }
    Tree *symbol_node, *highest_node_parent, **tempTree, **lastTree;
    uint16_t treeNodeOrder = treeNode->getTreeOrder();
    size_t treeNodeWeight = treeNode->getTreeWeight();
    Tree *tempNode, *latestNode = treeNodeRoot;
    uint16_t latestOrder;

    if (treeNodeRoot == treeSymbols[256]->getParentTreeNode()) {
        for (tempTree = treeOrders + treeNodeOrder + 1, lastTree = treeOrders +2 * 257 ; tempTree < lastTree; ++tempTree) {
            symbol_node = *tempTree;
            if (symbol_node && symbol_node->getNodeSymbol() < 256) {
                if (symbol_node->getTreeWeight() != treeNodeWeight) {
                    break;
                }
                latestNode = symbol_node;
            }
        }
        treeNode->incrementTreeWeight();
        if (__builtin_expect(latestNode != treeNodeRoot, 0)) {
            if (treeNodeRoot->getRightTreeNode() == treeNode) {
                treeNodeRoot->setRightTreeNode(latestNode);
            } else {
                treeNodeRoot->setLeftTreeNode(latestNode);
            }
            highest_node_parent = latestNode->getParentTreeNode();
            if (highest_node_parent->getRightTreeNode() == latestNode) {
                highest_node_parent->setRightTreeNode(treeNode);
            } else {
                highest_node_parent->setLeftTreeNode(treeNode);
            }
            latestOrder = latestNode->getTreeOrder();
            treeNode->setParentTreeNode(highest_node_parent);
            treeNode->setTreeOrder(latestOrder);
            treeOrders[latestOrder] = treeNode;
            latestNode->setParentTreeNode(treeNodeRoot);
            latestNode->setTreeOrder(treeNodeOrder);
            treeOrders[treeNodeOrder] = latestNode;
            treeNode = highest_node_parent;
        } else {
            treeNode = treeNodeRoot;
        }
    } else {
        for (tempTree = treeOrders + treeNodeOrder + 1,
                    lastTree = treeOrders + 2 * 257;
            tempTree < lastTree; ++tempTree) {
            symbol_node = *tempTree;
            if (symbol_node) {
                if (symbol_node->getTreeWeight() != treeNodeWeight) {
                    break;
                }
                latestNode = symbol_node;
            }
        }
        treeNode->incrementTreeWeight();
        if (__builtin_expect(latestNode != treeNodeRoot, 0)) {
            if (treeNodeRoot->getRightTreeNode() == treeNode) {
                treeNodeRoot->setRightTreeNode(latestNode);
            } else {
                treeNodeRoot->setLeftTreeNode(latestNode);
            }
            highest_node_parent = latestNode->getParentTreeNode();
            if (highest_node_parent->getRightTreeNode() == latestNode) {
                highest_node_parent->setRightTreeNode(treeNode);
            } else {
                highest_node_parent->setLeftTreeNode(treeNode);
            }
            latestOrder = latestNode->getTreeOrder();
            treeNode->setParentTreeNode(highest_node_parent);
            treeNode->setTreeOrder(latestOrder);
            treeOrders[latestOrder] = treeNode;
            latestNode->setParentTreeNode(treeNodeRoot);
            latestNode->setTreeOrder(treeNodeOrder);
            treeOrders[treeNodeOrder] = latestNode;
            treeNode = highest_node_parent;
        } else {
            treeNode = treeNodeRoot;
        }
    }
    if (treeNode != treeNodeParent) {
        do {
            treeNodeRoot = treeNode->getParentTreeNode();
            if (treeNodeRoot == treeNodeParent) {
                treeNode->incrementTreeWeight();
                treeNodeParent->incrementTreeWeight();
                return;
            }
            treeNodeOrder = treeNode->getTreeOrder();
            treeNodeWeight = treeNode->getTreeWeight();
            latestNode = treeNodeRoot;
            tempNode = treeNodeRoot;

            for (tempTree = treeOrders + treeNodeOrder + 1,
                        lastTree = treeOrders + 2 * 257;
                tempTree < lastTree; ++tempTree) {
                symbol_node = *tempTree;
                if (symbol_node) {
                    if (symbol_node->getTreeWeight() != treeNodeWeight) {
                        tempNode = symbol_node;
                        break;
                    }
                    latestNode = tempNode;
                }
            }
            for (tempTree = treeOrders + treeNodeOrder + 1,
                        lastTree = treeOrders + 2 * 257;
                tempTree < lastTree; ++tempTree) {
                symbol_node = *tempTree;
                if (symbol_node) {
                    if (symbol_node->getTreeWeight() != treeNodeWeight) {
                        break;
                    }
                    latestNode = symbol_node;
                }
            }
            treeNode->incrementTreeWeight();
            if (__builtin_expect(latestNode != treeNodeRoot, 0)) {
                if (treeNodeRoot->getRightTreeNode() == treeNode) {
                    treeNodeRoot->setRightTreeNode(latestNode);
                } else {
                    treeNodeRoot->setLeftTreeNode(latestNode);
                }
                highest_node_parent = latestNode->getParentTreeNode();
                if (highest_node_parent->getRightTreeNode() == latestNode) {
                    highest_node_parent->setRightTreeNode(treeNode);
                } else {
                    highest_node_parent->setLeftTreeNode(treeNode);
                }
                latestOrder = latestNode->getTreeOrder();
                treeNode->setParentTreeNode(highest_node_parent);
                treeNode->setTreeOrder(latestOrder);
                treeOrders[latestOrder] = treeNode;
                latestNode->setParentTreeNode(treeNodeRoot);
                latestNode->setTreeOrder(treeNodeOrder);
                treeOrders[treeNodeOrder] = latestNode;
                treeNode = highest_node_parent;
            } else {
                treeNode = treeNodeRoot;
            }
        } while (treeNode != treeNodeParent);
    }
    treeNode->incrementTreeWeight();
}

/**
 * Encoding all input bytes to the output file
 * First byte is handled differently from other bytes
 * No padding is added to the las byte, if last is higher than highest possible
 * we can finish here. Decoder will be able to decode this file
 * Padding info is written to the end of the file.
 */
void Adaptive::encodeFile(FILE *outputBytes) {
    // printf("encodeFile started.\n");
    if (fileLength == 0) {
        return;
    }
    BitHandler bitHandler;
    Tree *symbolNode, *treeNodeRoot, *treeNode;
    size_t counter;
    bool bitBool;
    unsigned char treeSymbol = fileBytes[0];
    bitHandler.addByteToStream(treeSymbol);
    editTree(createTreeTree(treeSymbol));
    for (counter = 1; counter < fileLength; ++counter) {
        unsigned char symbol = fileBytes[counter];
        symbolNode = treeSymbols[symbol];
        if (!symbolNode) {
            treeNode = treeSymbols[256];
        } else {
            treeNode = symbolNode;
        }
        do {
            treeNodeRoot = treeNode->getParentTreeNode();
            bitBool = treeNode == treeNodeRoot->getRightTreeNode();
            bitsSymbols.emplace_front(bitBool);
            treeNode = treeNodeRoot;
        } while (treeNode != treeNodeParent);
        for (bool reverseBitBool : bitsSymbols) {
            bitHandler.addBitToStream(reverseBitBool);
        }
        bitsSymbols.clear();
        if (!symbolNode) {
            editTree(createTreeTree(symbol));
            bitHandler.addByteToStream(symbol);
        } else {
            editTree(symbolNode);
        }
    }
    vector<unsigned char> &allBytes = bitHandler.getBytesFromStream();
    for (unsigned char s : allBytes) {
        fputc(s, outputBytes);
    }
    if (bitHandler.howManyBitLefts() == 0 && allBytes.back() > CHAR_BIT -1) {
        return;
    }
    fputc(bitHandler.howManyBitLefts(), outputBytes);
}

/**
 * Decodeing and storing input bytes.
 * If padding is more than 7, we dont have padding
 * Implementing tree traversal, getting symbols from node, updating tree
 * Remembering visited NYT, when all bits are proced the module finishes
 */
void Adaptive::decodeFile(vector<unsigned char> &bytesDecoded) {
    Tree *treeNode = treeNodeParent, *rightNode, *leftNode;
    bool visitedNode = true;
    unsigned treePad = fileBytes[ fileLength - 1 ];
    size_t bitsInNode = 0, bits, counter;
    unsigned char nodeItem, nodeSymbol, treeBuffer = 0, *node = fileBytes;
    uint16_t symbol;
    if (treePad > CHAR_BIT - 1) {
        bits = (fileLength * CHAR_BIT);
    } else {
        bits = ((fileLength - 1) * CHAR_BIT) - treePad;
    }
    while (true) {
        nodeItem = *node++;
        for (counter = 0; counter < CHAR_BIT; ++counter) {
            bool nodeBit = nodeItem & (1 << (CHAR_BIT - counter - 1));
            if (__builtin_expect(!visitedNode, 1)) {
                rightNode = treeNode->getRightTreeNode();
                leftNode = treeNode->getLeftTreeNode();
                treeNode = nodeBit ? rightNode : leftNode;
                if (!treeNode) {
                    return;
                }
                symbol = treeNode->getNodeSymbol();
                if (__builtin_expect(symbol <= 256, 0)) {
                    if (__builtin_expect(symbol == 256, 0)) {
                        visitedNode = true;
                        treeNode = treeNodeParent;
                    } else {
                        nodeSymbol = symbol;
                        bytesDecoded.emplace_back(nodeSymbol);
                        editTree(treeNode);
                        treeNode = treeNodeParent;
                    }
                }
            } else {
                treeBuffer = treeBuffer | (nodeBit << (CHAR_BIT - bitsInNode - 1));
                bitsInNode = bitsInNode + 1;
                if (bitsInNode == CHAR_BIT) {
                    nodeSymbol = treeBuffer;
                    bytesDecoded.emplace_back(nodeSymbol);
                    editTree(createTreeTree(nodeSymbol));
                    visitedNode = false;
                    bitsInNode = 0;
                    treeBuffer = 0;
                }
            }
            bits = bits -1;
            if (bits == 0) {
                return;
            }
        }
    }
}