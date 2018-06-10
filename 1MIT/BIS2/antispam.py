#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
@name:		antispam.py
@type			BIS proj2
@author: 	Michal Ormos
@date:		Dec 2017
@VUT FIT Brno 2017
"""

"""
Libraries
"""
# for parsing text from emails
import email

from email import message_from_bytes
#from emaildata.text import Text

# arguments
import sys
import os

# scientific computing for Python
import numpy as np

# container datatypes
from collections import Counter

# free software machine learning
from sklearn.naive_bayes import MultinomialNB
from sklearn.svm import LinearSVC
from sklearn.metrics import confusion_matrix

# for saving/loading the model
import pickle
from sklearn.externals import joblib
from sklearn.svm import LinearSVC

DEBUG = 0

"""""""""""""""""""""""""""""""""
Functions for machine learning
"""""""""""""""""""""""""""""""""	    
def make_Dictionary(train_dir):
    emails = [os.path.join(train_dir,f) for f in os.listdir(train_dir)]    
    all_words = []       
    for mail in emails:    
        with open(mail) as m:
            for i,line in enumerate(m):
                if i == 2:
                    words = line.split()
                    all_words += words
    
    dictionary = Counter(all_words)
    
    list_to_remove = list(dictionary) #dictionary.keys()
    for item in list_to_remove:
        if item.isalpha() == False: 
            del dictionary[item]
        elif len(item) == 1:
            del dictionary[item]
    dictionary = dictionary.most_common(3000)
    return dictionary
    
def extract_features(mail_dir, dictionary): 
    files = [os.path.join(mail_dir,fi) for fi in sorted(os.listdir(mail_dir))]
    features_matrix = np.zeros((len(files),3000))
    docID = 0;
    for fil in files:
      with open(fil) as fi:
        for i,line in enumerate(fi):
          if i == 2:
            words = line.split()
            for word in words:
              #print(word)
              wordID = 0
              for i,d in enumerate(dictionary):
                if d[0] == word:
                  wordID = i
                  features_matrix[docID,wordID] = words.count(word)
        docID = docID + 1     
    return features_matrix

#for file as sys.argv
def extract_mail_sysargv(mail, dictionary): 
	#if DEBUG:
	print("[LOG] extracti_mail	Starting to check an email ")
	docID = 0;
	features_matrix = np.zeros((1,3000))
	with open(mail) as fi:
		for i,line in enumerate(fi):
		  if i == 0:
		    words = line.split()
		    for word in words:
		      wordID = 0
		      for i,d in enumerate(dictionary):
		        if d[0] == word:
		          wordID = i
		          features_matrix[docID,wordID] = words.count(word)
	return features_matrix

#for file as sys.argv
def extract_mail_string(mail, dictionary): 
	#if DEBUG:
	#print("[LOG] extracti_mail	Starting to check an email ")
	#print(mail)
	docID = 0;
	features_matrix = np.zeros((1,3000))
	if mail is not None:
		try:
			words = mail.split()
		except AttributeError:
			return 0
		for word in words:
		  wordID = 0
		  for ii,d in enumerate(dictionary):
		    if d[0] == word:
		      wordID = ii
		      features_matrix[docID,wordID] = words.count(word)
		return features_matrix
	else:
		return 0;


"""""""""""""""""""""""""""""""""
Functions for processing learning
"""""""""""""""""""""""""""""""""	 
def trainModel(model_name, dic_name):
	# Create a dictionary of words with its frequency	
	train_dir = 'train-mails'
	dictionary = make_Dictionary(train_dir)
	
	# Prepare feature vectors per training mail and its labels
	train_labels = np.zeros(702)
	train_labels[351:701] = 1
	train_matrix = extract_features(train_dir, dictionary)	

	# Training SVM and Naive bayes classifier and its variants
	#model1 = LinearSVC()
	model2 = MultinomialNB()	

	#model1.fit(train_matrix,train_labels)
	model2.fit(train_matrix,train_labels)	

	savePickel(dictionary, dic_name)
	# pickle.dump(dictionary, open(model_name, 'wb'))
	print("[LOG] Dictionary was saved")	

	savePickel(model2, model_name)
	# pickle.dump(model2, open(model_name, 'wb'))
	print("[LOG] Model was saved")	

def loadPickle(filename):
	with open(filename, 'rb') as pickle_file:
		content = pickle.load(pickle_file, encoding='iso-8859-1')	
	return content
	# return joblib.load(filename)

def savePickel(name ,filename):
	pickle.dump(name, open(filename, 'wb'))


# def loadDictionary(dic_name):
# 	return np.load(dic_name).item()

"""""""""""""""""""""""""""""""""	 
Main
"""""""""""""""""""""""""""""""""	 
# for all files on arguments line
model_filename = 'model2.sav'
# dictionary_filename = 'dictionary_2.sav'
# model_filename = 'finalized_eng_model.sav'
dictionary_filename = 'dictionary_eng.npy'

# trainModel(model_filename, dictionary_filename)
model = loadPickle(model_filename)
dictonary = loadPickle(dictionary_filename)
ham_counter = 0
spam_counter = 0
failed_counter = 0

for i in sys.argv:
	if (i != "./antispam"):	
		failed = False
		# raw_email = email.message_from_file(open(i))
		# print(raw_email)
		# content =  text.Text(raw_email)

		#current_email = email.message_from_file(open(i))
		try:
			with open(i, 'rb') as input_file:
				raw_email = input_file.read()
		except (FileNotFoundError, IsADirectoryError):
			print("%s - %s" % (i, "FAIL - FAILED TO OPEN FILE"))
			continue

		current_email = email.message_from_bytes(raw_email)

		if current_email.is_multipart():
			for load in current_email.get_payload():
				content = load.get_payload()
		else:
			content = current_email.get_payload()

		#print(content)
		test_matrix = extract_mail_string(content, dictonary)

		if test_matrix is not 0:
			result = model.predict(test_matrix)
		else:
			print("%s - %s" % (i, "FAIL - FAILED TO OPEN FILE"))
			result = [0]
			failed_counter = failed_counter + 1
			failed = True

		if failed is not True:
			if result[0]:
				result_string = "SPAM"
				spam_counter = spam_counter + 1
			else:
				result_string = "OK"
				ham_counter = ham_counter + 1
			print("%s - %s" % (i, result_string))
