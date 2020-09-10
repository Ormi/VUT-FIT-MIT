/**
 * @file dmx.cpp
 *
 * @brief This is main for BMS project Demultiplexing and analysis of DVB-T transport stdoutStream
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

#include <string>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "common.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;


Demultiplexor::Demultiplexor() : NITPid(NIT), analyzeNetworkInformation(false), analyzeProgramAssociation (false), analyzeServiceDescription(false),
  packets(0), newServiceDescriptorTable(false), packetParser(false) {
}

/**
 * This calculation was adopt from source https://dveo.com/broadcast/bit-rate-calculation-for-COFDM-modulation.shtml
 * Author of this source has credit for this citation
 * Dveo copyright 2005
 * @param none
 */
void Demultiplexor::calculateBitrate() {
	// Packet length can be either 188 bytes or 204 bytes
	double secondPacketLength = 204;
	double packetTS = packetLength / secondPacketLength;
	long double A = 54000000 * packetTS;
	// Bandwidth is defined as 8MHz = 1
	long double bandwidth = getBandwidthValue() / 8;
	long double B = A * bandwidth;
	// Modulation QPSL 1/4
	long double C = B * getModulationScheme();
	// FEC Code rate can be define as 3/4
	long double D = C * getCodeRateValue();
	// Guad interval is defined as 1/16 = 16/17
	totalBitrate = D * getGuardValue();
}

/**
 * Final printing of results
 * Print to file or stdout debend on how the program is maked
 */
void Demultiplexor::printFinalResults(ostream& stdoutStream) {
#ifdef DEBUGMODE
	cout << "Network name: " << networkName << endl
		<< "Network ID: " << networkID << endl
		<< "Bandwidth: " << getBandwidth() << endl
		<< "Constellation: " << getConstellation() << endl
		<< "Guard interval: " << getGuardInterval() << endl
		<< "Code rate: " << getCodeRate() << endl;
	cout << endl;
	for(auto &program: programs)
		cout << "0x" << hex << setw(4) << setfill('0') << program.PID
		<< "-" << program.serviceProviderName << "-" << program.serviceName
		<< ": " << fixed << setprecision(2) << program.bitrate << " Mbps"
		<< endl;
#else
	stdoutStream << "Network name: " << networkName << endl
		<< "Network ID: " << networkID << endl
		<< "Bandwidth: " << getBandwidth() << endl
		<< "Constellation: " << getConstellation() << endl
		<< "Guard interval: " << getGuardInterval() << endl
		<< "Code rate: " << getCodeRate() << endl;
	stdoutStream << endl;
	for(auto &program: programs)
		stdoutStream << "0x" << hex << setw(4) << setfill('0') << program.PID
		<< "-" << program.serviceProviderName << "-" << program.serviceName
		<< ": " << fixed << setprecision(2) << program.bitrate << " Mbps"
		<< endl;
#endif	
}

void Demultiplexor::printErrorOutput(string errorOutput) {
	cerr << errorOutput << endl;
}

/**
 * Deleting unknow packets from stream
 * Maping programs to structure
 * Calculate bitrates
 * Sorting them by ID
 * @param none
 * @return sorted program
 */
void Demultiplexor::deleteUnknowPackets() {
	for(auto &packet: programElementary) {
		for(auto &elementary: packet.second) {
			if(packetsCounter.find(elementary) == packetsCounter.end()) {
				packet.second.erase(elementary);
			}
		}
	}
}

void Demultiplexor::bitratesCalculation() {
	for(auto &program: programs) {
		program.PID = programAsociationTable[program.serviceID];
		for(auto &epid: programElementary[program.serviceID]) {
			packetsCounter[program.PID] = packetsCounter[program.PID] + packetsCounter[epid];
		}
		program.bitrate = (packetsCounter[program.PID] 
						/ (long double)packets) * totalBitrate
						/ 1000000;
	}
}

void Demultiplexor::bindMaps() {
	sort(programs.begin(), programs.end());
}

/**
 * Decide if packet is from PAM Table
 * @param PID PacketIDentifier
 * @return bool if packet is from program table
 */
bool Demultiplexor::isPacketProgramMapTable (uint16_t PID) {
	if(PID == NITPid) {
		return false;
	}
	for(auto &program: programAsociationTable) {
		if (program.second == PID) {
			return true;
		}
	}
	return false;
}

/**
 * Take packet stream and parse PAT table from it
 * Inspired from ETSI EN 300 468 
 * https://www.etsi.org/deliver/etsi_en/300400_300499/300468/01.13.01_40/en_300468v011301o.pdf
 * @param packetTS packet stream
 * @return PAT Table in structure
 */
