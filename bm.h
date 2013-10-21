/* 
 * File:   bm.h
 * Author: Me
 *
 * Created on October 25, 2010, 9:48 PM
 */

#ifndef BM_H
#define	BM_H

#include <iostream>
#include <bitset>
#include <list>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "dm.h"

using namespace std;

/**
 * Buffer Manager for MuBase. Implements LRU replacement policy.
 */
 
typedef struct rec{
    int blockId;
    int index;
}rec;

typedef struct lruRec{
    const char* dbName;
    int blockId;
}lruRec;
 
class BufferManager {

    private:
    /**
     * BufferManager will have a singleton class (direct
     * instantiation is prohibited - so the constructor is
     * private.
     */
    static BufferManager *loneInstance;

    /**
     * The db name.
     */
    vector <const char* > dbName;
    vector <const char* >::iterator dIt1;

    /**
     * The actual buffer pool, which stores the pages.
     */
    char* buffer;
    
    // Store pincounts of blocks;
    int pinCount[NUM_BUFFS];
    
    //store wether dirty
    bitset<NUM_BUFFS> dirty;
    
    //hash storage of blockID
    vector < vector< list<rec> > > hashTable;
    list<rec>::iterator hIt1;
    vector < vector< list<rec> > >::iterator eIt1;
    
    //lru list
    list<lruRec> lru;
    list<lruRec>::iterator lruIt1;
    
    //free list
    list<int> freeList;
    list<int>::iterator flIt1;
    
    /**
     * Private constructor.
     */
    BufferManager() {
        // Open the db and allocate memory for the buffer pool
        // to hold NUM_BUFFS number of blocks.
        buffer = (char*) malloc( NUM_BUFFS*BLOCK_SIZE );
        dirty.reset();
        pinCount = {0};
        for(int i = 0; i < NUM_BUFFS; i++){
            freeList.push_back(i);
        }
    }

    public:

    static BufferManager *getInstance() {
        if(BufferManager::loneInstance == NULL){
            BufferManager::loneInstance = new BufferManager();}
        return BufferManager::loneInstance;
    }
    
    /**
    * Associate a db with the buffer
    */
    void createDB(const char* dbName, int initialNumBlocks);
    
    /**
     * Associate a db with the buffer
     */
    void openDB(const char* dbName);
    
    /**
     * Fetch the block in memory (if not already present), increment
     * its pinCount and return its address.
     */
    char* pinPage(const char* dbName, int blockId);

    /**
     * Reduce the pinCount of the buffer block. If the pinCount
     * becomes 0, the buffer block will be a candidate for being
     * evicted (eviction happens lazily when the need arises).
     */
    void unpinPage(const char*  dbName, int blockId, bool isDirty);

    /**
     * Forcibly write a given buffer block to the disk. Does not evict
     */
    void forcePage(const char* dbName, int blockId);
    
    /**
     * Close associated db
     */
    void closeDB(const char* dbName);
    
    /**
     * Destructor. Write all dirty pages to disk.
     */
     ~BufferManager();
     
    /**
     * Get index
     */
     int getIndex(const char* dbName);
     
    /**
     * Search in hashTable and return index lcocation
     */
     int searchHash(const char* dbName, int blockId);
     
     /**
      *Delete in hashTable and return index lcocation of deleted page
      */
     int delHash(const char* dbName, int blockId);
};



#endif	/* BM_H */

