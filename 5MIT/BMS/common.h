/**
 * @file common.h
 *
 * @brief Common variables and functions definitions for BMS project Demultiplexing and analysis of DVBT transport stdoutStream
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

// Shared libraries
#include <stdint.h>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>

using namespace std;

#ifndef COMMON_H
#define COMMON_H

// Define constants from ETSI EN 300 468 norm
// source taken from https://www.etsi.org/deliver/etsi_en/300400_300499/300468/01.13.01_40/en_300468v011301o.pdf
// Digital Video Broadcasting (DVB);
// Specification for Service Information (SI) in DVB systems 
// Start here

// Taken from Table 1, page 20
const uint8_t programAsociationTable = 0x00;
const uint8_t CAT = 0x01; // not needed
const uint8_t NIT = 0x10;
const uint8_t SDT = 0x11;

// Taken from Table 2, page 20
const uint8_t PAS = 0x00;
const uint8_t CAS = 0x01; // not needed
const uint8_t PMS = 0x02;
const uint8_t NIS = 0x40;
const uint8_t SDS = 0x42;

// Taken from Table 12, page 32
const uint8_t NETWORK_NAME_DESCRIPTOR = 0x40;
const uint8_t SERVCE_DESCRIPTOR = 0x48;
const uint8_t TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR = 0x5A;

// Taken from table 26, page 40
const uint8_t VIDEO_STREAM = 0x02; // not needed
const uint8_t AUDIO_STREAM = 0x03; // not needed
// Ends here

const uint8_t HEXA_0 = 0x00;
const uint8_t HEXA_15 = 0xF;
const uint8_t HEXA_31 = 0x1F;

// This magic number was taken from VUT FIT BMS Forum and documentation to project
static const int packetLength = 188;

class Packet {
	public:
		Packet();
		Packet(uint8_t *data, int n);
		vector<uint8_t> payload;
		bool readPacketLine(ifstream &in);
		bool packetParser();

		bool isEmpty() { 
			return packetHeader.PID == 0x1FFF; 
		}		
		bool packetError() { 
			return packetHeader.packetError; 
		}
		uint16_t getPID() { 
			return packetHeader.PID; 
		}
		bool getPayloadStart() { 
			return packetHeader.payloadBeginning == 1; 
		}
		uint8_t getContinuity() { 
			return packetHeader.continuityCounter; 
		}
	private:
		// Based on table 108
		// Number of bits
		struct Header {
			uint8_t synchronizationByte : 8;
			uint8_t packetError;
			uint8_t payloadBeginning: 1;
			uint8_t transportPriority: 1;
			uint16_t PID: 13;
			uint8_t transportScrambling : 2;
			uint8_t adaptationFieldDataDescriptor : 2;
			uint8_t continuityCounter : 4;
		};
		vector<uint8_t> rawData;
		Header packetHeader;
		vector<uint8_t> adaptationFieldDataDescriptor;
		string networkName;
	};

// Most information was taken from https://www.etsi.org/deliver/etsi_en/300400_300499/300468/01.13.01_40/en_300468v011301o.pdf
class Demultiplexor {
	public:
		Demultiplexor();
		bool parseNetworkInformationTable(Packet &packet);
		bool parseProgramAssociationTable (Packet &packet);
		bool parseServiceDescriptionTable(Packet &packet);
		bool parseProgramMapTable(Packet &packet);
		void printErrorOutput(string errorOutput);
		bool isPacketProgramMapTable (uint16_t PID);
		void printFinalResults(ostream& stdoutStream);

		void calculateBitrate();
		void deleteUnknowPackets();
		void bitratesCalculation();
		void bindMaps();		

		uint16_t NITPid;
		bool analyzeNetworkInformation;		
		bool analyzeProgramAssociation;
		bool analyzeServiceDescription;

		map<uint16_t, uint32_t> packetsCounter;
		map<uint16_t, set<uint16_t>> programElementary;
		uint32_t packets;

		string getBandwidth() { 
			if (bandwidth < bandwidthTable.size()) {
				return bandwidthTable[bandwidth];
			} else {
				return "Unknown";  
			}
		}
		string getConstellation() { 
			if (constellation < constellationCharacteristics.size()) {
				return constellationCharacteristics[constellation];
			} else {
				return "Unknown";  
			}
		}			
		string getGuardInterval() { 
			if (guardInterval < guardIntervalTable.size()) {
				return guardIntervalTable[guardInterval];
			} else {
				return "Unknown";  
			}
		}					
		string getCodeRate() { 
			if (codeRateHP < codeRateTable.size()) {
				return codeRateTable[codeRateHP];
			} else {
				return "Unknown";  
			}
		}				

		long double getBandwidthValue() { 
			return bandwidthValue[bandwidth]; 
		}
		long double getModulationScheme() {
			if (constellation == 0) {
				return 1/(long double)4;
			} else if (constellation == 1) {
				return 1/(long double)2.0;
			} else if (constellation == 2) {
				return 3/(long double)4.0;
			} else {
				return 0.0;
			}
		}
		long double getCodeRateValue() {
			if (codeRateHP == 0) {
				return 1/(long double)2.0;
			} else if (codeRateHP == 1) {
				return 2/(long double)3.0;
			} else if (codeRateHP == 2) {
				return 3/(long double)4.0;
			} else if (codeRateHP == 3) {
				return 5/(long double)6.0;
			} else if (codeRateHP == 4) {
				return 7/(long double)8.0;
			} else {
				return 0.0;
			}
		}
		long double getGuardValue() {
			if (guardInterval == 0) {
					return 32/(long double)33.0;
			} else if (guardInterval == 1) {
					return 16/(long double)17.0;
			} else if (guardInterval == 2) {
					return 8/(long double)9.0;
			} else if (guardInterval == 3) {
					return 4/(long double)5.0;
			} else {
				return 0.0;
			}
		}
	private:
		bool parseDescriptorsTable(Packet &packet, uint8_t offset, uint16_t length);
		void parseServiceDescriptorTable(uint16_t offset, uint16_t length, uint16_t serviceID);
		ofstream outputFile;
		map<uint16_t, uint16_t> programAsociationTable;
		bool newServiceDescriptorTable;
		bool packetParser;
		string networkName;
		string serviceProviderName;
		string serviceName;

		uint16_t networkID;
		
		uint8_t bandwidth;
		uint8_t constellation;
		uint8_t codeRateHP;
		uint8_t guardInterval;
		
		unsigned long totalBitrate;

		vector<uint8_t> serviceDescriptorTableData;

		// Taken from table 43 of reference document
		vector<long double> bandwidthValue = { 8.0, 7.0, 6.0, 5.0, 1.7 };
		vector<string> bandwidthTable = { "8 MHz", "7 MHz", "6 MHz", "5 MHz", "1,7 MHz"	};
		// Taken from table 45 of reference document
		vector<string> constellationCharacteristics = {"QPSK", "16-QAM", "64-QAM" };
		// Taken from table 48 of reference document
		vector<string> codeRateTable = { "1/2", "2/3", "3/4", "5/6", "7/8" };
		// Taken from table 49 of reference document
		vector<string> guardIntervalTable = { "1/32", "1/16", "1/8", "1/4" };

		struct ProgramInfo {
			uint16_t PID;
			uint16_t serviceID;
			string serviceProviderName;
			string serviceName;
			long double bitrate;

			bool operator<(const ProgramInfo &channel) const {
				return PID < channel.PID;
			}

		};
		vector<ProgramInfo> programs;
	};

class inputStreamReader {
	public:
		void openTS_Stream(string);
		void readTS_Stream();
	private:
		ifstream inputTSFile;
		ofstream outputTSFile;
		uint8_t buffer[packetLength];
		Demultiplexor dmx;
};

#endif