bool Demultiplexor::parseProgramAssociationTable (Packet &packetTS) {
	uint8_t tableByID = packetTS.payload[0];
	uint16_t lengthOfSection = (packetTS.payload[1] & HEXA_15) << 8  
							| packetTS.payload[2];
	uint8_t indexOfSection = 0;
	uint16_t numberOfProgram;
	uint16_t PID;

	if(PAS != tableByID) {
		printErrorOutput("[Error] Table ID will be set to " + HEXA_0);
		return false;
	}
	while(indexOfSection < lengthOfSection) {
		numberOfProgram = packetTS.payload[indexOfSection + 8] << 8 
						| packetTS.payload[indexOfSection + 9];
		PID = (packetTS.payload[indexOfSection + 10] & HEXA_31) << 8 
						| packetTS.payload[indexOfSection + 11];
		programAsociationTable.insert(make_pair(numberOfProgram, PID));
		if(numberOfProgram == HEXA_0) {
			NITPid = PID;
		}
		indexOfSection = indexOfSection + 4;
	}
	analyzeProgramAssociation = true;
	return true;
}

/**
 * Take packet stream and parse NIT table from it
 * Inspired from ETSI EN 300 468 
 * https://www.etsi.org/deliver/etsi_en/300400_300499/300468/01.13.01_40/en_300468v011301o.pdf
 * @param packetTS packet stream
 * @return NIT table in structure
 * table_id 8bits
 * network_id 16bits
 * network_lenth 12bits
 * length_of_trnasport_stream 12bits
 */
bool Demultiplexor::parseNetworkInformationTable(Packet &packetTS) {
	uint8_t tableByID = packetTS.payload[0];
	networkID = packetTS.payload[3] << 8 
			| packetTS.payload[4];
	uint16_t networkLength = (packetTS.payload[8] & HEXA_15) << 8 
							| packetTS.payload[9];
	parseDescriptorsTable(packetTS, 10, networkLength);
	uint16_t lengthOfTransportStream = (packetTS.payload[networkLength + 10] & HEXA_15) << 8 
									| packetTS.payload[networkLength + 11];
	uint16_t transportDescriptorLen;
	uint8_t indexOfPayload = networkLength + 12;
	uint8_t tansportStream = 0;	
	if(tableByID != NIS) {
		printErrorOutput("[Error] Wrong NIT Table ID");
		return false;
	}
	while(tansportStream < lengthOfTransportStream) {
		indexOfPayload = indexOfPayload + 4;
		transportDescriptorLen = (packetTS.payload[indexOfPayload] & HEXA_15) << 8 
										| packetTS.payload[indexOfPayload + 1];
		indexOfPayload = indexOfPayload + 2;;
		parseDescriptorsTable(packetTS, indexOfPayload, transportDescriptorLen);
		tansportStream = tansportStream + transportDescriptorLen + 6;
	}
	analyzeNetworkInformation = true;
	return true;
}

/**
 * Take packet stream and parse SDT table from it
 * Inspired from ETSI EN 300 468 
 * https://www.etsi.org/deliver/etsi_en/300400_300499/300468/01.13.01_40/en_300468v011301o.pdf
 * @param packetTS packet stream
 * @return SDT table in structure
 * section length 12btis
 */
bool Demultiplexor::parseServiceDescriptionTable(Packet &packetTS) {
	uint16_t lengthIndex = 0, serviceID, destinationLength;
	uint16_t lengthOfSection = (packetTS.payload[1] & HEXA_15) << 8 
							| packetTS.payload[2];
	if(packetTS.getPayloadStart()) {
		if(newServiceDescriptorTable) {
			packetParser = true;	
		}
		newServiceDescriptorTable = true;
	}
	if(!packetParser) {
		serviceDescriptorTableData.insert(serviceDescriptorTableData.end(), packetTS.payload.begin(), packetTS.payload.end());
	}
	if(packetParser) {
		while(lengthIndex < lengthOfSection) {
			serviceID = serviceDescriptorTableData[lengthIndex + 11] << 8 | serviceDescriptorTableData[lengthIndex + (11 + 1)];
			destinationLength = (serviceDescriptorTableData[lengthIndex + (11 + 3)] & HEXA_15) << 8 | serviceDescriptorTableData[lengthIndex + (11 + 4)];
			parseServiceDescriptorTable(lengthIndex + (11 + 5), destinationLength, serviceID);
			lengthIndex = lengthIndex + destinationLength + 5;
		}
	} else {
		return false;
	}
	packetParser = false;
	analyzeServiceDescription = true;
	return true;
}

/**
 * Take packet stream and parse PMT table from it
 * Inspired from ETSI EN 300 468 
 * https://www.etsi.org/deliver/etsi_en/300400_300499/300468/01.13.01_40/en_300468v011301o.pdf
 * @param packetTS packet stream
 * @return PMT table in structure
 * 
 */
