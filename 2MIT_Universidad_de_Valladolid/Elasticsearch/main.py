#!/usr/bin/python3
# -*- coding: utf-8 -*-

__author__ = "Michal Ormos"
__copyright__ = "Copyright 2018, UVs"
__version__ = "1.0.1"
__maintainer__ = "Michal Ormos"
__email__ = "michal.ormos@alumnos.uva.es"
__status__ = "Educational"

import requests
import json
from elasticsearch import Elasticsearch
import time

if __debug__:
    print ('Debug ON')
else:
    print ('Debug OFF')

introduction_comment = """	
	This script demonstrate what I learn why using Elastich search in Python
	for purpose of demonstrate it in AWBGI class
	2018 UVa
"""

print(introduction_comment)
if __debug__:
    print ("No sleep")
else:
    time.sleep(3)

# Make sure our ES is running
print("#############\nConnection test\n#############")
res = requests.get('http://localhost:9200')
print(res.content)
print('\n')

if __debug__:
    print ("No sleep")
else:
    time.sleep(3)

# Connect to our cluster
es = Elasticsearch(
	[{'host': 'localhost', 'port': 9200}],
	sniff_on_start=True,
    # refresh nodes after a node fails to respond
    sniff_on_connection_fail=True,
    # and also every 60 seconds
    sniffer_timeout=60,
    # set sniffing request timeout to 10 seconds
    sniff_timeout=10)

# Index some test data
print("#############\nFirst data test\n#############")
es.index(index='test-index', doc_type='test', id=1, body={'test': 'test'})

# Test if they are there
res = es.get(index='test-index', doc_type='test', id=1)
print (json.dumps(res['_source'], indent=4, sort_keys=True))

# Delete test data and try with something more interesting
delete = es.delete(index='test-index', doc_type='test', id=1)
print(delete)

print('\n')
if __debug__:
    print ("No sleep")
else:
    time.sleep(3)

# Index some more complicated test data
print("#############\nSecond data test\n#############")
es.index(index='sw', doc_type='people', id=1, body={
	"name": "Luke Skywalker",
	"height": "172",
	"mass": "77",
	"hair_color": "blond",
	"birth_year": "19BBY",
	"gender": "male",
})

# Test if they are there
res = es.get(index='sw', doc_type='people', id=1)
print (json.dumps(res['_source'], indent=4, sort_keys=True))

es.delete(index='sw', doc_type='people', id=1)

print('\n')
if __debug__:
    print ("No sleep")
else:
    time.sleep(3)

# Lets add all SW people
print("#############\nLets add SW characters from SW API (This could take some time)\n#############")
r = requests.get('http://localhost:9200') 
i = 1
while r.status_code == 200:
    r = requests.get('http://swapi.co/api/people/'+ str(i))
    es.index(index='sw', doc_type='people', id=i, body=json.loads(r.content.decode('utf-8')))
    i=i+1
print("We added %d characters from API" %i)

print('\n')
if __debug__:
    print ("No sleep")
else:
    time.sleep(3)

# Search demonstration
print("#############\nWho is under id 5?\n#############")
res = es.get(index='sw', doc_type='people', id=5)
# print(res['_source'])
print (json.dumps(res['_source'], indent=4, sort_keys=True))
print('\n')

# Lets add more SW people
print("#############\nLets add more SW characters from SW API (This could take more time than last time)\n#############")
r = requests.get('http://localhost:9200') 
i = 18
while r.status_code == 200:
    r = requests.get('http://swapi.co/api/people/'+ str(i))
    es.index(index='sw', doc_type='people', id=i, body=json.loads(r.content.decode('utf-8')))
    i=i+1
print("We added %d more characters from API" %i)

print('\n')
if __debug__:
    print ("No sleep")
else:
    time.sleep(3)

es.indices.refresh(index="sw")

print('\n')
print("#############\nDo we have some Darth Vader here? Let's search him\n#############")
res = es.search(index="sw", body={"query": {"match" : { "name" : "Darth" }}})
print (json.dumps(res, indent=4, sort_keys=True))
print("Got %d Hits:" % res['hits']['total'])
print("#############\nSeems like we have found 2 Darth's!\n#############")

print('\n')
if __debug__:
    print ("No sleep")
else:
    time.sleep(3)

print("#############\nLet's see of some name start's with sting 'Lu'\n#############")
res = es.search(index="sw", body={"query": {"prefix" : { "name" : "lu" }}})

print (json.dumps(res, indent=4, sort_keys=True))

print('\n')
if __debug__:
    print ("No sleep")
else:
    time.sleep(3)

print("#############\nLet's find who is born in year 19\n#############")
res = es.search(index="sw", body={"query": {"prefix" : { "birth_year" : "19" }}})
print (json.dumps(res, indent=4, sort_keys=True))

print('\n')
if __debug__:
    print ("No sleep")
else:
    time.sleep(3)

print("#############\nLet's try som fuzzy query\n#############")
res = es.search(index="sw", body={"query": {"fuzzy" : { "name" : {"value" : "jaba", "max_expansions":5}}}})
print (json.dumps(res, indent=4, sort_keys=True))
print("#############\nWe found Jabba even we mispell his name!\n#############")