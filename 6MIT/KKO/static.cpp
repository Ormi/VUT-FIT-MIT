/*******************************************************************************
 * @project Static and Adaptive Huffman Coding
 * @author Michal Ormos
 * @email xormos00@stud.fit.vutbr.cz
 * @date March 2020
 * @subject KKO
 * @file static.cpp
 * @description Definition of class for static Huffman Coding
*******************************************************************************/

#include "static.h"
using namespace std;

Static::Static(unsigned char *fileBytes, size_t fileLength)
    :fileBytes{fileBytes},fileLength{fileLength} {
    size_t counter = 0;
    symbolFrequency.resize(256);
    while (counter < fileLength - 1) {
        ++symbolFrequency[fileBytes[counter]];
        counter = counter + 1;
    }
    counter = 0;
    symbolFrequency.resize(256);
    for (counter = 0; counter < fileLength; ++counter) {
        ++symbolFrequency[fileBytes[counter]];
    }
};

Static::~Static() {
    Tree *treeNode;
    while (!huffmanCodeTree.empty()) {
        treeNode = huffmanCodeTree.top();
        huffmanCodeTree.pop();
        freeTree(treeNode);
    }
}

void Static::freeTree(Tree *treeNode) {
    if(treeNode == nullptr) {
        return;
    }
    freeTree(treeNode->getLeftTreeNode());
    freeTree(treeNode->getRightTreeNode());
    delete treeNode;
}

void Static::saveCode() {
    saveCode(huffmanCodeTree.top());
}

void Static::saveCode(Tree *treeNode, vector<bool> treeCode) {
    vector<bool> beginCode;
    vector<bool> beginSymbol;
    if (!treeNode) {
        return;
    }
    if (treeNode->isLeafNode()) {
        symbolCode.emplace_back(make_pair(treeNode->getNodeSymbol(), treeCode));
        return;
    }
    beginCode = treeCode;
    beginSymbol = treeCode;
    beginCode.emplace_back(0);
    beginSymbol.emplace_back(1);
    saveCode(treeNode->getLeftTreeNode(), beginCode);
    saveCode(treeNode->getRightTreeNode(), beginSymbol);
}

void Static::buildTree() {
    Tree *leftNode, *rightNode, *topNode;
    size_t counter, frequencies;

    for (counter = 0; counter < 256 ; ++counter) {
        frequencies = symbolFrequency[counter];
        if (frequencies > 0) {
            huffmanCodeTree.push(new Tree(counter, frequencies));
        }
    }
    if (huffmanCodeTree.size() == 1) {
        huffmanCodeTree.push(new Tree(256, 1));
    }

    while (huffmanCodeTree.size() != 1) {
        leftNode = huffmanCodeTree.top();
        huffmanCodeTree.pop();
        rightNode = huffmanCodeTree.top();
        huffmanCodeTree.pop();
        topNode = new Tree(leftNode->getTreeWeight() + rightNode->getTreeWeight(), leftNode, rightNode);
        huffmanCodeTree.push(topNode);
    }
}

/**
 * Making tree canonical
 * sorting tree lengths by first -> second -> rest
 */
void Static::makeTreeCanonical() {
    sort(symbolCode.begin(), symbolCode.end(),
        [](const pair<unsigned char, vector<bool>> &lhs,
        const pair<unsigned char, vector<bool>> &rhs) {
        return lhs.second.size() < rhs.second.size();
        });
    pair<unsigned char, vector<bool>> &symbol = symbolCode[0];
    size_t leftBit = symbol.second.size();
    size_t leftCode = 0;
    size_t leftSecond, rightSecond, rightBit, rightCode, prevLeftBit, prevLeftCode;
    size_t counter = 0;
    size_t index;
    symbol.second.clear();
    while ( counter < leftBit) {
        counter = counter + 1;
        symbol.second.emplace_back(0);
    }
    if (symbolCode.size() > 1) {
        pair<unsigned char, vector<bool>> &symbolRight = symbolCode[1];
        rightBit = symbolRight.second.size();
        rightCode = (leftCode + 1) << (rightBit - leftBit);
        prevLeftBit = rightBit;
        prevLeftCode = rightCode;

        symbolRight.second.clear();
        while (rightCode) {
            bool set = rightCode & 1 ? 1 : 0;
            symbolRight.second.emplace_back(set);
            rightCode >>= 1;
        }
        while (symbolRight.second.size() < rightBit) {
            symbolRight.second.emplace_back(0);
        }
        reverse(symbolRight.second.begin(), symbolRight.second.end());
        for (counter = 2, index = symbolCode.size(); counter < index; ++counter) {
            pair<unsigned char, vector<bool>> &symbolLeft = symbolCode[counter];
            leftSecond = symbolLeft.second.size();
            rightSecond = (prevLeftCode + 1) * (1 << (leftSecond - prevLeftBit));
            prevLeftBit = leftSecond;
            prevLeftCode = rightSecond;
            symbolLeft.second.clear();
            while (rightSecond) {
                bool set = rightSecond & 1 ? 1 : 0;
                symbolLeft.second.emplace_back(set);
                rightSecond >>= 1;
            }
            while (symbolLeft.second.size() < leftSecond) {
                symbolLeft.second.emplace_back(0);
            }
            reverse(symbolLeft.second.begin(), symbolLeft.second.end());
        }
    }
}

/**
 * Encoding input bytes to the output
 * Building Huffman Tree, saving codes, canonicalize tree
 * Couting leading zeros, encoding symbols
 * Storing last byte as padding in the upper tree bits of offset of non zeros
 * Writing lengths, alphabet and ecode symbols
 */
