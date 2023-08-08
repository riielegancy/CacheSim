#ifndef __CACHE__H__
#define __CACHE__H__

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <inttypes.h>
#include <cmath>

#define ADDRESS_LENGTH 32

using namespace std;


struct block{
  int LRU;
  int Valid;
};

struct address_t{
    uint32_t tag;
    uint32_t index;
    uint32_t offset;
};

class Cache{
    private:
        uint32_t cacheSize;
        uint32_t blockSize;
        int sets;
        int nWays; //1: direct, 0: fully associative
        int indexBits, tagBits, offsetBits; //bit assignment
        int hits;
	// uint32_t cache[16]; // hard coded 16 entry cache
        std::vector<std::map<uint32_t, struct block> > cache;
        int totalAccesses;
        bool debug;

        address_t parseAddress(string, int);

    public:
        Cache(uint32_t, uint32_t, int, bool);
        bool isTagInCache(address_t);
        void ResettagLRUbits(address_t addr);
        bool accessCache(string, int);
        void printStats();
        void accessSet(address_t);
        void findNextIndexEntry(address_t addr);

};

#endif
