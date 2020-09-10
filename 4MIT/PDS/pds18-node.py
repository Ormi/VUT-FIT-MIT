###############################################################################
# Project for PDS ( Data Communications, Computer Networks and Protocols )
# @file pds18-node.py (3)
# @author Michal Ormos (xormos00)
# @email xormos00@stud.fit.vutbr.cz
# @date April 2019
###############################################################################
# Compatible with PDS Virtual machine
# Socket for easy establishing communication between two points
import socket
# Threading for making more instancies easilt
import threading
# System calls
import sys
# Getting time
import time
# Parsing arguments
import argparse
# Creating random stream of numbers
from random import randint
# working with json
import json
import os
# library for bencoding
from bencoder import bencode
from bencoder import bdecode
# deep copy of list/dic
import copy

TXID = 1
TXID_LOCK = threading.Lock()
def next_id():
	global TXID
	with TXID_LOCK:
		result = TXID
		TXID += 1
	return result

class Server:
	db = []

	def RPCHandler(self):
		RPCid = findRPCPort()
		# Create a UDP socket
		sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		# Bind the socket to the port
		server_address = (REG_IPV4, int(RPCid))
		print('Node listening for RPC {} port {}'.format(*server_address))
		sock.bind(server_address)        
		while True:
			# we receiving data by connection and maximum data is
			data, address = sock.recvfrom(4096)
			print(data)
			if data == b'DATABASE':
				peersData = self.preparePeersData()
				print("===============")
				print("=== RPC DATABASE ===")
				print(peersData)
				print("===============")				
			elif data == b'NEIGHBORS':
				print("call getlist")
			elif data == b'CONNECT':
				print("call peers")
			elif data == b'DISCONNECT':  
				print("call reconnect")
			elif data == b'SYNC':  
				print("call reconnect")				

	
	def checkPeer(self, data):
		print("[INFO] checkPeer")
		timestamp = time.time()
		dbCopy = self.db
		dataCopy = copy.deepcopy(data)
		dataCopy.update({"time": timestamp})
		# data.update({"portID": port})
		peerID = 0
		found = False
		for key in data:
			if key == "username":
				peerID = data[key]
				break
		# First entry to database
		if not dbCopy:
			found = True
			dbCopy.append(dataCopy)
		# Check if entry is not already in database
		# if so just rewrite timestamp
		# if not add to db
		else:
			for peer in dbCopy:
				for entry in peer.keys():
					line = peer
					if entry == 'username':
						if str(line[entry]) == peerID:
							dbCopy.remove(line)
							dbCopy.append(data)
							found = True
							break
		if found == False:
			dbCopy.append(data)							
		self.db = dbCopy   
		return 

	def checkPeerHelloInterval(self):
		dbCopy = self.db
		actualTimestamp = time.time()
		for peer in dbCopy:
			line = peer
			for entry in line:
				if entry == 'timestamp':
					if actualTimestamp - line[entry] > 30:
						dbCopy.remove(line)
						break		
		self.db = dbCopy
		return						
		

	def parseMessage(self, data):
		for line in data:
			if line == 'ipv4':
				data[line] = data[line].decode("utf-8")
			if line == 'username':
				data[line] = data[line].decode("utf-8")
			if line == 'type':
				data[line] = data[line].decode("utf-8")
		return data

	def __init__(self):
		print("[INFO] init")
		print("Server is starting ...")
		# Start RPC daemon for listening to his messages
		RPCThread = threading.Thread(target=self.RPCHandler, args=())
		RPCThread.daemon = True
		RPCThread.start()		

		# Create a UDP socket
		sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

		# Bind the socket to the port
		server_address = (REG_IPV4, int(REG_PORT))
		print('Serverstarting up socket on {} port {}'.format(*server_address))
		sock.bind(server_address)

		while True:
			# print('\nwaiting to receive message')
			data, address = sock.recvfrom(4096)

			# print('received {} bytes from {}'.format(
				# len(data), address))
			data = data.decode("utf-8") 
			dataDecoded = bdecode(data)
			message = self.parseMessage(dataDecoded)
			# print(message)
			for line in message:
				self.checkPeerHelloInterval()
				if line == 'type':
					if message[line] == 'getlist':
						print("[SERVER] I got GETLIST")
						data = self.preparePeersData()
						bencoded = bencode(data)
						sent = sock.sendto(bencoded, address)
					elif message[line] == 'hello':
						print("[SERVER] I got HELO")
						self.checkPeer(message)
					elif message[line] == 'ack':
						print("ACK")
			# if data:
			#     sent = sock.sendto(data, address)
			#     print('sent {} bytes back to {}'.format(
			#         sent, address))

	def preparePeersData(self):
		print("[INFO] preparePeersData")
		next_id()
		dbCopy = self.db
		counter = 0
		peerRecords = []
		for peer in dbCopy:
			for entry in peer:
				line = peer
				if entry == 'username':
					username = line[entry]
				if entry == 'ipv4':
					ip = line[entry]
				if entry == 'port':
					port = line[entry]
			record = {str(counter): {
				"username": str(username),
				"ipv4": str(ip),
				"port": int(port)
			}}
			peerRecords.append(record)
			counter += 1
		data = {
			"type": "list",
			"txid": int(TXID),
			"peers": {}
		}
		for record in peerRecords:
			data["peers"].update(record)
		return data	


