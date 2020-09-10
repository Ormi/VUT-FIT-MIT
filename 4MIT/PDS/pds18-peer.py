###############################################################################
# Project for PDS ( Data Communications, Computer Networks and Protocols )
# @file pds18-peer.py (3)
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

TXID = 1
TXID_LOCK = threading.Lock()
def next_id():
	global TXID
	with TXID_LOCK:
		result = TXID
		TXID += 1
	return result

REG_IPV4 = ""
REG_PORT = 0

class Client:
	def RPCHandler(self):
		RPCid = self.findRPCPort()
		# Create a UDP socket
		sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		# Bind the socket to the port
		server_address = (CHAT_IPV4, int(RPCid))
		print('Peer listening for RPC {} port {}'.format(*server_address))
		sock.bind(server_address)        
		while True:
			# we receiving data by connection and maximum data is
			data, address = sock.recvfrom(4096)
			print(data)
			if data == b'MESSAGE':
				print("===============")
				print("=== RPC Message ===")
				print("=== ack sent ===")
				print("===============")		
				try:
					# Send data
					data = b'ACK'
					# print('sending {!r}'.format(data))
					sent = sock.sendto(data, address)
					# Receive response
					# print('waiting to receive')
					data, server = sock.recvfrom(4096)
					# print('received {!r}'.format(data))
				finally:
					print(data)
					self.sendMessage(data)
					# print('closing socket')
					# self.sendMessage("a")  
			elif data == b'GETLIST':
				print("===============")
				print("=== RPC Getlist ===")
				print("=== ack sent ===")
				print("===============")				
				result = self.sendGetList()
			elif data == b'PEERS':
				result = self.sendGetList()
				print("===============")
				print("=== RPC Peers ===")
				print(result)
				print("===============")
			elif data == b'RECONNECT':  
				print("===============")
				print("=== RPC RECONNECT ===")
				print("===============")		
				try:
					# Send data
					data = b'ACK'
					# print('sending {!r}'.format(data))
					sent = sock.sendto(data, address)
					# Receive response
					# print('waiting to receive')
					data, server = sock.recvfrom(4096)
					# print('received {!r}'.format(data))
				finally:
					# print(data)
					self.reconnect(data)
					# print('closing socket')
					# self.sendMessage("a")  

	def prepareData(self):
		next_id()
		data = {
			"type": "hello",
			"txid": TXID,
			"username": (USERNAME),
			"ipv4": str(CHAT_IPV4),
			"port": int(CHAT_PORT)
		}
		return data

	def reconnect(self, data):
		global REG_IPV4, REG_PORT
		print("[INFO] Reconnect")
		data2 = data.decode("utf-8")
		data2 = data2.replace("'", '"')
		data3 = json.loads(data2)
		REG_IPV4 = data3["ip"]
		REG_PORT = data3["port"]


	def sendHello(self):
		# global REG_IPV4, REG_PORT
		helloTime = time.time()
					
		firstHello = True
		while True:
			if (time.time() - helloTime) > 10 or firstHello:
				# Create a UDP socket
				sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

				# print(REG_IPV4, REG_PORT)

				server_address = (REG_IPV4, int(REG_PORT))      
				firstHello = False
				data = self.prepareData()
				bencoded = bencode(data)
				try:
					# Send data
					# print('sending {!r}'.format(bencoded))
					# print("to")
					# print(server_address)
					# print(bencoded)
					sent = sock.sendto(bencoded, server_address)
					# Receive response
					# print('waiting to receive')
					# data, server = sock.recvfrom(4096)
					# print('received {!r}'.format(data))

				finally:
					# print('closing socket')
					sock.close()
				helloTime = time.time() 

	def sendMessage(self, data):
		print("[INFO] Send message")
		data2 = data.decode("utf-8")
		data2 = data2.replace("'", '"')
		data3 = json.loads(data2)
		to = data3['to']
		message = data3['message']
		users = self.sendGetList()
		for item in users:
			if item == "peers":
				for entry in users[item]:
					user = (users['peers'][str(entry)])
					if user['username'].decode("utf-8") == to:
						("here2")
						ip = (user['ipv4']).decode("utf-8")
						port = (user['port']) 
		
		# Create a UDP socket
		sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

		server_address = (ip, int(port))
		next_id()
		data = {
			"type":"message",
			"txid": TXID,
			"from": str(USERNAME),
			"to": str(to),
			"message":str(message)
		}
				 
		while True:
			bencoded = bencode(data)
			try:
				# Send data
				# print('sending {!r}'.format(bencoded))
				sent = sock.sendto(bencoded, server_address)

				# Receive response
				# print('waiting to receive')
				data, server = sock.recvfrom(4096)
				print('received {!r}'.format(data))

			finally:
				# print('closing socket')
				sock.close()
				return

	def sendACK(self):
		# Create a UDP socket
		sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

		server_address = (REG_IPV4, int(REG_PORT))
		next_id()
		data = {
			"type":"ack", 
			"txid": TXID
		}
				 
		while True:
			bencoded = bencode(data)
			try:
				# Send data
				# print('sending {!r}'.format(bencoded))
				sent = sock.sendto(bencoded, server_address)
			finally:
				# print('closing socket')
				sock.close()
			return


	def sendGetList(self):
		# Create a UDP socket
		sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

		server_address = (REG_IPV4, int(REG_PORT))

		data = {
			"type":"getlist", 
			"txid": TXID
		}
				 
		while True:
			bencoded = bencode(data)
			try:
				# Send data
				# print('sending {!r}'.format(bencoded))
				sent = sock.sendto(bencoded, server_address)

				# Receive response
				# print('waiting to receive')
				data, server = sock.recvfrom(4096)
				decodedData = bdecode(data)
				# print('received {!r}'.format(decodedData))
				self.sendACK()
			finally:
				# print('closing socket')
				sock.close()
			return decodedData

	"""
	Port for communication with RPC is generated as id with 9's until length is 5
	"""
	def findRPCPort(self):
		if(len(str(ID)) == 1):
			RPCid = ID + "999"
		if(len(str(ID)) == 2):
			RPCid = ID + "999"
		if(len(str(ID)) == 3):
			RPCid = ID + "99"
		if(len(str(ID)) == 4):
			RPCid = ID + "9"                                           
		return RPCid

	def __init__(self, sock):
		print("Peer started ...")
		# Start RPC daemon for listening to his messages
		RPCThread = threading.Thread(target=self.RPCHandler, args=())
		RPCThread.daemon = True
		RPCThread.start()

		helloThread = threading.Thread(target=self.sendHello, args=())
		helloThread.daemon = True
		helloThread.start()        

		# Get some time variable out of while loop
		helloTime = time.time()

		while True:       
			# print('\nwaiting to receive message')
			data, address = sock.recvfrom(4096)

			# print('received {} bytes from {}'.format(
				# len(data), address))
			data = data.decode("utf-8") 
			dataDecoded = bdecode(data)
			message = self.parseMessage(dataDecoded)
			for line in message:
				if line == 'type':
					if message[line] == 'message':
						print("[SERVER] I got message from someone")
						print(message)
						bencoded = bencode(b'ACK')
						sent = sock.sendto(bencoded, address)

	def parseMessage(self, data):
		for line in data:
			if line == 'ipv4':
				data[line] = data[line].decode("utf-8")
			if line == 'username':
				data[line] = data[line].decode("utf-8")
			if line == 'type':
				data[line] = data[line].decode("utf-8")
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
parser = argparse.ArgumentParser(description="Hybrid p2p chat application PEER module")
parser.add_argument("--id", type=int, required=True, help="--id us unique identifier of peer instance for cases, where it is needed to differ between peer in case of oe host (OS), on which they are running")
parser.add_argument("--username", required=True, help="Unique username identifing this peer within the chat")
parser.add_argument("--chat-ipv4", required=True, help="IP address on which peer listening and receiving messages from other peers or nodes")
parser.add_argument("--chat-port", required=True, help="Port on which peer listening and receiving messages from other peers or nodes")
parser.add_argument("--reg-ipv4", required=True, help="IP address of egistration node, on which peer send messages HELLO and GETLIST")        
parser.add_argument("--reg-port", required=True, help="Port of egistration node, on which peer send messages HELLO and GETLIST")
args = parser.parse_args()
ID = str(args.id)
USERNAME = str(args.username)
CHAT_IPV4 = str(args.chat_ipv4)
CHAT_PORT = str(args.chat_port)
REG_IPV4 = str(args.reg_ipv4)
REG_PORT = str(args.reg_port)
whoIAm = {
	"type": "peer",	
	"username": str(USERNAME),
	"id": int(ID),
	"ip": str(CHAT_IPV4),
	"port": int(findRPCPort())
}	
checkwhoIAm("add", whoIAm)
try:
	print("Trying to connect ...")
	# between 1 to 5 seconds
	# time.sleep(randint(1,5))
	# for peer in p2p.peers:
	try:

		# Create a UDP socket
		sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		# Bind the socket to the port
		server_address = (str(CHAT_IPV4), int(CHAT_PORT))
		print('Starting up Message socket on {} port {}'.format(*server_address))
		sock.bind(server_address)		
		client = Client(sock)
		# clientPeer = clientPeer(peer)
	except KeyboardInterrupt:
		checkwhoIAm("remove", whoIAm)
		sys.exit(0)
	except:
		pass				
except KeyboardInterrupt:
	checkwhoIAm("remove", whoIAm)
	sys.exit(0)        