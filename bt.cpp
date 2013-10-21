#include<cmath>
#include "bt.h"
#include "sputil.h"

void BPlusTree::deleteFromNode(TreeNode* N, int key) {

    BufferManager *bm = BufferManager::getInstance();

    //finding the location of pair to be deleted
    int i;
    for(i=1;i<=N->getTotalPairs();i++) {
        if(N->getKey(i)==key) break;
    }
    //deleting the pair
    moveBytes( N->getBuffPage(), 8*i, 8+8*i, (N->getTotalPairs()-i+1)*8+4 );
    N->updateSlot(1, N->getTotalPairs()-1);

    //if N is root AND has only one child
    if(N->getSlot(BLOCK_SIZE/4)==-1 && N->getSlot(1)==1) {
        //delete that node and make its only child as root
        int newRootId = N->getSlot(3);
        root = new TreeNode(bm->pinPage(dbName, N->getSlot(3)));
        int blockToFree = N->getSlot(BLOCK_SIZE/4-1);
        bm->unpinPage(dbName, blockToFree, true);
        StorageManager::freeBlock(dbName, blockToFree);
        root->updateSlot(BLOCK_SIZE/4, -1);
    }
    else {
        int parentPtr = readInt(N->getBuffPage(), BLOCK_SIZE-4);
        TreeNode* parent = new TreeNode(bm->pinPage(dbName, parentPtr));
        int loc=parent->findPtr(N->getNodeId());

        bool correct;
        int NDashId,keyDash;
        //finding next or previous node and the value between them 
        if(loc>1) {
            correct=true;
            NDashId = loc-1;
            keyDash = parent->getKey(loc-1);
        } else {
            correct=false;
            NDashId = loc+1;
            keyDash = parent->getKey(loc);
        }

        TreeNode* NDash = new TreeNode(bm->pinPage(dbName, NDashId));

        //if N and NDash can coalesce in one node
        if(N->getTotalPairs()+NDash->getTotalPairs()-1 == BT_N) {
            //swapping if successive nodes r in reverse order
            if(correct==false) {
                TreeNode* temp;
                temp = N;
                N = NDash;
                NDash = temp;
            }
    
            //coalescing nodes
            if(!N->isLeaf()) {
                int num = NDash->getTotalPairs();
                NDash->updateSlot(2*NDash->getTotalPairs()+2, keyDash);
                for(int i=0;i<(N->getTotalPairs()*2);i++) {
                    NDash->updateSlot(2*NDash->getTotalPairs()+2+i, N->getSlot(i+2));
                    NDash->updateSlot(1,NDash->getTotalPairs()+num);
                }
            }
            else {
                int num = NDash->getTotalPairs();
                NDash->updateSlot(2*NDash->getTotalPairs()+2, keyDash);
                for(int i=0;i<(N->getTotalPairs()*2);i++)
                    NDash->updateSlot(2*NDash->getTotalPairs()+2+i, N->getSlot(i+2));
                NDash->updateSlot(1,NDash->getTotalPairs()+num);
                NDash->updateSlot(2+2*BT_N+1,N->getSlot(2+2*BT_N+1));
            }
            deleteFromNode(parent, keyDash);
        }

        //if NDash is predecessor
        if(correct==true){
            if(!N->isLeaf()) {
                //removing last key and pointer to next leaf
                int lastKey = NDash->getSlot(2+NDash->getTotalPairs()*2);
                int lastPtr = NDash->getSlot(2+NDash->getTotalPairs()*2+1);
                NDash->updateSlot(1,NDash->getTotalPairs()+1);
                //adding one pair to N
                moveBytes(N->getBuffPage(), 16, 8, N->getTotalPairs()*8+4);
                N->updateSlot(1, N->getTotalPairs()+1);
                N->updateSlot(3, lastPtr);
                N->updateSlot(4, keyDash);
                //replacing keyDash in parent by the removed key
                int location = parent->findKey(keyDash);
                parent->updateSlot(2+2*location, lastKey);
            } else {
                //removing last key and pointer to next leaf
                int lastKey = NDash->getSlot(2+NDash->getTotalPairs()*2);
                int lastPtr = NDash->getSlot(2+NDash->getTotalPairs()*2-1);
                NDash->updateSlot(1,NDash->getTotalPairs()+1);
                //adding one pair to N
                moveBytes(N->getBuffPage(), 16, 8, N->getTotalPairs()*8);
                N->updateSlot(1, N->getTotalPairs()+1);
                N->updateSlot(3, lastPtr);
                N->updateSlot(4, keyDash);
                //replacing keyDash in parent by the removed key
                int location = parent->findKey(keyDash);
                parent->updateSlot(2+2*location, lastKey);
            }
        } else {
            if(!NDash->isLeaf()) {
                //removing last key and pointer to next leaf
                int lastKey = N->getSlot(2+N->getTotalPairs()*2);
                int lastPtr = N->getSlot(2+N->getTotalPairs()*2+1);
                N->updateSlot(1,N->getTotalPairs()+1);
                //adding one pair to NDash
                moveBytes(NDash->getBuffPage(), 16, 8, NDash->getTotalPairs()*8+4);
                NDash->updateSlot(1, NDash->getTotalPairs()+1);
                NDash->updateSlot(3, lastPtr);
                NDash->updateSlot(4, keyDash);
                //replacing keyDash in parent by the removed key
                int location = parent->findKey(keyDash);
                parent->updateSlot(2+2*location, lastKey);
            } else {
                //removing last key and pointer to next leaf
                int lastKey = N->getSlot(2+N->getTotalPairs()*2);
                int lastPtr = N->getSlot(2+N->getTotalPairs()*2-1);
                N->updateSlot(1,N->getTotalPairs()+1);
                //adding one pair to NDash
                moveBytes(NDash->getBuffPage(), 16, 8, NDash->getTotalPairs()*8);
                NDash->updateSlot(1, NDash->getTotalPairs()+1);
                NDash->updateSlot(3, lastPtr);
                NDash->updateSlot(4, keyDash);
                //replacing keyDash in parent by the removed key
                int location = parent->findKey(keyDash);
                parent->updateSlot(2+2*location, lastKey);
            }
        }
    }
}

