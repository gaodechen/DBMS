#include "sm.h"

class TreeNode {
    private:
    char *buffPage;

    public:
    TreeNode(char *buffPage) {
        this->buffPage = buffPage;
    }

    char *getBuffPage() {
        return buffPage;
    }

    int getKey(int position){
        int key=readInt(buffPage,12+8*position);
        return key;
    }

    int getRid(int position){
        int id=readInt(buffPage,8+8*position);
        return id;
    }

    int getTotalPairs(){
        int num=readInt(buffPage,0);
        return num;
    }

    bool isLeaf(){
        int num=readInt(buffPage,4);
        if(num==0) return false;
        else if(num==1) return true;
    }

    bool isRoot(){
        int num=readInt(buffPage,BLOCK_SIZE-4);
        if(num==-1) return true;
        else return false;
    }

    //update int value at the i th int slot STARTING FROM ONE, NOT ZERO
    void updateSlot(int slotNum, int newVal) {
        writeInt(buffPage, (slotNum-1)*4, newVal);
    }

    //get int value at the i th int slot STARTING FROM ONE, NOT ZERO
    int getSlot(int slotNum) {
        readInt(buffPage, (slotNum-1)*4);
    }

    //get the pointer to the parent node. Is '-1' for the root node
    int getParent() {
        int num=readInt(buffPage,BLOCK_SIZE-4);
        return num;
    }

    //get blockId of the node
    int getNodeId() {
        int num=readInt(buffPage,BLOCK_SIZE-8);
        return num;
    }

    //toggles the node between leaf and non-leaf
    void toggle() {
        int val = readInt(buffPage,4);
        if(val==0)
            val = 1;
        else if(val==1)
            val = 0;
        int * valPtr = &val;
        memcpy(buffPage,valPtr,4);
    }

    //finding the location of key from the start in that node
    int findKey(int val) {
        int i;
        for(i=1;i<=getTotalPairs();i++) {
            if(getKey(i)==val) break;
        }
        return i;
    }

    //finding the location of pointer from the start in that node
    int findPtr(int val) {
        int i;
        for(i=1;i<=getTotalPairs();i++) {
            if(getRid(i)==val) break;
        }
        return i;
    }

};


class BPlusTree {

    TreeNode *root;

    const char* dbName;

    public:

    BPlusTree(const char *dbName, const char *indexName, bool create) {
        BufferManager *bm = BufferManager::getInstance();
        
        this->dbName = dbName;
        
        if(create) {
            StorageManager::createObject(dbName, indexName);
        }
        
        int rootBlockId = StorageManager::getFirstBlockId(dbName, indexName);
        cout<<"rootBlockId : "<<rootBlockId<<endl;
                                  

        char *rootNodeBuff = bm->pinPage(dbName, rootBlockId);
        writeInt(rootNodeBuff, 0, 0);
        writeInt(rootNodeBuff, 4, 1);
        writeInt(rootNodeBuff, BLOCK_SIZE-4, -1);
        writeInt(rootNodeBuff, BLOCK_SIZE-8, rootBlockId);
        root = new TreeNode(rootNodeBuff);
    }


    /**
     * Insert an entry into the index.
     */
    void insertEntry(int key, int rid);

    void insert_in_parent( TreeNode* N, int key, TreeNode* NDash);

    void insert_in_leaf( TreeNode* L, int key, int rid);

    /**
     * Delete an entry from the index. For now, assume the index
     * to be on a candidate key.
     */
    void deleteEntry(int key);

    void deleteFromNode(TreeNode* N, int key);


    /**
     * Index lookup for a key. Returns the rid if found, else -1.
     * Assuming the index on a candidate key.
     */
    int lookup(int key);


   /**
     * Destructor. Just force all the dirty buffer pages to disk.
     */
    ~BPlusTree() {
    }
};