bool Demultiplexor::parseProgramMapTable(Packet &packetTS) {
	uint8_t tableByID = packetTS.payload[0];
	if(tableByID != PMS) {
		printErrorOutput("[Error} PMT has wrong table ID");
		cerr << hex << "PID=0x" << packetTS.getPID() << endl;
		return false; 
	}
	uint16_t lengthOfSection = (packetTS.payload[1] & HEXA_15) << 8 
							| packetTS.payload[2];
	uint16_t numberOfProgram = packetTS.payload[3] << 8 
							| packetTS.payload[4];
	uint16_t programInfoLength = (packetTS.payload[10] & HEXA_15) << 8 
							| packetTS.payload[11];
	uint8_t indexOfPayload = programInfoLength + 12;
	uint8_t sectionIndex = 0;
	uint16_t elementaryPacketIdentifier, elementaryStreamInfoLength;

	while (sectionIndex < lengthOfSection) {
		elementaryPacketIdentifier = (packetTS.payload[indexOfPayload + sectionIndex + 1] & HEXA_31) << 8 | packetTS.payload[indexOfPayload + sectionIndex + 2];
		elementaryStreamInfoLength = (packetTS.payload[indexOfPayload + sectionIndex + 3] & HEXA_15) << 8 | packetTS.payload[indexOfPayload + sectionIndex + 4];
	
		if(elementaryPacketIdentifier != 0x1FFF)
			programElementary[numberOfProgram].insert(elementaryPacketIdentifier);
		sectionIndex += elementaryStreamInfoLength + 5;
	}
	return true;
}

/**
 * Take packet stream and parse SDT table from it
 * Inspired from ETSI EN 300 468 
 * https://www.etsi.org/deliver/etsi_en/300400_300499/300468/01.13.01_40/en_300468v011301o.pdf
 * @param offset
 * @param SDTLength length of SDT table
 * @param serviceID ID of service we want to parse
 * @return SDT table in structure
 */
void Demultiplexor::parseServiceDescriptorTable(uint16_t offset, uint16_t SDTLength, uint16_t serviceID) {
	uint16_t index = 0, indexOffset;
	uint8_t length, serviceProviderNameLength, serviceNameLength;
	ProgramInfo info;
	while(index < SDTLength) {
		indexOffset = index + offset;
		length = serviceDescriptorTableData[indexOffset + 1];
		if(serviceDescriptorTableData[indexOffset] == SERVCE_DESCRIPTOR) {
			serviceProviderNameLength = serviceDescriptorTableData[indexOffset + 3];
			info.serviceProviderName.assign(reinterpret_cast<char*>(serviceDescriptorTableData.data()) + indexOffset + 4, serviceProviderNameLength);
			serviceNameLength = serviceDescriptorTableData[indexOffset + serviceProviderNameLength + 4];
			info.serviceName.assign(reinterpret_cast<char*>(serviceDescriptorTableData.data()) + indexOffset + serviceProviderNameLength + 5, serviceNameLength);
			info.serviceID = serviceID;
			programs.push_back(info);
			break;
		} else {
			break;
		}
		break;
		index = index + length + 2;
	}

}

/**
 * Take packet stream and parse DT table from it
 * Inspired from ETSI EN 300 468 
 * https://www.etsi.org/deliver/etsi_en/300400_300499/300468/01.13.01_40/en_300468v011301o.pdf
 * @param packet packet from stream
 * @param offset
 * @param length
 * @return DT table in structure
 */
bool Demultiplexor::parseDescriptorsTable(Packet &packet, uint8_t offset, uint16_t length) {
	uint16_t PDTIndex = 0, PDTIndexOffset;
	uint8_t PDTLength;
	while(PDTIndex < length) {
		PDTIndexOffset = PDTIndex + offset;
		PDTLength = packet.payload[PDTIndexOffset + 1];
		if(packet.payload[PDTIndexOffset] == NETWORK_NAME_DESCRIPTOR) {
			networkName.assign(reinterpret_cast<char*>(packet.payload.data()) + PDTIndexOffset + 2, PDTLength);
			break;
		} 
		if (packet.payload[PDTIndexOffset] == TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR) {
			guardInterval = (packet.payload[PDTIndexOffset + 8] >> 3) & 0x3;
			codeRateHP = packet.payload[PDTIndexOffset + 7] & 0x7;
			constellation = packet.payload[PDTIndexOffset + 7] >> 6;
			bandwidth = packet.payload[PDTIndexOffset + 6] >> 5;
			break;
		} 
		PDTIndex = PDTIndex + PDTLength + 2;
	}
	return true;
}