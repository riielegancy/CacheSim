#include <algorithm>
#include "cache.h"

Cache::Cache(uint32_t cacheSize, uint32_t blockSize, int nWays, bool debug){

    // Cache configuration
    this->cacheSize = cacheSize;
    this->blockSize = blockSize;
    this->nWays = nWays;

    // Cache stats
    this->hits = 0;
    this->totalAccesses = 0;
    this->debug = debug;

    // Cache number of sets
    int no_of_blocks= cacheSize/blockSize;
    if (nWays==0)
    {
      //fullyAssociated
      sets= no_of_blocks;
      this->indexBits= 0;

    }
    else if (nWays>=1)
    {
      sets= no_of_blocks/nWays;
      this->indexBits=log2(sets);
      //Direct mapped
    }



    // this->sets = 16;  // Currently hard coded

    // Address field bits
    this->offsetBits = log2(blockSize);
    // this->indexBits = 4;  // Currently hard coded.
    this->tagBits = ADDRESS_LENGTH - this->indexBits - this->offsetBits;

    // Initialize Cache
    cache.resize(sets);

    // Do not change the following print statements
    printf("------------------------------\n");
    printf("Cache size: %" PRIu32 " Bytes\nBlock Size: %" PRIu32 " Bytes\nSets: %d\nWays: %d\nTag bits: %d\nIndex bits: %d\nOffset bits: %d\n",
            this->cacheSize, this->blockSize, this->sets, this->nWays, this->tagBits, this->indexBits, this->offsetBits);
    printf("------------------------------\n");
}

bool Cache::isTagInCache(address_t addr)
{
  // Check if tag exists in the set.
  int set = addr.index;
  if (nWays == 0)
    set = 0;

  if (cache[set].find(addr.tag) != cache[set].end())
    return true;
  else
    return false;
}
void Cache::ResettagLRUbits(address_t addr)
{
  // Check if tag exists in the set.
  int set = addr.index;

  if (nWays == 0)
    set = 0;
    std::map<uint32_t, struct block>::iterator it = cache[set].find(addr.tag);
  if (it != cache[set].end())
      {
        it->second.LRU=0;
      }
}
void Cache::accessSet(address_t addr)
{
  int set = addr.index;
  if (nWays == 0)
    set = 0;
    std::map<uint32_t, struct block>::iterator it;

for (it = cache[set].begin(); it != cache[set].end(); ++it)
{
  it -> second.LRU++;

}
}
void Cache::findNextIndexEntry(address_t addr)
{
  int set = addr.index;
  if (nWays == 0)
    set = 0;
  int blocks = cacheSize/blockSize;
  if ((nWays == 0 && cache[set].size() < blocks) || (nWays > 0 && cache[set].size() < nWays))

  {  

    cache[set][addr.tag] = {.LRU = 0, .Valid = 1};
  }

  else
  {
    std::map<uint32_t, struct block>::iterator it;
    int max= -1;
    uint32_t tag=0;
    for (it = cache[set].begin(); it != cache[set].end(); ++it)
    {
      if (max < it->second.LRU)

{
          max = it->second.LRU;
          tag = it->first;
      }


    }
    cache[set].erase(tag);
    cache[set][addr.tag] = {.LRU = 0, .Valid = 1};
  }

}
bool Cache::accessCache(string address, int addressOffset){
    // Get address fields
    address_t addr = this->parseAddress(address, addressOffset);

    // Check for cache hit
    bool hit = isTagInCache(addr); // (cache[addr.index] == addr.tag);
    accessSet(addr);
    if(hit){
      ResettagLRUbits(addr);
        // Cache hit. Yay!
    }
    else // If miss, then load cache block
    {
      // int set = addr.index;
      // if (nWays == 0)
      //   set = 0;
      // cache[addr.index][addr.tag] = {.LRU = 0, .Valid = 1};
      findNextIndexEntry(addr);
    }

    // Update stats
    if(hit)
      this->hits += 1;
    this->totalAccesses += 1;

    return hit;
}



// DO NOT CHANGE FILE BELOW THIS LINE

address_t Cache::parseAddress(string address, int addressOffset){
    istringstream converter(address);
    uint64_t conversion;
    converter >> std::hex >> conversion;

    // Add the addressOffset to the address
    conversion += addressOffset;

    // Truncate to 32-bit address
    uint32_t conversion32;
    conversion32 = (uint32_t)(conversion);

    /*calculate the offset */
    unsigned int mask = (1 << this->offsetBits) - 1;
    uint32_t offset = conversion32 & mask;

    /*calculate the index */
    mask = (1 << this->indexBits) - 1;
    uint32_t index = (conversion32 >> this->offsetBits) & mask;

    /*calculate the tag */
    uint32_t tag = conversion32 >> (this->offsetBits + this->indexBits);

    if(this->debug){
        printf("Address: %s\n",address.c_str());
        printf("Offset: %d\n",addressOffset);
        printf("address + offest: %lu\n",(unsigned long)conversion32);
        printf("tag: %" PRIu32 "\n",(uint32_t)tag);
        printf("index: %" PRIu32 "\n",(uint32_t)index);
        printf("offset: %" PRIu32 "\n",(uint32_t)offset);

    }

    address_t parts = { (uint32_t)tag, (uint32_t)index, (uint32_t)offset} ;

    return parts;
}
void Cache::printStats(){
    if(this->totalAccesses == 0){
        printf("Error, Cannot get rate \n");
    }
    else{
        printf("Hit rate: %.2f%%\n", 100.00 * (double)(this->hits)/this->totalAccesses);
        printf("Miss rate: %.2f%%\n", 100.00 * (1 - (double)(this->hits)/this->totalAccesses));
    }
    printf("Hits: %d\n", this->hits);
    printf("total: %d\n",this->totalAccesses);
    printf("------------------------------\n\n");
}
