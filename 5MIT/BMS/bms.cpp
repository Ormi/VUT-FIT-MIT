/**
 * @file bms.cpp
 *
 * @brief This is main for BMS project Demultiplexing and analysis of DVBT transport stdoutStream
 *
 * @date Nov 2019
 *
 * @author Michal Ormos
 * xormos00@stud.fit.vutbr.cz
 *
 * VUT FIT
 * BMS 2019/2020
 *
 */
 
// Common definitions
#include "common.h"
 
// Common libraries
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>

#define RETURN_SUCCESS 0
#define RETURN_ERROR 1
 
// Bad practice, but whatever, for school project it's fine
using namespace std;

/**
 * Function for opening input .ts file from stdin
 * @param filename string
 * Output file will be named same as input file
 * Make file a txt file as project description require
 * Open inputfile for Writing
 */
void inputStreamReader::openTS_Stream(string filename) {
	inputTSFile.open(filename, ios::binary);
	size_t eof = filename.find_last_of(".");
	string outputfile = filename.substr(0, eof).append(".txt");
	outputTSFile.open(outputfile);
}
 
/**
 * Function for reading input .ts file
 * Calling packetParser to proces this file
 * @param inputTSFile;
 * @param outputTSFile;
 * @param buffer[packetLength] uint8_t;
 * @param dmx Demultiplexor;
 * 
 * @return final string result 
 */ 
void inputStreamReader::readTS_Stream() {
	// Read the file until the end
    while(!inputTSFile.eof()) {
        inputTSFile.read(reinterpret_cast<char*>(buffer), packetLength);
		// If length is different than 188 break the reading
        if(inputTSFile.gcount() != packetLength) {
            break;
		}
		// add every packet to decoder
        dmx.packets++;
        Packet packetTS(buffer, inputTSFile.gcount());

		// parse packet
        packetTS.packetParser();
        dmx.packetsCounter[packetTS.getPID()]++;

		// If there is broken packetTS skip it
        if (packetTS.isEmpty() || packetTS.packetError()) {
            continue;
		}

		// Otherwise process it in Demultiplexor
        if(packetTS.getPID() == SDT && !dmx.analyzeServiceDescription) {
            dmx.parseServiceDescriptionTable(packetTS);
        }
        if (!dmx.analyzeProgramAssociation  && packetTS.getPID() == programAsociationTable) {
            dmx.parseProgramAssociationTable (packetTS);
        }
        else if(dmx.analyzeProgramAssociation  && dmx.isPacketProgramMapTable (packetTS.getPID())) {
            dmx.parseProgramMapTable(packetTS);
        }
        else if(!dmx.analyzeNetworkInformation && packetTS.getPID() == dmx.NITPid){
            dmx.parseNetworkInformationTable(packetTS);
        }        
    }
	// Calculate bitrates from packet
    dmx.calculateBitrate();
	// Delete unknown packet
    dmx.deleteUnknowPackets();
    // calculate bitrate
    dmx.bitratesCalculation();
    // bind maps
    dmx.bindMaps();
	// Print final results to file or stdout for debug
    dmx.printFinalResults(outputTSFile);
}
 
Packet::Packet() {
    rawData.resize(packetLength);
}
 
Packet::Packet(uint8_t *data, int index) {
    if(index != packetLength) {
        cerr << "Bad packetLength" << endl;
	}
    rawData.assign(data, data + index);
}

/**
 * Reading packet line from input stream
 * @param inputTSFile
 * if file is open read line othewise return false
 */
bool Packet::readPacketLine(ifstream &inputTSFile) {
    if(inputTSFile.is_open()) {
		inputTSFile.read(reinterpret_cast<char*>(rawData.data()), packetLength);
		return true;
	} else {
		return false;
	}
}
 
/**
 * Parsing packet
 */
bool Packet::packetParser() {
    uint8_t length = 0;

    adaptationFieldDataDescriptor.clear();
    payload.clear();
    packetHeader.synchronizationByte = rawData[0];
    if(packetHeader.synchronizationByte != 0x47) {
        cerr << "Error in synchronization byte\n";
        return RETURN_ERROR;
    }
    packetHeader.adaptationFieldDataDescriptor = rawData[3] >> 4 & 0x3;
    packetHeader.continuityCounter = rawData[3] & 0xF;
    packetHeader.packetError = rawData[1] >> 7;
    packetHeader.payloadBeginning = rawData[1] >> 6 & 0x1;
    packetHeader.PID = (rawData[1] & 0x1F) << 8 | rawData[2];
    packetHeader.transportPriority = rawData[1] >> 5 & 0x1;
    packetHeader.transportScrambling = rawData[3] >> 6;
 
	//Adaptation field is based on page 34
    if(packetHeader.adaptationFieldDataDescriptor == 3 || packetHeader.adaptationFieldDataDescriptor == 2) {
        length = rawData[4];
        adaptationFieldDataDescriptor.insert(adaptationFieldDataDescriptor.begin(), rawData.begin() + 4, rawData.begin() + 4 + length);
    }
    if (packetHeader.payloadBeginning) {
        length++;
	}
    if(packetHeader.adaptationFieldDataDescriptor == 3 || packetHeader.adaptationFieldDataDescriptor == 1) {
        payload.insert(payload.begin(), rawData.begin() + 4 + length, rawData.end());
    }
    return true;
 }
 
 int main(int argc, char **argv) {
    inputStreamReader stdoutStream;

    if (argc <= 1) {
        cerr << "You have to put at least one file argument" << endl;
        return RETURN_ERROR;
    }
    stdoutStream.openTS_Stream(argv[1]);
    stdoutStream.readTS_Stream();
}