#ifndef SM_CPP
#define SM_CPP

#include "sm.h"

//Slotted PAge Implementation

char* SlottedPage::block = (char*) malloc(PAGE_SIZE);

// Initializes (formats) the slotted page
void SlottedPage::initBlock() {
    writeShort(block, (short)0, (short)0);
    writeShort(block, (short)2, (short)PAGE_SIZE);
}

char *SlottedPage::getRecord(short slotNum) {
    short n = readShort(block, (short)0);
    short recStart  = (short)readShort(block, (short)(4*slotNum+4));
    short recLength = readShort(block, (short)(4*slotNum+6));
    if(slotNum < n){
        char* data = readBytes(block, recStart, recLength);
        return data;
    }
    else{
        char* a = "-1";
        return a;
    }
}

short SlottedPage::storeRecord(char *record) {
    short numRecords = readShort(block, (short)0);
    short eofs = readShort(block, (short)2);
    short recLen = (short)length(record);

    //enough space for incoming record
    if(eofs-4*(numRecords+2) >= recLen){
        //fill record
        writeBytes(block, (eofs-recLen), record);
        //create slot
        writeShort(block, (numRecords+1)*4, eofs-recLen);
        writeShort(block, (numRecords+1)*4+2, recLen);
        //update headers
        writeShort(block, (short)0, numRecords+1);
        writeShort(block, (short)2, eofs-recLen);
        return numRecords+1;
    }
    else return -1; // Fill your implementation here
}

short SlottedPage::deleteRecord(short slotNum) {
    short numRecords = readShort(block, (short)0);
    short eofs = readShort(block, (short)2);
    
    if(slotNum < numRecords){
            short destStart  = (short)readShort(block, (short)(4*slotNum+4));
            short destLength = readShort(block, (short)(4*slotNum+6));
            moveBytes(block, eofs+destLength, eofs, destStart-eofs);

            moveBytes(block, 4*slotNum+4, 4*slotNum+8, (numRecords-slotNum-1)*4);

            for(int i=slotNum; i<numRecords-1; i++)
                writeShort(block, (short)(4*i+4), readShort(block, (short)(4*i+4))+destLength);
            
            writeShort(block, (short)0, numRecords-1);
            writeShort(block, (short)2, eofs+destLength);
            return eofs-4*(numRecords+1);
    }
    else return -1;
}



//Storage Manager Implementation
void StorageManager::createDB(const char* dbName, int initialNumBlocks){
    BufferManager *bm = BufferManager::getInstance();
    bm->createDB(dbName, initialNumBlocks);
    initDB(dbName);
}

void StorageManager::openDB(const char* dbName){
    BufferManager *bm = BufferManager::getInstance();
    bm->openDB(dbName);
}

void StorageManager::closeDB(const char* dbName){
    BufferManager *bm = BufferManager::getInstance();
    bm->closeDB(dbName);
}


void StorageManager::initDB(const char *dbName){
    char* data = (char*) malloc(BLOCK_SIZE);
    
    BufferManager *bm = BufferManager::getInstance();
    long size = DiskManager::sizeDB(dbName);
    
    //Pin first block and create free list header
    data = bm->pinPage(dbName, 1);
    writeInt(data, 0, 2);
    SlottedPage::block = data+4;
    SlottedPage::initBlock();
    bm->unpinPage(dbName, 1, true);
    
    //Pin all other one by one
    for(int i=2; i <= size-1; i++){
        data = bm->pinPage(dbName, i);
        writeInt(data, 0, i+1);
        bm->unpinPage(dbName, i, true);
    }
    
    //Last Block
    bm->pinPage(dbName, size);
    writeInt(data, 0, -1);
    bm->unpinPage(dbName, size, true);
}


void StorageManager::createObject(const char *dbName, const char *objectName){
    char* record = (char*) malloc(strlen(objectName)+5);
    
    //Obtain free block
    int allcBlock = allocateBlock(dbName);
    //cout<<"Block Allocated\n";
    
    BufferManager *bm = BufferManager::getInstance();
    char* data = bm->pinPage(dbName, 1);
    
    //Add new object to table
    int* allcBlockPtr = &allcBlock;
    memcpy(record, objectName, strlen(objectName));
    memcpy(record+strlen(objectName)+2, allcBlockPtr, 4);
    
    SlottedPage::block = data+4;
    
    //cout<<"Record:"<<record<<endl;
    int a = 0;
    int* as = &a;
    memcpy(as, record+strlen(record)+2, 4);
    //cout<<"int: "<<*as;
    int slot = SlottedPage::storeRecord(record);
    //cout<<"Created at slot:"<<slot<<endl;
    if(slot == -1){
        cout<<"Name of Object too large. Enter again with a shorter name"<<endl;
        freeBlock(dbName,allcBlock);
    }
    
    bm->unpinPage(dbName, 1, true);
}

