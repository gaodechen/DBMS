/** ----------------------------------------------------
 * CS305: Introduction to Database Systems
 * Department of Computer Science and Engineering
 * IIT Hyderabad
 *
 * File: dm.h
 * Header files for the class DiskManager
 *
 * Version 1.0 Initial
 * Version 1.1 Updated signature of rawRead
 * -----------------------------------------------------
 */

#ifndef DM_H
#define	DM_H

#include <iostream>
#include <vector>
#include <cstdio>

#include "global.h"

using namespace std;
	
    static vector<string> openName;
    static vector<int> openFd;

/**
 * Class to deal with direct writing of disk blocks. Current
 * implementation is over a file. Can be extended to work with
 * a raw disk partition as well.
 */
class DiskManager {
	//private
	
	public:
	/**
	 *Finds and returns the fd in the table of the given name
	 *otherwise -1
	 */
	static int getFd(const char* dbName);
	
	//static void addFd(const char* dbName);

    //public:
    /**
     * Create a new file and pre-allocates initialNumBlocks of
     * disk blocks (i.e., file = initialNumBlocks * BLOCK_SIZE).
     */
    static void createDB(const char *dbName, int initialNumBlocks);

    /**
     * Opens the file for the given dbName in random access mode.
     * If the db is already open, returns without doing anything.
     */
    static void openDB(const char *dbName);

    /**
     * Direct (unbufffered) write of disk block. If the db is
     * not open, throws an exception.
     */
    static void rawWrite(const char *dbName, int blockId, char *buff);

    /**
     * Direct (unbufffered) read of disk block. If the db is
     * not open, throws an exception. The block is read into
     * the buffer page pointed to by buff, it is assumed that
     * memory is already allocated for buff.
     */
    static void rawRead (const char *dbName, int blockId, char *buff);

    /**
     * Close the file.
     */
    static void closeDB(const char *dbName);
    
    /**
     * Increase filesize
     */
     static void resizeDB(const char* dbName);
    
    /*
     * Return the size of the opened database file
     */
     static int sizeDB(const char* dbName);
};


#endif	/* DM_H */