void BPlusTree::deleteEntry(int key) {

    BufferManager *bm = BufferManager::getInstance();

    //lookup till the required node
    TreeNode* c = root;
    while( c->isLeaf() ) {
        int sKey = -1;
        int finalKey = -1;
        for(int i=0; i<(c->getTotalPairs()); i++){
            sKey=c->getKey(i);
            if(sKey>key){
                finalKey=i;
                break;
            }
        }
        if(finalKey==-1) {
            int m=c->getTotalPairs();
            
            c = new TreeNode(bm->pinPage(dbName, c->getRid(m)));
        }
        else
            c = new TreeNode(bm->pinPage(dbName, c->getRid(finalKey)));
    }

    deleteFromNode(c, key);

}

void BPlusTree::insert_in_leaf( TreeNode* L, int key, int rid) {

    BufferManager *bm = BufferManager::getInstance();

    if(key < L->getKey(1)) {
        cout<<"inif2"<<endl;
        //inserting the key-pointer pair at the start of the node
        moveBytes( L->getBuffPage(), 8, 16, (L->getTotalPairs())*8 );
        writeInt( L->getBuffPage(), 0, L->getTotalPairs() + 1 );
        cout<<readInt( L->getBuffPage(), 0)<<endl;
    }
    else {
        cout<<"inelse2"<<endl;
        int totPairs = L->getTotalPairs();
        int i = totPairs;

        //finding the location to insert
        while(key<L->getKey(i)){
            cout<<"inwhile\n";
            i--;
            if(key>L->getKey(i))
                break;
        }
        cout<<"outofwhile\n";
        i++;
        //inserting the key-pointer pair at the correct location
        cout<<i<<" "<<L->getTotalPairs()<<"\n";
        moveBytes( L->getBuffPage(), 16+8*i, 8+8*i, (L->getTotalPairs())-i );
        cout<<"moved\n";
        writeInt( L->getBuffPage(), 8+8*i, rid );
        cout<<"write\n";
        writeInt( L->getBuffPage(), 12+8*i, key );
        cout<<"write1\n";
        writeInt( L->getBuffPage(), 0, L->getTotalPairs() + 1 );
        cout<<"written all data";
    }
}

