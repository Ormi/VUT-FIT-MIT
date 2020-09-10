###############################################################################
# Project for PDS ( Data Communications, Computer Networks and Protocols )
# @file pds18-rpc.py (3)
# @author Michal Ormos (xormos00)
# @email xormos00@stud.fit.vutbr.cz
# @date April 2019
###############################################################################

# peer message DONE
# peer getlist DONE
# peer peers DONE
# peer reconnect

# node database DONE
# node neighbors
# node connect
# node disconnect
# node sync

import argparse
import socket
import sys
import time
import json

def getNetwork():
	allLines = []
	with open("network.dat","r") as f:
		for line in f:
			allLines.append(line)
		f.close()
	return allLines

def defineStep(args):
	network = getNetwork()
	if args.peer:
		if args.command == "message":
			print("peer message")
			ip = False
			port = False
			peer = False
			username = False
			for line in network:
				jsondata = json.loads(line)
				for item in jsondata:
					if item == "ip":
						ip = jsondata[item]
					if item == "port":
						port = jsondata[item]
					if item == "type":
						if jsondata[item] == "peer":
							peer = True
					if item == "username":
						username = jsondata[item]
					if (str(username) == FROM):
						if (port and ip and peer):		
							sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
							server_address = (str(ip), int(port))
							message = b'MESSAGE'
							try:
								# send
								sent = sock.sendto(message, server_address)

								# Receive response
								data, server = sock.recvfrom(4096)

								# send2							
								message = {
									"to": args.to,
									"message": args.message
								}
								sent = sock.sendto(bytes(str(message), "utf-8"), server_address)
							finally:
								sock.close()								
				ip = False
				port = False
				peer = False	
				username = False		
		elif args.command == "getlist":
			print("peer getlist")
			ip = False
			port = False
			peer = False
			for line in network:
				jsondata = json.loads(line)
				for item in jsondata:
					if item == "ip":
						ip = jsondata[item]
					if item == "port":
						port = jsondata[item]
					if item == "type":
						if jsondata[item] == "peer":
							peer = True
					if (port and ip and peer):		
						sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
						server_address = (str(ip), int(port))
						message = b'GETLIST'
						try:
							# print('sending {!r}'.format(message))
							sent = sock.sendto(message, server_address)
						finally:
							# print('closing socket')
							sock.close()
				ip = False
				port = False	
				peer = False							
		elif args.command == "peers":
			print("peer peers")        
			ip = False
			port = False
			peer = False
			for line in network:
				jsondata = json.loads(line)
				for item in jsondata:
					if item == "ip":
						ip = jsondata[item]
					if item == "port":
						port = jsondata[item]
					if item == "type":
						if jsondata[item] == "peer":
							peer = True
					if (port and ip and peer):		
						sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
						server_address = (str(ip), int(port))
						message = b'PEERS'
						try:
							# print('sending {!r}'.format(message))
							sent = sock.sendto(message, server_address)
						finally:
							# print('closing socket')
							sock.close()
				ip = False
				port = False
				peer = False			    
		elif args.command == "reconnect":
			print("peer reconnect")   
			ip = False
			port = False
			peer = False
			for line in network:
				jsondata = json.loads(line)
				for item in jsondata:
					if item == "ip":
						ip = jsondata[item]
					if item == "port":
						port = jsondata[item]
					if item == "type":
						if jsondata[item] == "peer":
							peer = True
					if (port and ip and peer):		
						sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
						server_address = (str(ip), int(port))						
						print(server_address)
						message = b'RECONNECT'
						try:
							# send
							# print('sending {!r}'.format(message))
							sent = sock.sendto(message, server_address)

							# Receive response
							# print('waiting to receive')
							data, server = sock.recvfrom(4096)
							# print('received {!r}'.format(data))	

							# send2							
							message = {
								"ip": args.reg_ipv4,
								"port": args.reg_port
							}
							# print('sending {!r}'.format(message))
							sent = sock.sendto(bytes(str(message), "utf-8"), server_address)
						finally:
							# print('closing socket')
							sock.close()	
				ip = False
				port = False
				peer = False									
		else:
			print("Unknow command for Peer")         
	elif args.node:
		if args.command == "database":
			print("node database")   
			ip = False
			port = False
			node = False
			for line in network:
				jsondata = json.loads(line)
				for item in jsondata:
					if item == "ip":
						ip = jsondata[item]
					if item == "port":
						port = jsondata[item]
					if item == "type":
						if jsondata[item] == "node":
							node = True
					if (port and ip and node):		
						sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
						server_address = (str(ip), int(port))
						message = b'DATABASE'
						try:
							# print('sending {!r}'.format(message))
							sent = sock.sendto(message, server_address)
						finally:
							# print('closing socket')
							sock.close()
				ip = False
				port = False
				node = False			
		elif args.command == "neighbors":
			print("node neighbors")                          
			print("[ERROR] NOT IMPLEMENTED")                          
		elif args.command == "connect":
			print("node connect")    
			print("[ERROR] NOT IMPLEMENTED")                          
		elif args.command == "disconnect":
			print("node disconnect") 
			print("[ERROR] NOT IMPLEMENTED")                          
		elif args.command == "sync":
			print("node sync")                                         
			print("[ERROR] NOT IMPLEMENTED")                          
		else:
			print("Unknow command for Node")         
	else:
		print("Error, unknow arguments")



parser = argparse.ArgumentParser(description="Hybrid p2p chat application RPC module")
group = parser.add_mutually_exclusive_group()
# command_group = parser.add_mutually_exclusive_group()
group.add_argument("--peer", action='store_true',  help="Command for instance of peer")
group.add_argument("--node", action='store_true', help="Command for instance of server")

parser.add_argument("--command",  help="Command for instance of server")

parser.add_argument("--from", help="Command for instance of server")
parser.add_argument("--to", help="Command for instance of server")
parser.add_argument("--message", help="Command for instance of server")

parser.add_argument("--reg-ipv4", help="Command for instance of server")
parser.add_argument("--reg-port", help="Command for instance of server")

FROM = ""
args = parser.parse_args()
for arg in vars(args):
	try:
		if arg == "from":
			FROM = getattr(args, arg)
	except:
		pass
	# print(arg, getattr(args, arg))

defineStep(args)

# --peer --command message --from <username1> --to <username2> --message <zpráva>