def checkwhoIAm(action, data):
	print("[INFO] checkwhoIam")
	weGotUserName = False
	weGotId = False
	allLines = []
	lineToRemove = ""
	if os.path.exists("network.dat"):
		with open("network.dat","r") as f:
			for line in f:
				allLines.append(line)
				jsondata = json.loads(line)
				for item in jsondata:
					# if item == "username":
					# 	if jsondata[item] == USERNAME:
					# 		weGotUserName = True
					if item == "id":
						if jsondata[item] == int(ID):
							lineToRemove = line
							weGotId = True
		f.close()
		if(action == "add"):
			with open("network.dat","a") as f:
				if (weGotId == False):
					f.write("\n")
					f.write(json.dumps(data))
			f.close()
		elif(action == "remove"):
			allLines.remove(lineToRemove)
			if len(allLines) != 0:
				with open("network.dat","w") as f:
					for line in allLines:
						# line = line[:-2]
						# if firstLine == True:
						# 	firstLine = False
						# else:
						# 	f.write("\n")						
						f.write(line)
				f.close()	
			else:
				os.remove("network.dat")
	else:
		with open("network.dat","w") as f:
			f.write(json.dumps(data))
			f.close()	


def findRPCPort():
	if(len(str(ID)) == 1):
		RPCid = ID + "999"
	if(len(str(ID)) == 2):
		RPCid = ID + "999"
	if(len(str(ID)) == 3):
		RPCid = ID + "99"
	if(len(str(ID)) == 4):
		RPCid = ID + "9"                                           
	return RPCid

# while True:
parser = argparse.ArgumentParser(description="Hybrid p2p chat application NODE module")
parser.add_argument("--id", type=int, required=True, help="--id us unique identifier of node instance for cases, where it is needed to differ between peer in case of oe host (OS), on which they are running")
parser.add_argument("--reg-ipv4", required=True, help="IP address on which node listening and receiving messages from other peers or nodes")
parser.add_argument("--reg-port", required=True, help="Port on which node listening and receiving messages from other peers or nodes")
args = parser.parse_args()
ID = str(args.id)
REG_IPV4 = str(args.reg_ipv4)
REG_PORT = str(args.reg_port)
whoIAm = {
	"type": "node",	
	"id": int(ID),
	"ip": str(REG_IPV4),
	"port": int(findRPCPort())
}	
checkwhoIAm("add", whoIAm)	
try:
	print("Trying to connect ...")
	try:
		server = Server()
	except KeyboardInterrupt:
		checkwhoIAm("remove", whoIAm)
		sys.exit(0)
	except Exception as e:
		print(e)
except KeyboardInterrupt:
	checkwhoIAm("remove", whoIAm)
	sys.exit(0)                        
