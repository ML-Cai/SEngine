#ifndef SUT_BINARYSEARCHTREE_H
#define SUT_BINARYSEARCHTREE_H
// --------------------------------------------------------------------------
#include <pthread.h>
/** --------------------------------------------------------------------------
 * The common binary search tree.
 *  This tree is constructed as binary tree, and the data type of search key is defiend by <iTYPE> when constructed.
 */
template <typename iTYPE>
class sutBinarySearchTree {
public:
    //--------------------------------------------------------------------------
    struct sutBinaryTreeNode {
        iTYPE key;
        void *data;
        struct sutBinaryTreeNode *nodeR;
        struct sutBinaryTreeNode *nodeL;
    };

    struct sutBinaryTreeNodePage {
        struct sutBinaryTreeNode *page;
        struct sutBinaryTreeNodePage *next;
    };

    //--------------------------------------------------------------------------
    sutBinarySearchTree() { return; }
    //--------------------------------------------------------------------------
    ~sutBinarySearchTree() { release(); }
    //--------------------------------------------------------------------------
    int init() {
        pthread_mutexattr_t mattr;   // init mutex
        count = 0;
        root = NULL;
        pNodeList_Head = NULL;
        pNodeList_Tail = NULL;
        pResourceList = NULL;
        pthread_mutexattr_init(&mattr);
        pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_TIMED_NP);
        if(pthread_mutex_init(&mutex, &mattr) != 0) {
            printf("Fatal : sutPoolQueue mutex init error\n");
            return 0;
        }
        isInitial = DataStructMagicNumber;
        return 1;
    }
    //--------------------------------------------------------------------------
    void release() {
        if(isInit()) {
            struct sutBinaryTreeNodePage *rPage = pResourceList;
            this->lock();
            while(pResourceList) {
                rPage = pResourceList;
                pResourceList = pResourceList->next;
                free(rPage);
            }
            isInitial =0;
            this->unlock();
        }
    }
    /** --------------------------------------------------------------------------
     *  Add the data into BST
     *      @param inKey : The search key of input data ${inData};
     *      @param inData : The input data address.
    */
    int insert(iTYPE inKey, void *inData) {
        struct sutBinaryTreeNode *nNode = allocNode();
        struct sutBinaryTreeNode *pNode = root;
        struct sutBinaryTreeNode *pParent;

        if(nNode != NULL) {
            this->lock();
            nNode->key = inKey;
            nNode->data = inData;

            if(isEmpty()) {
                root = nNode;
            }
            else {
                while(pNode != NULL) {
                    pParent = pNode;
                    pNode = (inKey > pNode->key) ? pNode->nodeR : pNode->nodeL ;
                }
                if(inKey > pParent->key)
                    pParent->nodeR = nNode;
                else
                    pParent->nodeL = nNode;
            }
            count++;
            this->unlock();
            return 1;
        }
        return 0;
    }
    /** --------------------------------------------------------------------------
     *  Search the carried data by Key
     *      @param inKey : The key for search, the data type is defined when sutBinarySearchTree<> created.
     *      @return : NULL when search missing, otherwise the data pointer associated by key is returned.
     */
    void *search(iTYPE inKey) {
        this->lock();
        struct sutBinaryTreeNode *recRoot = searchNode(inKey);
        this->unlock();
        if(recRoot) return recRoot->data;
        return NULL;
    }
    /** --------------------------------------------------------------------------
     *  Remove the data by Key
     *      @param inKey : The key for remove, the data type is defined when sutBinarySearchTree<> created.
     */
    void remove(iTYPE inKey) {
        this->lock();
        struct sutBinaryTreeNode **delParent = &root;
        struct sutBinaryTreeNode *delNode = searchNode(inKey, &delParent);
        struct sutBinaryTreeNode *repNode;
        struct sutBinaryTreeNode **repParent;

        if(delNode) {
            if(delNode->nodeL == NULL && delNode->nodeR == NULL) {
                *delParent = NULL;
                RecycleNode(delNode);
            }
            else if (delNode->nodeL == NULL) {
                *delParent = delNode->nodeR;
                RecycleNode(delNode);
            }
            else if (delNode->nodeR == NULL) {
                *delParent = delNode->nodeL;
                RecycleNode(delNode);
            }
            else {
                repParent = &delNode->nodeL;
                repNode = searchNode_Biggest(delNode->nodeL, &repParent);
                delNode->data = repNode->data;
                delNode->key = repNode->key;
                *repParent = repNode->nodeL;
                RecycleNode(repNode);
            }
            count--;
        }
        this->unlock();
    }
    /** --------------------------------------------------------------------------
     *  Dump the information in BST
    */
    void dump() {
        recDump(root, 0);
    }

