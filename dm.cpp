#ifndef DM_CPP
#define	DM_CPP

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <string>

#include "dm.h"

using namespace std;

int DiskManager::getFd(const char* dbName){
    for(int i=0; i < openName.size(); i++)
        if (openName[i] == dbName)
            return openFd[i];
    
    return -1;
}

void DiskManager::createDB(const char *dbName, int initialNumBlocks) {
    string name = dbName;
    name += ".db";
    int fd = open(name.c_str(), O_CREAT|O_RDWR|O_TRUNC, S_IRUSR+S_IWUSR);
    
    if(fd == -1)   {cout<<"Error in creating database"; return;}
    
    fallocate(fd,0,0,initialNumBlocks*BLOCK_SIZE);
    
    openName.push_back(dbName);
    openFd.push_back(fd);
}

void DiskManager::openDB(const char *dbName) {
    string name = dbName;
    name += ".db";
    if(getFd(dbName) == -1){
        int fd = open(name.c_str(), O_RDWR);
        
        if(fd == -1)   {cout<<"Error in opening database"; return;}
        
        openName.push_back(dbName);
        openFd.push_back(fd);
    }
}

void DiskManager::rawWrite(const char *dbName, int blockId, char *buff) {
    int fd = getFd(dbName);
    
    if(fd == -1) {cout<<"Error : Database attempted to write in is not open";return;}
    
    pwrite(fd, buff, BLOCK_SIZE, BLOCK_SIZE*(blockId-1));
}

void DiskManager::rawRead (const char *dbName, int blockId, char *buff) {
    int fd = getFd(dbName);
    
    if(fd == -1) {cout<<"Error : Database attempted to write in is not open";return;}
       
    pread(fd, buff, BLOCK_SIZE, BLOCK_SIZE*(blockId-1));
}

void DiskManager::closeDB(const char *dbName) {
    int fd = getFd(dbName);
    
    if(fd == -1) {cout<<"Error : Database attempted to write in is not open";return;}
    
    close(fd);
    
    //remove from open list
    int i;
    for(i=0; i < openName.size(); i++)
        if (openName[i] == dbName)
            break;
            
    vector<string>::iterator vIt1;
    vector<int>::iterator vIt2;
    vIt1 = openName.begin();
    vIt2 = openFd.begin();
    advance(vIt1, i);
    advance(vIt1, i);
    openName.erase(vIt1);
    openFd.erase(vIt2);
}

void DiskManager::resizeDB(const char *dbName) {
    int fd = getFd(dbName);
    
    fallocate(fd,0,0,(sizeDB(dbName)+RESIZE_BY)*BLOCK_SIZE);
    
    openName.push_back(dbName);
    openFd.push_back(fd);
}

int DiskManager::sizeDB(const char* dbName){
    string name = dbName;
    name += ".db";
    struct stat st;
    stat(name.c_str(), &st);
    return (st.st_size/BLOCK_SIZE);
}

#endif	/* DM_CPP */
