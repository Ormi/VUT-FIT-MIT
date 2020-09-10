#!/usr/bin/env python3
# ec.py
# KRY Project 2 - Find private key to PUBLIC key
# Michal Ormos (xormos00)
# xormos00@stud.fit.vutbr.cz
# April 2019

# source from which the code was implemented
## https://wis.fit.vutbr.cz/FIT/st/cwk.php?title=Projekt_2&src=Eliptick%E9_k%F8ivky_a_%B9ifrov%E1n%ED&ns=KRY&action=download&cache=no&csid=669637&id=12834

# libraries
import sys

# constants from assigment
# define curve
FP = 0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff
# A and B are constats for defining equation for curve
A = -0x3
B = 0x5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b
# base point
BASE_POINT = (0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296,
    0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5)
# Public key Q = (Qx, Qy)
#PUBLIC = (0x52910a011565810be90d03a299cb55851bab33236b7459b21db82b9f5c1874fe,
#    0xe3d03339f660528d511c2b1865bcdfd105490ffc4c597233dd2b2504ca42a562)
 
"""
calculateInverseElement
"""
def calculateInverseElement(element):
    return pow(element, FP-2, FP)

"""
ECSumTwoPoints    
    - sum of two point on elyptical curve
    graph is symetrical on 'x' axis, draw a line through them to find where the points are, and intercept them if exist
    - p+q = -r
"""
def ECSumTwoPoints(pointA, pointB):
    (Xp,Yp) = pointA
    (Xq,Yq) = pointB
    # calculating the slope of function
    slope = (Yq - Yp) * calculateInverseElement(Xq - Xp)
    slope = slope % FP
    Xr = (pow(slope,2) - Xp - Xq) % FP
    Yr = (slope * (Xp - Xr) - Yp) % FP
    return (Xr,Yr)

"""
findPrivateKey
"""
def findPrivateKey(p_key):
    # initialization of iteration factor
    i = 2

    # calculating EC point multiplication
    (Xp,Yp) = BASE_POINT
    # calculating the slope of function    
    slope = ((3 * pow(Xp,2) + A)) * (calculateInverseElement(2 * Yp))
    slope = slope % FP
    Xr = (pow(slope,2) - 2 * Xp) % FP
    Yr = (slope * (Xp - Xr) - Yp) % FP
    iterationOfP = (Xr,Yr)

    while (iterationOfP != p_key):
        i += 1
        iterationOfP = ECSumTwoPoints(BASE_POINT, iterationOfP)
    return i

"""
Main
    - process arguments from Makefile
    - parse input to desired Tuple and convert to int
"""
if __name__ == '__main__':
    PUBLIC_KEY = sys.argv[1]
    first_key = PUBLIC_KEY.split(',')[0]
    first_key = (first_key[1:])
    second_key = PUBLIC_KEY.split(',')[1]
    second_key = (second_key[:-1])   
    first_part_decode = int(first_key,0)     
    second_part_decode = int(second_key,0)     
    p_key = ((first_part_decode), (second_part_decode)) 
    print(findPrivateKey(p_key))
