#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>

#include "bt.h"

int main() {
    /*
    // More operations - can be menu driven.
    char* page = (char*) malloc(BLOCK_SIZE);
    memcpy(page, "ankit has a dog", 15);
    DiskManager::rawWrite("testdb", 1, page);
    //delete(page);
    
    char* page1 = (char*) malloc(BLOCK_SIZE);
    memcpy(page1, "bekaar hai sab", 14);
    DiskManager::rawWrite("testdb", 2, page1);
    //delete(page1);
    
    DiskManager::createDB("testdb1", 1000);
    
    char* page2 = (char*) malloc(BLOCK_SIZE);
    memcpy(page2, "bekaar hai sab", 14);
    DiskManager::rawWrite("testdb1", 1, page2);
    //delete(page2);
    
    char* page3 = (char*) malloc(BLOCK_SIZE);
    memcpy(page3, "rushi", 5);
    DiskManager::rawWrite("testdb", 3, page3);
    delete(page3);

    char* page4 = (char*) malloc(BLOCK_SIZE);
    memcpy(page4, "ankit has a black cat", 21);
    DiskManager::rawWrite("testdb1", 4, page4);
    delete(page4);
    
    DiskManager::closeDB("testdb");
    DiskManager::openDB("testdb");
    DiskManager::closeDB("testdb1");

    char * dabba = (char*) malloc(BLOCK_SIZE);
    DiskManager::rawRead("testdb", 2, dabba);
    cout<<dabba<<endl;
    
    DiskManager::closeDB("testdb");
    
    int a;
    cin>>a;
    
    
//    cout<<"created";
//    DiskManager::openDB("testdb");
//    cout<<"opened";
    
      char* pala = (char*) malloc(BLOCK_SIZE);
//    memcpy(page, "ankit has a dog", 15);
//    DiskManager::rawWrite("testdb", 1, page);
//      cout<<"Dabba gol"<<endl;

      BufferManager *temp = BufferManager::getInstance();
      //char* dbName1 = (char*) malloc(7);
      //memcpy(dbName1, "testdb", 7);
      
      //char* dbName2 = (char*) malloc(8);
      //memcpy(dbName2, "testdb1", 8);
      cout<<"asdasd";
      
      temp->openDB("testdb");
      
      pala = temp->pinPage("testdb", 1);
      cout<<"Data (1): "<<pala<<endl;
      int w = 4322243;
      int* wer = &w;
      memcpy(pala, wer, 4);
      temp->unpinPage("testdb", 1,true);
      
      pala = temp->pinPage("testdb", 1);
      cout<<"Edited data (1) : "<<pala<<endl;
      int* wed = (int*) malloc(4);
      memcpy(wed, pala, 4);
      cout<<"Edited Number:"<<*wed<<endl;
      temp->unpinPage("testdb", 1,false);
      
      pala = temp->pinPage("testdb", 2);
      cout<<"Data (2): "<<pala<<endl;
      temp->unpinPage("testdb", 2,false);
      
      temp->openDB("testdb1");
      
      pala = temp->pinPage("testdb1", 1);
      cout<<"Data (1): "<<pala<<endl;
      temp->unpinPage("testdb1", 1,false);
      
      pala = temp->pinPage("testdb1", 1);
      cout<<"Data (1): "<<pala<<endl;
      memcpy(pala, "bewaa", 5);
      temp->unpinPage("testdb1", 1,true);
      
      pala = temp->pinPage("testdb1", 1);
      cout<<"Edited data (1): "<<pala<<endl;
      temp->unpinPage("testdb1", 1,false);
      //temp->forcePage(1);
      
      //cout<<"came here\n";
      temp->closeDB("testdb");
      //cout<<"came here too";
      temp->closeDB("testdb1");
      //cout<<"came out too\n";
      temp->~BufferManager();
      //cout<<"out";*/
      
    const char* dbName = "testdb";
    StorageManager::createDB(dbName, 1000);
    
    cout<<"\n\n\nCreated and initialized db\n\n\n";
    
    //char* object = "btree1";
//    StorageManager::createObject("testdb", "btree1");
    /*StorageManager::createObject("testdb", "btree2");
    StorageManager::createObject("testdb", "btree3");
    StorageManager::createObject("testdb", "btree4");
    StorageManager::createObject("testdb", "btree5");
    StorageManager::createObject("testdb", "btree6");
    StorageManager::createObject("testdb", "btree7");
    
    
    StorageManager::dropObject("testdb", "btree4");
    cout<<"\n\nobject 4 deleted\n\n";*/
    
//    cout << "first block id for object 1 is : " << StorageManager::getFirstBlockId("testdb", "btree1") << endl;
//    cout << "first block id for object 5 is : " << StorageManager::getFirstBlockId("testdb", "btree5") << endl;
    //StorageManager::allocateBlock("testdb");
  /*  StorageManager::createObject("testdb", "btree8");
    cout << "first block id for object 8 is : " << StorageManager::getFirstBlockId("testdb", "btree8") << endl;
    
    cout<<"\n\n\nCreated object\n\n\n";
    
    int x = StorageManager::getFirstBlockId("testdb", "btree1");
    cout<<"Offset is:"<<x<<endl;*/

/*    cout << "first block id for object 3 is : " << StorageManager::getFirstBlockId("testdb", "btree3") << endl;
    cout << "first block id for object 6 is : " << StorageManager::getFirstBlockId("testdb", "btree6") << endl;
    cout << "first block id for object 8 is : " << StorageManager::getFirstBlockId("testdb", "btree8") << endl;
    
    //cout<<StorageManager::allocateBlock("testdb")<<endl;

    StorageManager::createObject("testdb", "daaiioo4");
    cout << "first block id for object 4 is : " << StorageManager::getFirstBlockId("testdb", "daaiioo4") << endl;
    
    StorageManager::closeDB("testdb");
    BufferManager* bm = BufferManager::getInstance();
    bm->~BufferManager();*/
    
    
    const char* indexName = "index1";
    BPlusTree* bt = new BPlusTree(dbName, indexName, true);
    
    cout<<"Index Created\n";
    
    bt->insertEntry(1,1);
    cout<<"Entries 1 1 Inserted\n";
    bt->insertEntry(2,2);
    cout<<"Entries 2 2 Inserted\n";
    bt->insertEntry(3,3);
    cout<<"Entries 3 3 Inserted\n";
    bt->insertEntry(4,4);
    cout<<"Entries 4 4 Inserted\n";
    
    cout<<"Entries Inserted\n";
    
    cout<<bt->lookup(4);
    cout<<bt->lookup(2);
    
    StorageManager::closeDB(dbName);
    BufferManager* bm = BufferManager::getInstance();
    bm->~BufferManager();

    
    return 0;
}

