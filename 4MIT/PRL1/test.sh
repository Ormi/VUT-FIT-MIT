#!/bin/bash
# PRL Project 1
# @author Michal Ormos
# @email bks@stud.fit.vutbr.cz
# @date Match 2019

# input arguments check
if [ "$#" -gt 1 ]; then
    echo "Illegal number of parameters"
    exit 1
elif [ "$#" -eq 0 ]; then
    numbers=5; 
elif [ $1 -lt 1 ]; then
    echo "invalid amount of numbers"
    exit 1 
elif [ "$#" -eq 1 ]; then
    numbers=$1;
else
    numbers=5;
fi
	
# create random numbers file
dd if=/dev/urandom bs=1 count=$numbers of=numbers status=noxfer > /dev/null 2>&1
# dd if=/dev/random bs=1 count=$numbers of=numbers 2>/dev/null

# compile
mpic++ --prefix /usr/local/share/OpenMPI -o bks bks.cpp

# Calculating small numbers is uselles, deal with them manually
if [ $numbers -eq 1 ]; then
	processes=1
elif [ $numbers -eq 2 ]; then
	processes=1
elif [ $numbers -eq 3 ]; then
	processes=3
else
	# get the logarith of 2 of array length
	z=$(echo "l($numbers)/l(2)" | bc -l)
	zz=${z%.*}

	# get the log of 2 second time
	a=$(echo "l($zz)/l(2)" | bc -l)
	aa=${a%.*}

	# round to higher int
	b=$(echo "($a+0.9999999)/1" | bc)

	# round it to nearest higher power of 2
	c=$(echo "2^$b" | bc -l; )

	# substract 1 from result
	processes=$(echo "2*$c - 1" | bc -l;)
fi

#run
mpirun --prefix /usr/local/share/OpenMPI -np $processes bks
#remove
rm -f numbers