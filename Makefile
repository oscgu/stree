stree: stree.c config.h
	cc -o $@ $< -Os -s -Wall -pedantic