void BPlusTree::insert_in_parent( TreeNode* N, int key, TreeNode* NDash) {

    BufferManager *bm = BufferManager::getInstance();

    if(N->isRoot()) {
        //creating new node
        int newNodeId = StorageManager::allocateBlock(dbName);
        int* newNodeIdPtr = &newNodeId;
        TreeNode * R = new TreeNode(bm->pinPage(dbName, newNodeId));
        writeInt( R->getBuffPage(), 0, 1 );
        writeInt( R->getBuffPage(), 4, 2 );
        writeInt( R->getBuffPage(), 8, N->getNodeId() );
        writeInt( R->getBuffPage(), 12, key );
        writeInt( R->getBuffPage(), 16, NDash->getNodeId() );
        writeInt( R->getBuffPage(), BLOCK_SIZE-4, -1 );
        writeInt( R->getBuffPage(), BLOCK_SIZE-8, newNodeId );
        
        writeInt( N->getBuffPage(), BLOCK_SIZE-4, R->getNodeId() );
    }
    TreeNode* P = new TreeNode(bm->pinPage(dbName, N->getParent()));
    if(P->getTotalPairs() < BT_N) {
        //finding the location of pointer to N in the parent
        int i;
        for(i=1;i<=P->getTotalPairs();i++) {
            if(P->getKey(i)==N->getNodeId()) break;
        }
        //inserting the pair in parent p just after N
        writeInt( P->getBuffPage(), 0, P->getTotalPairs()+1 );
        moveBytes(P->getBuffPage(), 8*P->getTotalPairs()-(8+8*i)+4, 8+8*i, 8);//****
        writeInt( P->getBuffPage(), 8+8*i, NDash->getNodeId() );//****
        writeInt( P->getBuffPage(), 12+8*i, key );//****
    } else {
        //allocating temporary memory T
        char * T = (char *)calloc(BLOCK_SIZE+8,sizeof(char));
        memcpy(T, P->getBuffPage(), 12+8*BT_N);

        //finding the location of pointer to N in the parent
        int i;
        for(i=1; i <= ((TreeNode*)T)->getTotalPairs(); i++) {
            if(P->getKey(i)==N->getNodeId()) break;
        }
        //inserting the pair in temp memory
        moveBytes(T, 8+8*i+8, 8+8*i,(P->getTotalPairs()-i)*8);
        writeInt(T, 8+8*i, NDash->getNodeId());
        writeInt(T, 12+8*i, key);

        //temp
        int temp = ceil(BT_N/2);
        int* tempPtr = &temp;

        //keep only first half of T in P
        memcpy(P->getBuffPage(), T, 8+8*temp);
        writeInt(P->getBuffPage(), 4,temp);
        int newKey = P->getKey(temp);

        //new node
        int PDashId = StorageManager::allocateBlock(dbName);
        int* PDashIdPtr = &PDashId;
        TreeNode * PDash = new TreeNode(bm->pinPage(dbName, PDashId));
        writeInt(PDash->getBuffPage(), BLOCK_SIZE-4, readInt(P->getBuffPage(), BLOCK_SIZE-4));
        writeInt(PDash->getBuffPage(), BLOCK_SIZE-8, PDashId);

        //copying to new node
        memcpy(PDash->getBuffPage(), T+8+8*temp, 8*BT_N-8*temp+4);
        writeInt(PDash->getBuffPage(), 4, BT_N-temp);
        insert_in_parent(P,newKey,PDash);
    }
}