void Static::encodeBytes(FILE *outputBytes) {
    BitHandler packBits;
    unsigned char actualByte = 0;
    unsigned char lengthCountZero;
    vector<vector <unsigned char>> byteHeader;
    vector<vector<bool>> byteTable;
    size_t counter;
    size_t zeroLengthsCountPad;
    if (fileLength == 0) {
        return;
    }
    buildTree();
    saveCode();
    makeTreeCanonical();
    for (auto &i : symbolCode) {
        actualByte = max((unsigned char)i.second.size(), actualByte);
    }
    lengthCountZero = actualByte;
    for(auto &i : symbolCode) {
        lengthCountZero = min((unsigned char)i.second.size(), lengthCountZero);
    }
    lengthCountZero = lengthCountZero - 1;
    actualByte = actualByte + 1;
    byteHeader.resize(actualByte);
    byteTable.resize(256);
    for (auto &i : symbolCode) {
        unsigned char li = i.second.size();
        byteTable[i.first] = i.second;
        byteHeader[li].emplace_back(i.first);
    }
    for (counter = 0; counter < fileLength; ++counter) {
        packBits.addBitsToStream(byteTable[fileBytes[counter]]);
    }
    unsigned padding = packBits.howManyBitLefts();
    zeroLengthsCountPad = lengthCountZero;
    zeroLengthsCountPad |= padding << 5;
    fputc(actualByte, outputBytes);
    fputc(zeroLengthsCountPad, outputBytes);

    for (int i = lengthCountZero + 1, e = byteHeader.size(); i < e; ++i) {
        fputc((unsigned char)byteHeader[i].size(), outputBytes);
    }
    for (auto &i : byteHeader) {
        for (unsigned char c : i) {
            fputc(c, outputBytes);
        }
    }
    for (auto &byte : packBits.getBytesFromStream()) {
        fputc(byte, outputBytes);
    }
}

/**
 * Decoding and storing input bytes with help of FCFS method
 * If garbage bytes found the module skip it
 * Detecting situation when L8 has 256 characters
 */
void Static::decodeBytes(vector<unsigned char> &decodeBytes) {
    size_t position = 0;
    size_t actualBit = fileBytes[position];
    vector<size_t> byteLengths, beginCode, beginSymbol, othersSymbols;
    size_t symbolsLength = 0, fcfsCounterZero = 0, fcfsCounterFirst = 1;
    size_t counter = 0, index;
    size_t indexLength, initLengthCounter;
    unsigned char symbol;

    if(fileLength < actualBit + 1) {
        fprintf(stderr, "Something went wrong, skipping.\n");
        return;
    }
    position = position + 1;
    initLengthCounter = fileBytes[position];
    position = position + 1;
    pad = (initLengthCounter & 0b11100000) >> 5;
    bitPositionInByte = 0;
    initLengthCounter = initLengthCounter & ~ 0b11100000;
    actualBit = actualBit - initLengthCounter;
    while (counter < initLengthCounter) {
        counter = counter + 1;
        byteLengths.emplace_back(0);
    }
    for (index = 1 + actualBit; position < index; ++position) {
        indexLength = fileBytes[position];
        if (indexLength == 0 && initLengthCounter == CHAR_BIT -1) {
            indexLength = 256;
        }
        byteLengths.emplace_back(indexLength);
        symbolsLength = symbolsLength + indexLength;
    }
    if (fileLength < actualBit + symbolsLength + 1) {
        return;
    }
    for (index = actualBit + 1 + symbolsLength; position < index; ++position) {
        othersSymbols.emplace_back(fileBytes[position]);
    }
    countOfBytes = 1 + actualBit + symbolsLength;
    {
        beginCode.emplace_back(fcfsCounterZero);
        beginSymbol.emplace_back(fcfsCounterFirst);
        for (position = 0, index = byteLengths.size(); position < index; ++position) {
            fcfsCounterFirst = fcfsCounterFirst + byteLengths[position];
            fcfsCounterZero = (fcfsCounterZero + (byteLengths[position] - 1) + 1) << 1;

            beginCode.emplace_back(fcfsCounterZero);
            beginSymbol.emplace_back(fcfsCounterFirst);
        }
    }
    {
        fcfsCounterZero = 0, fcfsCounterFirst = 0;
        bool isItEnd = false;
        while (!isItEnd) {
            unsigned char byte = fileBytes[countOfBytes];
            bool bit = byte & (1 << (CHAR_BIT - bitPositionInByte - 1));
            if ((bitPositionInByte + 1) != CHAR_BIT) {
                bitPositionInByte = bitPositionInByte + 1;
            } else {
                countOfBytes = countOfBytes + 1;
                bitPositionInByte = 0;
            }
            bool condition = bit;
            fcfsCounterFirst = fcfsCounterFirst + 1;
            fcfsCounterZero = (fcfsCounterZero << 1) + condition;
            if ((fcfsCounterZero << 1) < beginCode[fcfsCounterFirst]) {
                index = beginSymbol[fcfsCounterFirst - 1] + fcfsCounterZero - beginCode[fcfsCounterFirst - 1] - 1;
                if (index >= symbolsLength) {
                    return;
                }
                symbol = othersSymbols[index];
                decodeBytes.emplace_back(symbol);
                fcfsCounterFirst = 0, fcfsCounterZero = 0;
            }
            if (!pad) {
                isItEnd = (countOfBytes == fileLength);
            } else {
                isItEnd = (countOfBytes + 1 == fileLength) && (bitPositionInByte == CHAR_BIT - pad);
            }
        }
    }
}