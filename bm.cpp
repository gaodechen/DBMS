#ifndef BM_CPP
#define	BM_CPP

#include <algorithm>
#include <cstring>
#include <string>

#include "bm.h"

BufferManager *BufferManager::loneInstance = NULL;

void BufferManager::createDB(const char* dbName, int initialNumBlocks){
    DiskManager::createDB(dbName, initialNumBlocks);
    this->dbName.push_back(dbName);
    hashTable.resize(hashTable.size()+1);
    hashTable[this->dbName.size()-1].resize(10);
}


void BufferManager::openDB(const char* dbName){
    if(getIndex(dbName) == -1){
    DiskManager::openDB(dbName);
    this->dbName.push_back(dbName);
    hashTable.resize(hashTable.size()+1);
    hashTable[this->dbName.size()-1].resize(10);
    }
}

void BufferManager::closeDB(const char* dbName){
    //cout<<"in close db "<<dbName<<endl;
    int indexDb = getIndex(dbName);
        
    //Remove all blocks of the database from the buffer
    for(int i = 0; i < MOD_VALUE; i++){
        while(!hashTable[indexDb][i].empty()){
        forcePage(dbName, hashTable[indexDb][i].front().blockId);
        hashTable[indexDb][i].pop_front();
        }
    }
    
    //Remove entries from Hash Table
    eIt1 = hashTable.begin();
    advance(eIt1, indexDb);
    hashTable.erase(eIt1);
    
    //Remove entries from lru list
    for(lruIt1 = lru.begin(); lruIt1 != lru.end(); lruIt1++){
        //if(strcmp((*lru).dbName, dbName) == 0){
        if( (*lruIt1).dbName ==  dbName ){
            lru.erase(lruIt1);
            lruIt1--;
        }
    }

    //Remove from name list
    dIt1 = this->dbName.begin();
    advance(dIt1, indexDb);
    this->dbName.erase(dIt1);
    
    DiskManager::closeDB(dbName);
}

char* BufferManager::pinPage(const char* dbName, int blockId) {
    //Already in  Buffer ?
    //char* page = (char*) malloc(BLOCK_SIZE);
    //cout<<"Pinning : "<<blockId<<endl;
    
    int indexDb = getIndex(dbName);
    if(indexDb == -1) cout<<"Db Name record not found"<<endl;
    //cout<<"IndexDb:"<<indexDb<<endl;
    
    int index = searchHash(dbName, blockId);
    //cout<<"Index:"<<index<<endl;
    if(index != -1){
        //Already in the buffer
        //Present in the LRU list ?
        //cout<<"In buffer"<<endl;
        if(pinCount[index] == 0){
            //Present in the LRU list
            //Remove from LRU list
            //cout<<"In LRU list"<<endl;
            //lru.erase(find(lru.begin(),lru.end(),blockId));
            for(lruIt1 = lru.begin(); lruIt1 != lru.end(); lruIt1++){
                if( (*lruIt1).dbName == dbName && (*lruIt1).blockId == blockId){
                lru.erase(lruIt1);
                break;
                }
            }
            pinCount[index]++;
            //memcpy(page, buffer+BLOCK_SIZE*index, BLOCK_SIZE);
            return buffer+BLOCK_SIZE*index;
        }
        else{
            //Has already been pinned
            //cout<<"Not in LRU list\n";
            pinCount[index]++;
            //memcpy(page, buffer+BLOCK_SIZE*index, BLOCK_SIZE);
            return buffer+BLOCK_SIZE*index;
        }
    }
    else{
        //Not in the buffer
        //Read from disk
        //cout<<"Not in buffer"<<endl;
        char* page = (char*) malloc(BLOCK_SIZE);
        DiskManager::rawRead(dbName, blockId, page);        
        //Insert into buffer
        if(!freeList.empty()){
            //Buffer has empty space
            //cout<<"Buffer Has Empty Space\n";
            flIt1 = freeList.begin();
            index = *flIt1;
            freeList.pop_front();
            memcpy(buffer+BLOCK_SIZE*index, page, BLOCK_SIZE);
            
            //Add Entry to hash table
            int indexDb = getIndex(dbName);
            if(indexDb == -1) cout<<"Db Name record not found"<<endl;
            
            int val = blockId % MOD_VALUE;
            rec temp;
            temp.blockId = blockId;
            temp.index = index;
            hashTable[indexDb][val].push_back(temp);
            //Update pinCount and reset dirty bit
            pinCount[index] = 1;
            dirty.reset(index);
            return buffer+BLOCK_SIZE*index;
        }
        else{
            //Buffer does not have free space
            if(lru.empty())
            cout<<"Warning : buffer has no unpinned blocks";
            else{
                //Remove a block as per the lru algorithm
                //cout<<"Removing as per LRU"<<endl;
                const char* evictDbName = lru.front().dbName;
                int evictBlockId = lru.front().blockId;
                lru.pop_front();
                //cout<<"Evicting:"<<evictBlockId<<endl;
                forcePage(evictDbName, evictBlockId);
                int evictIndex = delHash(evictDbName, evictBlockId);
                
                //Insert the new block
                int indexDb = getIndex(dbName);
                if(indexDb == -1) cout<<"Db Name record not found"<<endl;
                
                int val = blockId % MOD_VALUE;
                rec temp;
                temp.blockId = blockId;
                temp.index = evictIndex;
                hashTable[indexDb][val].push_back(temp);
                pinCount[evictIndex] = 1;
                dirty.reset(evictIndex);
                memcpy(buffer+BLOCK_SIZE*evictIndex, page, BLOCK_SIZE);
                return buffer+BLOCK_SIZE*evictIndex;
            }
        }
    }
    //return page;
}