void BPlusTree::insertEntry(int key, int rid) {
cout<<"a\n";
    BufferManager *bm = BufferManager::getInstance();

    TreeNode* c = root;
cout<<"b\n";
    while( c->isLeaf() ) {
cout<<"a1\n";
        int sKey = -1;
        int finalKey = -1;
        for(int i=0; i<(c->getTotalPairs()); i++){
cout<<"a2\n";
            sKey=c->getKey(i);
            if(sKey>key){
                finalKey=i;
                break;
            }
        }
        if(finalKey==-1) {
            int m=c->getTotalPairs();
            c = new TreeNode(bm->pinPage(dbName, c->getRid(m)));
cout<<"a3\n";
        }
        else
        cout<<"aelse\n";
            c = new TreeNode(bm->pinPage(dbName, c->getRid(finalKey)));
    }
cout<<"a\n";
/*    int keyExists = -1;
    int position;
    for(int i=0; i<(c->getTotalPairs()); i++){
        if(key == c->getKey(i)){
            keyExists = key;
            position=i;
        }
    }
*/    

    if(c->getTotalPairs() < BT_N){
        cout<<"inif 1\n";
        insert_in_leaf(c, key, rid);
        cout<<"inserted in leaf\n";
    }
    else {
        cout<<"inelse 1\n";
        int temp;
        int* tempPtr = &temp;
        //creating new node
        int newNodeId = StorageManager::allocateBlock(dbName);
        int* newNodeIdPtr = &newNodeId;
        TreeNode * newNode = new TreeNode(bm->pinPage(dbName, newNodeId));
        writeInt(newNode->getBuffPage(), BLOCK_SIZE-4, readInt(c->getBuffPage(), BLOCK_SIZE-4));
        writeInt(newNode->getBuffPage(), BLOCK_SIZE-8, newNodeId);
        //creating temp node & copying data to temp node
        char *tempForTempNode = (char *)calloc(BLOCK_SIZE,sizeof(char));
        memcpy(tempForTempNode, c->getBuffPage(), 8+8*BT_N);
        TreeNode * tempNode = new TreeNode(tempForTempNode);

        insert_in_leaf(newNode, key, rid);

        //add new node newNode in the linked list
        memcpy( (newNode->getBuffPage())+(8+BT_N*8), (c->getBuffPage())+(8+BT_N*8), 4);
        memcpy( (c->getBuffPage())+(8+BT_N*8), newNodeIdPtr, 4);

        //no erasing, but overwriting
        //write first half of tempNode into original one
        temp = ceil((float)(BT_N/2));
        memcpy( (c->getBuffPage())+8, tempNode->getBuffPage()+8, 8*temp);
        memcpy( c->getBuffPage(), tempPtr, 4);

        //copy second half into newNode
        memcpy( (newNode->getBuffPage())+8, (tempNode->getBuffPage())+(8+8*temp), 8*(BT_N - temp));
        temp = BT_N - temp;
        memcpy( newNode->getBuffPage(), tempPtr, 4);
        temp = 1;
        memcpy( (newNode->getBuffPage())+4, tempPtr, 4);

        //taking smallest key of new node to update(insert) in parent
        int smallestKey = newNode->getRid(1);
        insert_in_parent(c, smallestKey, newNode);
    }
}



int BPlusTree::lookup(int key) {

    BufferManager *bm = BufferManager::getInstance();

    TreeNode* c = root;
    while( c->isLeaf() ) {
        int sKey = -1;
        int finalKey = -1;
        for(int i=0; i<(c->getTotalPairs()); i++){
            sKey=c->getKey(i);
            if(sKey>key){
                finalKey=i;
                break;
            }
        }
        if(finalKey==-1) {
            int m=c->getTotalPairs();
            c = new TreeNode(bm->pinPage(dbName, c->getRid(m)));
        }
        else
            c = new TreeNode(bm->pinPage(dbName, c->getRid(finalKey)));
    }
    int keyExists = -1;
    int position;
    for(int i=0; i<(c->getTotalPairs()); i++){
        if(key == c->getKey(i)){
            keyExists = key;
            position=i;
        }
    }
    if(keyExists != -1 ){
        return c->getRid(position);
    }
    else
    {
        return -1;
    }
}
