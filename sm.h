/* 
 * File:   sm.h
 * Author: Me
 *
 * Created on October 25, 2010, 9:43 PM
 */

#ifndef SM_H
#define	SM_H
#include <iostream>
#include "sputil.h"
#include "bm.h"

#define PAGE_SIZE (BLOCK_SIZE-4)

using namespace std;

class SlottedPage {
    public:
    static char *block;  // The underlying buffer block, to which this 
                  // class is a wrapper.
    
    /**
     * Initialize (format) the block.
     */
    static void initBlock();

    /**
     * Retrieve a record at the given slot number. Returns -1
     * if the slot number is invalid.
     */
    static char *getRecord(short slotNum);

    /**
     * Stores a record if there is enough space.  Returns the 
     * slot number for the new record, and -1 if not enough space.
     */
    static short storeRecord(char *rec);

    /**
     * Delete a record given its slot number and returns the
     * space available.
     */
    static short deleteRecord(short slotNum);

    /**
     * Display the contents of the block
     */ 
    static void display();

};


/**
 * Keeps track of which blocks are used by which objects (the
 * object could be a table or index or any persistent entity.
 * Also maintains the free list to keep track of free (unallocated)
 * disk blocks.
 *
 * For simplicity assume all the metadata (head of the free list
 * and (object-name, firstBlockId) pairs are maintained in a
 * single block the very first block of the database.
 *
 * The StorageManager makes use of the BufferManager to fetch
 * the disk blocks in memory and write them back.
 */
class StorageManager {

    public:
    /**
     * Creates a new db and initializes it
     */
    static void createDB(const char* dbName, int initialNumBlocks);
    
    /**
     * Opens a new db it : Forward method for classes underneath
     */
    static void openDB(const char* dbName);
    
    /**
     * Closes a db it : Forward method for classes underneath
     */
    static void closeDB(const char* dbName);
    
    /**
     * Initializes a freshly created database by linking all the disk
     * blocks in the free list. To be called only once, just after
     * the creation of the database.
     */
    static void initDB(const char *dbName);

    /**
     * Creates an object and allocates the starting block
     * for it.
     */
    static void createObject(const char *dbName, const char *objectName);

    /**
     * Returns the id of the first (starting) block of an object.
     * In the case of a table, this would be the first block. In
     * the case of a B+-tree index, this would be the root node.
     */
    static int getFirstBlockId(const char *dbName,
                               const char *objectName);

    /**
     * Allocates a new block from the free list. Returns the
     * block-id.
     */
    static int allocateBlock(const char *dbName);

    /**
     * Frees (deallocates) a given block and adds it to the
     * free list.
     */
    static void freeBlock(const char *dbName, int blockId);

    /**
     * Drops (removes) an object (table/index) freeing its starting
     * block.
     */
    static void dropObject(const char *dbName, char *objectName);
    
    /**
     * Resizes and inits the new positions
     */
    static void resize_initDB(const char *dbName);
};

#endif	/* SM_H */