private :
    static const unsigned int DataStructMagicNumber = 0x5693;
    unsigned int isInitial;
    struct sutBinaryTreeNode *root;
    pthread_mutex_t mutex;
    int count;
    //--------------------------------------------------------------------------
    inline int isEmpty() { return (root == NULL); }
    //--------------------------------------------------------------------------
    inline int isInit() { return (isInitial == DataStructMagicNumber); }
    //--------------------------------------------------------------------------
    inline struct sutBinaryTreeNode *searchNode(iTYPE inKey) {
        struct sutBinarySearchTree<iTYPE>::sutBinaryTreeNode *recRoot = root;
        while(recRoot != NULL) {
            if(inKey == recRoot->key) {
                return recRoot;
            }
            else if(inKey > recRoot->key)
                recRoot = recRoot->nodeL;
            else if(inKey < recRoot->key)
                recRoot = recRoot->nodeR;
        }
        return NULL;
    }
    /** --------------------------------------------------------------------------
     * Search the Maxminum node by Key ${inKey}
     *      @param inKey : The search key.
     *      @param outParent : This parameter is used to pass the "parent nodeR/L" address of the returned node to the caller,
     *                         and used to remove or balance BST. The inital of this param doesn't required.
     * */
    inline struct sutBinaryTreeNode *searchNode(iTYPE inKey, sutBinaryTreeNode ***outParent) {
        struct sutBinarySearchTree<iTYPE>::sutBinaryTreeNode *retNode = root;
        struct sutBinarySearchTree<iTYPE>::sutBinaryTreeNode **retParent = &root;
        while(retNode != NULL) {
            if(inKey == retNode->key) {
                *outParent = retParent;
                return retNode;
            }
            else if(inKey < retNode->key) {
                retParent = &retNode->nodeL;
                retNode = retNode->nodeL;
            }
            else if(inKey > retNode->key) {
                retParent = &retNode->nodeR;
                retNode = retNode->nodeR;
            }
        }
        return NULL;
    }
    /** --------------------------------------------------------------------------
     * Search the Maxminum node below specify node ${inNode}
     *      @param inNode : The heade of search node .
     *      @param outParent : This parameter is used to pass the "parent nodeR/L" address of the returned node to the caller,
     *                         and used to remove or balance BST.
     *                         This parameter must initialize as the pointer, and point to one point which is pointed the address of inNode by caller.
     * */
    inline struct sutBinaryTreeNode *searchNode_Biggest(sutBinaryTreeNode *inNode, sutBinaryTreeNode ***outParent) {
        struct sutBinaryTreeNode **retParent = *outParent;
        do {
            if(inNode->nodeR == NULL) {
                *outParent = retParent;
                return inNode;
            }
            retParent = &inNode->nodeR;
            inNode = inNode->nodeR;
        } while(inNode != NULL);
        return NULL;
    }
    //--------------------------------------------------------------------------
    void recDump(struct sutBinaryTreeNode *inNode, int Level) {
        int Dl = Level;
        if(inNode != NULL) {
            while(Dl >0) {
                printf("\t");
                Dl--;
            }
            printf("Level %d, Key %d\n", Level, inNode->key);
            recDump(inNode->nodeR, Level +1);
            recDump(inNode->nodeL, Level +1);
        }
    }
    //--------------------------------------------------------------------------
    void lock() {
        if(pthread_mutex_lock(&mutex) != 0) {
            printf("Fatal : Lock error\n");
        }
    }
    //--------------------------------------------------------------------------
    void unlock() {
        pthread_mutex_unlock(&mutex);
    }

    // The resource handler
    static const int NodeRepository = 32;
    struct sutBinaryTreeNodePage *pResourceList;
    struct sutBinaryTreeNode *pActiveNode;
    struct sutBinaryTreeNode *pNodeList_Tail;
    struct sutBinaryTreeNode *pNodeList_Head;
    //--------------------------------------------------------------------------
    inline struct sutBinaryTreeNode *allocNode() {
        struct sutBinaryTreeNode *ret = NULL;

        if(pNodeList_Head == NULL) allocPage();
        if(pNodeList_Head != NULL) {    // if malloc failed , the allocPage may cause the pNodeList_Head as NULL
            ret = pNodeList_Head;
            pNodeList_Head = pNodeList_Head->nodeR;
            if(pNodeList_Head == NULL) pNodeList_Tail = NULL;
            ret->nodeL = NULL;
            ret->nodeR = NULL;
        }
        return ret;
    }
    //--------------------------------------------------------------------------
    int allocPage() {
        int i;
        struct sutBinaryTreeNodePage *nPage = (struct sutBinaryTreeNodePage *)malloc(sizeof(struct sutBinaryTreeNodePage));
        struct sutBinaryTreeNode *pNode;

        nPage->page = (struct sutBinaryTreeNode *)malloc(sizeof(sutBinaryTreeNode) * NodeRepository);
        if(nPage->page == NULL) {
            printf("Fatal : malloc failed\n");
            return 0;
        }
        pNode = nPage->page;
        for(i = 0 ; i < NodeRepository -1 ; i++) {
            pNode->nodeR = pNode +1;
            pNode++;
        }
        nPage->next = pResourceList;    // associate the new page with page list
        pResourceList = nPage;
        pNode->nodeR = NULL;    // associate the new page with node list
        if(pNodeList_Tail == NULL) pNodeList_Tail = pNode;  //the pNode in this condition is the last node allocated
        pNodeList_Head = nPage->page;
        return 1;
    }
    //--------------------------------------------------------------------------
    inline void RecycleNode(struct sutBinaryTreeNode *rNode) {
        if(rNode != NULL) {
            rNode->nodeR = pNodeList_Head;
            pNodeList_Head = rNode->nodeR;
            if(pNodeList_Tail == NULL) pNodeList_Tail = rNode;
        }
    }
};
//--------------------------------------------------------------------------
#endif // SUTBINARYSEARCHTREE_H