int StorageManager::getFirstBlockId(const char *dbName, const char *objectName){
    BufferManager *bm = BufferManager::getInstance();
    char* data = bm->pinPage(dbName, 1);
    SlottedPage::block = data+4;
    
    int blockId = -1;
    //cout<<"Object: "<<objectName<<endl;
    
    //Iterate through records
    char* rec;
    int i = 0;
    while(1){
        rec = SlottedPage::getRecord(i);
        //cout<<i<<"  "<<rec<<"    "<<strlen(rec)<<endl;
        if(rec == "-1")
            break;
        else{
            char* name = (char*) malloc(strlen(rec));
            memcpy(name, rec, strlen(rec)+1);
            //cout<<strcmp(name,objectName)<<endl;
            if(strcmp(name,objectName) == 0){
                //cout<<"matched"<<endl;
                int* blockIdPtr = (int*) malloc(4);
                memcpy(blockIdPtr, rec+strlen(rec)+2, 4);
                blockId = *blockIdPtr;
                break;
            }
        }
        i++;
    }
    return blockId;
}

int StorageManager::allocateBlock(const char *dbName){
    BufferManager *bm = BufferManager::getInstance();
    
    //Read free space
    char* data = bm->pinPage(dbName,1);
    //cout<<"pinned again. Calling read int\n";
    int free = readInt(data, 0);
    //cout<<"Read int returned:"<<free<<endl;
    
    //Read next free space
    char* temp = bm->pinPage(dbName, free);
    //cout<<"pinnednewFree\n";
    int newFree = readInt(temp, 0);
    //cout<<newFree<<endl;
    bm->unpinPage(dbName, free, true);
    
    if(newFree == -1){
    cout<<"out of scope";
    return -1;
    }
    
    //Assign next free
    writeInt(data,0,newFree);
    bm->unpinPage(dbName, 1, true);
    return free;
}

void StorageManager::freeBlock(const char *dbName, int blockId){
    BufferManager *bm = BufferManager::getInstance();
    //Will be added to the start of the free list
    //Read initial free pointer
    char* data = bm->pinPage(dbName,1);
    int free = readInt(data, 0);
    
    //Write this to present block
    char* temp = bm->pinPage(dbName,blockId);
    writeInt(temp,0,free);
    bm->unpinPage(dbName, blockId, true);
    
    //Update free pointer
    writeInt(data, 0, blockId);
    bm->unpinPage(dbName, blockId, true);
}

void StorageManager::dropObject(const char *dbName, char *objectName){
    BufferManager *bm = BufferManager::getInstance();
    
    int blockId = getFirstBlockId(dbName, objectName);
    
    freeBlock(dbName, blockId);
    
    char* data = bm->pinPage(dbName, 1);
    SlottedPage::block = data+4;
    
    char* rec;
    int i = 0;
    while(1){
        rec = SlottedPage::getRecord(i);
        
        if(rec == "-1")
            break;
        else{
            char* name = (char*) malloc(strlen(rec));
            memcpy(name, rec, strlen(rec)+1);
            if(name == objectName){
                int* blockIdPtr = (int*) malloc(4);
                memcpy(blockIdPtr, rec+strlen(rec)+2, 4);
                blockId = *blockIdPtr;
                break;
            }
        }
        i++;
    }
    
    SlottedPage::deleteRecord(i);
}

static void resize_initDB(const char *dbName){
    //Resize DB
    DiskManager::resizeDB(dbName);
    
    //Init the new part
    char* data = (char*) malloc(BLOCK_SIZE);
    
    BufferManager *bm = BufferManager::getInstance();
    long size = DiskManager::sizeDB(dbName);
    
    //Pin first block and update free list header
    data = bm->pinPage(dbName, 1);
    writeInt(data, 0, size-RESIZE_BY);
    bm->unpinPage(dbName, 1, true);
    
    //Pin all other one by one
    for(int i = size-RESIZE_BY; i <= size-1; i++){
        data = bm->pinPage(dbName, i);
        writeInt(data, 0, i+1);
        bm->unpinPage(dbName, i, true);
    }
    
    //Last Block
    bm->pinPage(dbName, size);
    writeInt(data, 0, -1);
    bm->unpinPage(dbName, size, true);
}

#endif
