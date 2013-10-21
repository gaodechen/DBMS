/** ----------------------------------------------------
 * CS305: Introduction to Database Systems
 * Department of Computer Science and Engineering
 * IIT Hyderabad
 *
 * File: sputil.c
 * Implementation of helper functions for the slotted page.
 * To be provided by the students.
 * -----------------------------------------------------
 */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "sputil.h"

using namespace std;

// Returns length of a record.  Assume records are NULL 
// terminated (not true in practice, but a shortcut).
inline int length(char *record) {
    return strlen(record)+5;
}

// Returns 2 bytes starting at block + offset as a short.
short readShort(char *block, short offset) {
    // Write your code here
    short val = 0;
    short *valPtr = &val;
    memcpy(valPtr, block+offset, 2);
    return *valPtr;
}

// Writes a 2 byte short value at block + offset
void writeShort(char *block, short offset, short value) {
    // Write your code here
    short val  =value;
    short* valPtr = &val;
    memcpy(block+offset, valPtr, 2);
}

// Returns 2 bytes starting at block + offset as a short.
int readInt(char *block, short offset) {
    // Write your code here
    int val = 0;
    int *valPtr = &val;
    memcpy(valPtr, block+offset, 4);
    return *valPtr;
}

// Writes a 2 byte short value at block + offset
void writeInt(char *block, short offset, int value) {
    // Write your code here
    int val  =value;
    int* valPtr = &val;
    memcpy(block+offset, valPtr, 4);
}

// Returns a fragment of the byte array
char *readBytes(char *block, short recOffset, short recLength) {
    // Write your code here
    char *data = (char *)calloc(recLength,sizeof(char));
    memcpy(data, block+recOffset, recLength);
    return data;
}

// Writes a fragment in the byte array
void writeBytes(char *block, short offset, char *rec) {
    // Write your code here
    memcpy(block+offset, rec, length(rec));
}

// Move a chunk of bytes. Use memcpy to implement this.
void moveBytes(char *block, short destOffset, short sourceOffset, 
               short chunkLength) {
    // Write your code here
    char *data = (char *)calloc(chunkLength,sizeof(char));
    memcpy(data, block+sourceOffset, chunkLength);
    memcpy(block+destOffset, data, chunkLength);
}