void BufferManager::unpinPage(const char* dbName, int blockId, bool isDirty) {
    //decrease pin count
    //cout<<"In unpin page for:"<<dbName<<" : "<<blockId<<endl;
    int index = searchHash(dbName, blockId);
    
    if(isDirty) dirty.set(index);
    
    if(pinCount[index] > 1)
        pinCount[index]-=1;
    else if(pinCount[index] == 1){
        pinCount[index]-=1;
        lruRec temp;
        temp.dbName = dbName;
        temp.blockId = blockId;
        lru.push_back(temp);
    }
}

void BufferManager::forcePage(const char* dbName, int blockId){
    int index = searchHash(dbName, blockId);
    //Is page dirty ?
    if(dirty.test(index)){
        //yes : write page to disk
        char* page = (char*) malloc(BLOCK_SIZE);
        memcpy(page, buffer+BLOCK_SIZE*index, BLOCK_SIZE);
        DiskManager::rawWrite(dbName, blockId, page);
        free(page);
        dirty.reset(index);
    }
}

BufferManager::~BufferManager(){
    //cout<<"Destructor called\n";
    //Force write all records
    //cout<<dbName.size()<<endl;
    
    //change to make close all dbs then delete structures
    
    for(int i=0; i < dbName.size(); i++){
        /*for(int j=0; j < MOD_VALUE; j++){
            while(!hashTable[i][j].empty()){
            forcePage(dbName[i], hashTable[i][j].front().blockId);
            hashTable[i][j].pop_front();
            }
        }*/
        
        closeDB(dbName[i]);
    }
    
    //cout<<"wrote all records";
    //delete the buffer and other structures
    dbName.clear();
    free(buffer);
    //free(pinCount);
    hashTable.clear();
    lru.clear();
    freeList.clear();
}

int BufferManager::getIndex(const char* dbName){
    //cout<<"Getting index for:"<<dbName<<endl;
    for(int i=0; i < this->dbName.size(); i++){
        //if( strcmp(this->dbName[i], dbName) == 0)
        if( this->dbName[i] == dbName )
            return i;
    }
    
    return -1;
}

int BufferManager::searchHash(const char* dbName, int blockId){
    int indexDb = getIndex(dbName);
    if(indexDb == -1) cout<<"Db Name record not found";
    
    int val = blockId%MOD_VALUE;
    for(hIt1 = hashTable[indexDb][val].begin(); hIt1 != hashTable[indexDb][val].end(); hIt1++){
        if ((*hIt1).blockId == blockId)
        return (*hIt1).index;
    }
    
    return -1;
}

int BufferManager::delHash(const char* dbName, int blockId){
    int indexDb = getIndex(dbName);
    if(indexDb == -1) cout<<"Db Name record not found";
    
    int val = blockId%MOD_VALUE;
    for(hIt1 = hashTable[indexDb][val].begin(); hIt1 != hashTable[indexDb][val].end(); hIt1++){
        if ((*hIt1).blockId == blockId){
        int temp = (*hIt1).index;
        hashTable[indexDb][val].erase(hIt1);
        return temp;
        }
    }
    return -1;
}

#endif	/* BM_CPP */
