################################################################################
# @project Static and Adaptive Huffman Coding
# @author Michal Ormos
# @email xormos00@stud.fit.vutbr.cz
# @date March 2020
# @subject KKO
# @file Makefile
################################################################################

CC=g++
CFLAGS= -std=c++11 -Wall -Wextra -O3 -flto -static
OUT=huff_codec

all: $(OUT)

helper.o: helper.cpp helper.h
	$(CC) $(CFLAGS) -c $< -o $@

static.o: static.cpp static.h
	$(CC) $(CFLAGS) -c $< -o $@

adaptive.o: adaptive.cpp adaptive.h
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): main.o helper.o static.o adaptive.o
	$(CC) $(CFLAGS) $^ -o $@

debug:
	$(CC) -std=c++11 -Wall -Wextra -ggdb3 main.cpp helper.cpp static.cpp adaptive.cpp -o $(OUT)

clean:
	rm -f *.o $(OUT)

makezip:
	zip -r kko_xormos00.zip *.cpp *.h Makefile dokumentace.pdf